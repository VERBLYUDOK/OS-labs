#include "TDeterminantCalculator.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

TDeterminantCalculator::TTask::TTask(const TMatrix& Mat, double e)
    : Matrix(Mat), element(e) {}

TDeterminantCalculator::TDeterminantCalculator(const TMatrix& Matrix, int numThreads)
    : Matrix_(Matrix), numThreads_(numThreads), globalDeterminant_(0.0), activeTasks_(0), stop_(false) {
    pthread_mutex_init(&queueMutex_, nullptr);
    pthread_cond_init(&queueCond_, nullptr);
    pthread_mutex_init(&determinantMutex_, nullptr);
    pthread_mutex_init(&activeTasksMutex_, nullptr);
    pthread_cond_init(&activeTasksCond_, nullptr);
}

TDeterminantCalculator::~TDeterminantCalculator() {
    pthread_mutex_destroy(&queueMutex_);
    pthread_cond_destroy(&queueCond_);
    pthread_mutex_destroy(&determinantMutex_);
    pthread_mutex_destroy(&activeTasksMutex_);
    pthread_cond_destroy(&activeTasksCond_);
}

void TDeterminantCalculator::PushTask(TTask* Task) {
    pthread_mutex_lock(&queueMutex_);
    taskQueue_.push(Task);
    pthread_cond_signal(&queueCond_);
    pthread_mutex_unlock(&queueMutex_);
}

bool TDeterminantCalculator::PopTask(TTask*& Task) {
    pthread_mutex_lock(&queueMutex_);
    while (taskQueue_.empty() && !stop_) {
        pthread_cond_wait(&queueCond_, &queueMutex_);
    }
    if (stop_ && taskQueue_.empty()) {
        pthread_mutex_unlock(&queueMutex_);
        return false;
    }
    Task = taskQueue_.front();
    taskQueue_.pop();
    pthread_mutex_unlock(&queueMutex_);
    return true;
}

void TDeterminantCalculator::Shutdown() {
    pthread_mutex_lock(&queueMutex_);
    stop_ = true;
    pthread_cond_broadcast(&queueCond_);
    pthread_mutex_unlock(&queueMutex_);
}

TMatrix TDeterminantCalculator::GetMinor(const TMatrix& Matrix, int row, int col) {
    TMatrix Minor;
    int n = Matrix.size();
    for (int i = 0; i < n; ++i) {
        if (i == row) {
            continue;
        }
        std::vector<double> minorRow;
        for (int j = 0; j < n; ++j) {
            if (j == col) {
                continue;
            }
            minorRow.push_back(Matrix[i][j]);
        }
        Minor.push_back(minorRow);
    }
    return Minor;
}

// Статическая функция-обёртка для потоков
void* TDeterminantCalculator::WorkerHelper(void* arg) {
    return ((TDeterminantCalculator*)arg)->Worker();
}

// Основная функция потока
void* TDeterminantCalculator::Worker() {
    while (true) {
        TTask* Task;
        if (!PopTask(Task)) {
            break; // Очередь остановлена и задач больше нет
        }

        int n = Task->Matrix.size();

        if (n == 1) {
            // Базовый случай
            double det = Task->element * Task->Matrix[0][0];
            pthread_mutex_lock(&determinantMutex_);
            globalDeterminant_ += det;
            pthread_mutex_unlock(&determinantMutex_);
        } else {
            // Создаем подзадачи
            int n_subtasks = n;
            pthread_mutex_lock(&activeTasksMutex_);
            activeTasks_ += n_subtasks;
            pthread_mutex_unlock(&activeTasksMutex_);

            for (int j = 0; j < n; ++j) {
                TMatrix Minor = GetMinor(Task->Matrix, 0, j); // Раскладываем по первой строке
                double element = Task->Matrix[0][j];
                int sign = ((j % 2) == 0) ? 1 : -1;

                // Вычисляем новое накопленное значение элемента
                double new_element = Task->element * element * sign;

                // Создаем новую задачу и добавляем ее в очередь
                TTask* newTask = new TTask(Minor, new_element);
                PushTask(newTask);
            }
        }

        pthread_mutex_lock(&activeTasksMutex_);
        activeTasks_--;
        int remaining_tasks = activeTasks_;
        if (remaining_tasks == 0) {
            // Уведомляем главный поток о завершении всех задач
            pthread_cond_signal(&activeTasksCond_);
        }
        pthread_mutex_unlock(&activeTasksMutex_);

        delete Task;
    }
    return nullptr;
}

double TDeterminantCalculator::Compute() {
    globalDeterminant_ = 0.0;
    activeTasks_ = 0;
    stop_ = false;

    // Создание потоков
    std::vector<pthread_t> threads(numThreads_);
    for (int i = 0; i < numThreads_; ++i) {
        int ret = pthread_create(&threads[i], nullptr, &TDeterminantCalculator::WorkerHelper, this);
        if (ret != 0) {
            std::cerr << "Ошибка создания потока: " << strerror(ret) << std::endl;
            exit(1);
        }
    }

    pthread_mutex_lock(&activeTasksMutex_);
    activeTasks_ = 1; // Корневая задача
    pthread_mutex_unlock(&activeTasksMutex_);

    TTask* InitialTask = new TTask(Matrix_, 1.0);
    PushTask(InitialTask);

    // Ожидаем завершения всех задач
    pthread_mutex_lock(&activeTasksMutex_);
    while (activeTasks_ > 0) {
        pthread_cond_wait(&activeTasksCond_, &activeTasksMutex_);
    }
    pthread_mutex_unlock(&activeTasksMutex_);

    Shutdown(); // Останавливаем очередь задач

    // Ожидаем завершения всех потоков
    for (int i = 0; i < numThreads_; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return globalDeterminant_;
}

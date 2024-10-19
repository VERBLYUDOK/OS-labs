#include "determinant_calculator.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

// Конструктор задачи
DeterminantCalculator::Task::Task(const Matrix& mat, double e)
    : matrix(mat), element(e) {}

// Конструктор класса
DeterminantCalculator::DeterminantCalculator(const Matrix& matrix, int num_threads)
    : matrix_(matrix), num_threads_(num_threads), global_determinant_(0.0), active_tasks_(0), stop_(false) {
    // Инициализация мьютексов и условных переменных
    pthread_mutex_init(&queue_mutex_, nullptr);
    pthread_cond_init(&queue_cond_, nullptr);
    pthread_mutex_init(&determinant_mutex_, nullptr);
    pthread_mutex_init(&active_tasks_mutex_, nullptr);
    pthread_cond_init(&active_tasks_cond_, nullptr);
}

// Деструктор класса
DeterminantCalculator::~DeterminantCalculator() {
    // Уничтожение мьютексов и условных переменных
    pthread_mutex_destroy(&queue_mutex_);
    pthread_cond_destroy(&queue_cond_);
    pthread_mutex_destroy(&determinant_mutex_);
    pthread_mutex_destroy(&active_tasks_mutex_);
    pthread_cond_destroy(&active_tasks_cond_);
}

// Добавить задачу в очередь
void DeterminantCalculator::pushTask(Task* task) {
    pthread_mutex_lock(&queue_mutex_);
    task_queue_.push(task);
    pthread_cond_signal(&queue_cond_);
    pthread_mutex_unlock(&queue_mutex_);
}

// Извлечь задачу из очереди
bool DeterminantCalculator::popTask(Task*& task) {
    pthread_mutex_lock(&queue_mutex_);
    while (task_queue_.empty() && !stop_) {
        pthread_cond_wait(&queue_cond_, &queue_mutex_);
    }
    if (stop_ && task_queue_.empty()) {
        pthread_mutex_unlock(&queue_mutex_);
        return false;
    }
    task = task_queue_.front();
    task_queue_.pop();
    pthread_mutex_unlock(&queue_mutex_);
    return true;
}

// Завершить работу очереди задач
void DeterminantCalculator::shutdown() {
    pthread_mutex_lock(&queue_mutex_);
    stop_ = true;
    pthread_cond_broadcast(&queue_cond_);
    pthread_mutex_unlock(&queue_mutex_);
}

// Вычислить минор матрицы
Matrix DeterminantCalculator::getMinor(const Matrix& matrix, int row, int col) {
    Matrix minor;
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        if (i == row) continue;
        std::vector<double> minorRow;
        for (int j = 0; j < n; ++j) {
            if (j == col) continue;
            minorRow.push_back(matrix[i][j]);
        }
        minor.push_back(minorRow);
    }
    return minor;
}

// Статическая функция-обёртка для потоков
void* DeterminantCalculator::worker_helper(void* arg) {
    return ((DeterminantCalculator*)arg)->worker();
}

// Основная функция потока
void* DeterminantCalculator::worker() {
    while (true) {
        Task* task;
        if (!popTask(task)) {
            break; // Очередь остановлена и задач больше нет
        }

        int n = task->matrix.size();

        if (n == 1) {
            // Базовый случай: вычисляем вклад в определитель
            double det = task->element * task->matrix[0][0];

            // Защищаем доступ к глобальному определителю
            pthread_mutex_lock(&determinant_mutex_);
            global_determinant_ += det;
            pthread_mutex_unlock(&determinant_mutex_);
        } else {
            // Создаем подзадачи
            int n_subtasks = n;

            // Увеличиваем счетчик активных задач
            pthread_mutex_lock(&active_tasks_mutex_);
            active_tasks_ += n_subtasks;
            pthread_mutex_unlock(&active_tasks_mutex_);

            for (int j = 0; j < n; ++j) {
                Matrix minor = getMinor(task->matrix, 0, j);
                double element = task->matrix[0][j];
                int sign = ((j % 2) == 0) ? 1 : -1;

                // Вычисляем новое накопленное значение элемента
                double new_element = task->element * element * sign;

                // Создаем новую задачу
                Task* newTask = new Task(minor, new_element);

                // Добавляем задачу в очередь
                pushTask(newTask);
            }
        }

        // Уменьшаем счетчик активных задач
        pthread_mutex_lock(&active_tasks_mutex_);
        active_tasks_--;
        int remaining_tasks = active_tasks_;
        if (remaining_tasks == 0) {
            // Уведомляем главный поток о завершении всех задач
            pthread_cond_signal(&active_tasks_cond_);
        }
        pthread_mutex_unlock(&active_tasks_mutex_);

        // Освобождаем память
        delete task;
    }
    return nullptr;
}

// Метод для вычисления определителя
double DeterminantCalculator::compute() {
    global_determinant_ = 0.0;
    active_tasks_ = 0;
    stop_ = false;

    // Создание потоков
    std::vector<pthread_t> threads(num_threads_);
    for (int i = 0; i < num_threads_; ++i) {
        int ret = pthread_create(&threads[i], nullptr, &DeterminantCalculator::worker_helper, this);
        if (ret != 0) {
            std::cerr << "Ошибка создания потока: " << strerror(ret) << std::endl;
            exit(1);
        }
    }

    // Устанавливаем счетчик активных задач
    pthread_mutex_lock(&active_tasks_mutex_);
    active_tasks_ = 1; // Корневая задача
    pthread_mutex_unlock(&active_tasks_mutex_);

    // Создаем корневую задачу
    Task* initialTask = new Task(matrix_, 1.0);

    // Добавляем задачу в очередь
    pushTask(initialTask);

    // Ожидаем завершения всех задач
    pthread_mutex_lock(&active_tasks_mutex_);
    while (active_tasks_ > 0) {
        pthread_cond_wait(&active_tasks_cond_, &active_tasks_mutex_);
    }
    pthread_mutex_unlock(&active_tasks_mutex_);

    // Останавливаем очередь задач
    shutdown();

    // Ожидаем завершения всех потоков
    for (int i = 0; i < num_threads_; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return global_determinant_;
}

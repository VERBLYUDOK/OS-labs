#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <cstdlib>
#include <cstring>

typedef std::vector<std::vector<double>> Matrix;

// Функция для ввода матрицы
Matrix inputMatrix() {
    int n;
    std::cout << "Введите размер матрицы (n x n): ";
    std::cin >> n;

    Matrix matrix(n, std::vector<double>(n));
    std::cout << "Введите элементы матрицы построчно:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "Строка " << i + 1 << ": ";
        for (int j = 0; j < n; ++j) {
            std::cin >> matrix[i][j];
        }
    }

    return matrix;
}

class DeterminantCalculator {
public:
    DeterminantCalculator(const Matrix& matrix, int num_threads)
        : matrix_(matrix), num_threads_(num_threads), global_determinant_(0.0), active_tasks_(0), stop_(false) {
        // Инициализация мьютексов и условных переменных
        pthread_mutex_init(&queue_mutex_, nullptr);
        pthread_cond_init(&queue_cond_, nullptr);
        pthread_mutex_init(&determinant_mutex_, nullptr);
        pthread_mutex_init(&cout_mutex_, nullptr);
        pthread_cond_init(&active_tasks_cond_, nullptr);
        pthread_mutex_init(&active_tasks_mutex_, nullptr);
    }

    ~DeterminantCalculator() {
        // Уничтожение мьютексов и условных переменных
        pthread_mutex_destroy(&queue_mutex_);
        pthread_cond_destroy(&queue_cond_);
        pthread_mutex_destroy(&determinant_mutex_);
        pthread_mutex_destroy(&cout_mutex_);
        pthread_cond_destroy(&active_tasks_cond_);
        pthread_mutex_destroy(&active_tasks_mutex_);
    }

    double compute();

private:
    // Матрица для вычисления
    const Matrix& matrix_;
    // Количество потоков
    int num_threads_;
    // Глобальный определитель
    double global_determinant_;
    // Счетчик активных задач
    int active_tasks_;
    // Структура задачи
    struct Task {
        Matrix matrix;
        double element;

        Task(const Matrix& mat, double e)
            : matrix(mat), element(e) {}
    };
    // Очередь задач
    std::queue<Task*> task_queue_;
    // Мьютексы и условные переменные
    pthread_mutex_t queue_mutex_;
    pthread_cond_t queue_cond_;
    pthread_mutex_t determinant_mutex_;
    pthread_mutex_t cout_mutex_;
    pthread_cond_t active_tasks_cond_;
    pthread_mutex_t active_tasks_mutex_;
    // Флаг для остановки потоков
    bool stop_;

    // Внутренние методы
    static void* worker_helper(void* arg);
    void* worker();
    void pushTask(Task* task);
    bool popTask(Task*& task);
    void shutdown();
    // Функция для вычисления минора
    Matrix getMinor(const Matrix& matrix, int row, int col);
};

void DeterminantCalculator::pushTask(Task* task) {
    pthread_mutex_lock(&queue_mutex_);
    task_queue_.push(task);
    pthread_cond_signal(&queue_cond_);
    pthread_mutex_unlock(&queue_mutex_);
}

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

void DeterminantCalculator::shutdown() {
    pthread_mutex_lock(&queue_mutex_);
    stop_ = true;
    pthread_cond_broadcast(&queue_cond_);
    pthread_mutex_unlock(&queue_mutex_);
}

void* DeterminantCalculator::worker_helper(void* arg) {
    return ((DeterminantCalculator*)arg)->worker();
}

void* DeterminantCalculator::worker() {
    while (true) {
        Task* task;
        if (!popTask(task)) {
            break; // Очередь остановлена и задач больше нет
        }

        int n = task->matrix.size();

        // Отладочный вывод
        pthread_mutex_lock(&cout_mutex_);
        std::cout << "Поток " << pthread_self() << " начал обработку задачи с элементом " << task->element << ". Размер матрицы: " << n << std::endl;
        pthread_mutex_unlock(&cout_mutex_);

        if (n == 1) {
            // Базовый случай: вычисляем вклад в определитель
            double det = task->element * task->matrix[0][0];

            // Защищаем доступ к глобальному определителю
            pthread_mutex_lock(&determinant_mutex_);
            global_determinant_ += det;
            pthread_mutex_unlock(&determinant_mutex_);

            // Отладочный вывод
            pthread_mutex_lock(&cout_mutex_);
            std::cout << "Поток " << pthread_self() << " добавил вклад в определитель: " << det << std::endl;
            pthread_mutex_unlock(&cout_mutex_);
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

                // Отладочный вывод
                pthread_mutex_lock(&cout_mutex_);
                std::cout << "Поток " << pthread_self() << " создал подзадачу с элементом " << newTask->element << std::endl;
                pthread_mutex_unlock(&cout_mutex_);
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

double DeterminantCalculator::compute() {
    // Инициализация переменных
    global_determinant_ = 0.0;
    active_tasks_ = 0;
    stop_ = false;

    // Создание потоков
    std::vector<pthread_t> threads(num_threads_);
    for (int i = 0; i < num_threads_; ++i) {
        int ret = pthread_create(&threads[i], nullptr, &DeterminantCalculator::worker_helper, this);
        if (ret != 0) {
            std::cerr << "Ошибка создания потока: " << std::strerror(ret) << std::endl;
            std::exit(EXIT_FAILURE);
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

Matrix DeterminantCalculator::getMinor(const Matrix& matrix, int row, int col) {
    Matrix minor;
    int n = matrix.size();
    for (int i = 1; i < n; ++i) { // Пропускаем первую строку
        std::vector<double> minorRow;
        for (int j = 0; j < n; ++j) {
            if (j != col) { // Пропускаем столбец
                minorRow.push_back(matrix[i][j]);
            }
        }
        minor.push_back(minorRow);
    }
    return minor;
}

int main(int argc, char* argv[]) {
    // Обработка аргументов командной строки
    if (argc != 3 || std::string(argv[1]) != "-t") {
        std::cerr << "Usage: " << argv[0] << " -t <max_threads>" << std::endl;
        return EXIT_FAILURE;
    }
    int num_threads = std::stoi(argv[2]);
    if (num_threads <= 0) {
        std::cerr << "Ошибка: число потоков должно быть положительным" << std::endl;
        return EXIT_FAILURE;
    }

    // Ввод матрицы пользователем
    Matrix matrix = inputMatrix();

    // Создание объекта калькулятора и вычисление определителя
    DeterminantCalculator calculator(matrix, num_threads);
    double determinant = calculator.compute();

    // Вывод результата
    std::cout << "Определитель: " << determinant << std::endl;

    return EXIT_SUCCESS;
}

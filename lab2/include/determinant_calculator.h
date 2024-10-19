#ifndef DETERMINANT_CALCULATOR_H
#define DETERMINANT_CALCULATOR_H

#include "matrix.h"
#include <queue>
#include <pthread.h>


class DeterminantCalculator {
public:
    // Конструктор
    DeterminantCalculator(const Matrix& matrix, int num_threads);
    
    // Деструктор
    ~DeterminantCalculator();
    
    // Метод для вычисления определителя
    double compute();
    
private:
    struct Task {
        Matrix matrix;
        double element;

        Task(const Matrix& mat, double e);
    };
    
    // Статическая функция-обёртка для потоков
    static void* worker_helper(void* arg);
    
    // Основная функция потока
    void* worker();
    
    // Методы QueueTask
    void pushTask(Task* task);
    bool popTask(Task*& task);
    void shutdown();

    Matrix getMinor(const Matrix& matrix, int row, int col);

    const Matrix& matrix_;
    int num_threads_;
    double global_determinant_;
    int active_tasks_;
    bool stop_;
    
    std::queue<Task*> task_queue_;
    
    // Синхронизация
    pthread_mutex_t queue_mutex_;
    pthread_cond_t queue_cond_;
    pthread_mutex_t determinant_mutex_;
    pthread_mutex_t active_tasks_mutex_;
    pthread_cond_t active_tasks_cond_;
};

#endif

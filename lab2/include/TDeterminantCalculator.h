#ifndef TDETERMINANT_CALCULATOR_H
#define TDETERMINANT_CALCULATOR_H

#include "TMatrix.h"
#include <queue>
#include <pthread.h>


class TDeterminantCalculator {
public:
    TDeterminantCalculator(const TMatrix& Matrix, int numThreads);
    
    ~TDeterminantCalculator();
    
    // Метод для вычисления определителя
    double Compute();
    
private:
    // Задача для потока
    struct TTask {
        TMatrix Matrix;
        double element;

        TTask(const TMatrix& Mat, double e);
    };
    
    // Статическая функция-обёртка для потоков
    static void* WorkerHelper(void* arg);
    
    // Основная функция потока
    void* Worker();
    
    // Методы для QueueTask
    void PushTask(TTask* Task);
    bool PopTask(TTask*& Task);
    void Shutdown();

    TMatrix GetMinor(const TMatrix& Matrix, int row, int col);

    const TMatrix& Matrix_;
    int numThreads_;
    double globalDeterminant_;
    int activeTasks_;
    bool stop_;
    
    std::queue<TTask*> taskQueue_;
    
    // Синхронизация
    pthread_mutex_t queueMutex_;
    pthread_cond_t queueCond_;
    pthread_mutex_t determinantMutex_;
    pthread_mutex_t activeTasksMutex_;
    pthread_cond_t activeTasksCond_;
};

#endif

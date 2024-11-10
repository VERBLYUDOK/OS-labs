#ifndef PARENT_H
#define PARENT_H

#include <iostream>
#include <iomanip>   // setprecision()
#include <fstream>   // ofstream
#include <unistd.h>  // fork(), sleep()
#include <sys/wait.h> // wait()
#include <sys/mman.h> // mmap(), munmap()
#include <sys/stat.h> // fstat()
#include <fcntl.h>    // O_CREAT, O_RDWR
#include <cstring>    // memcpy(), strlen()
#include <semaphore.h> // sem_t, sem_init(), sem_wait(), sem_post()
#include <cstdlib>    // exit()

#define SHARED_FILE "/tmp/shared_memory_file"
#define SHARED_SIZE 4096

struct SharedData {
    sem_t sem_parent; // Семафор для родителя
    sem_t sem_child;  // Семафор для ребенка
    char fileName[256];
    float numbers[100];
    int count;
    float sum;
};

void RunParentProcess(std::istream&);

#endif
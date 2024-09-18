#ifndef LAB1_H
#define LAB1_H

#include <iostream>
#include <cstdio>
#include <fstream> // Для работы с файлами через std::ofstream
#include <unistd.h> // Для системных вызовов POSIX (fork(), pipe(), dup2(), read(), write())
#include <sys/wait.h> // wait()
#include <cstdlib> // exit()
#include <cstring> // strlen()

void run_parent_process();

#endif
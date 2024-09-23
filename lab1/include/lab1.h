#ifndef LAB1_H
#define LAB1_H

#include <iostream>
#include <cstdio> // perror()
#include <iomanip> // setprecision()
#include <fstream> // Для работы с файлами через std::ofstream и std::ifstream
#include <unistd.h> // Для системных вызовов POSIX (fork(), pipe(), dup2(), read(), write())
#include <sys/wait.h> // wait()
#include <cstdlib> // exit()

void run_parent_process(std::istream&);

#endif
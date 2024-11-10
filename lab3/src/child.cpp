#include "lab3.h"

int main() {
    int fd = open(SHARED_FILE, O_RDWR, 0666);
    if (fd == -1) {
        std::cerr << "Ошибка открытия общего файла в дочернем процессе" << std::endl;
        exit(1);
    }

    // Отображаем файл в память
    SharedData* shared = (SharedData*)mmap(nullptr, SHARED_SIZE,
                                           PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        std::cerr << "Ошибка mmap в дочернем процессе" << std::endl;
        close(fd);
        exit(1);
    }
    close(fd);

    // Ждем сигнал от родительского процесса
    sem_wait(&shared->sem_child);

    // Читаем данные
    char fileName[256];
    strcpy(fileName, shared->fileName);

    float sum = 0;
    for (int i = 0; i < shared->count; ++i) {
        sum += shared->numbers[i];
    }

    // Записываем сумму в файл
    {
        std::ofstream file(fileName);
        if (!file.is_open()) {
            std::cerr << "Ошибка при открытии файла" << std::endl;
            exit(1);
        }

        file << std::fixed << std::setprecision(6);
        file << "Сумма: " << sum << '\n';
    }

    // Записываем результат в общую память
    shared->sum = sum;

    // Сигнализируем родительскому процессу о завершении
    sem_post(&shared->sem_parent);

    // Удаляем отображение памяти
    munmap(shared, sizeof(SharedData));

    return 0;
}
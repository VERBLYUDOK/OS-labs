#include "lab1.h"

int main() {
    // Чтение имени файла из pipe1 (STDIN)
    char fileName[100];
    read(STDIN_FILENO, fileName, 100);

    // Открытие файла для записи
    int fd = open(fileName, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        perror("Ошибка при открытии файла");
        exit(1);
    }

    // Чтение чисел из stdin и подсчет суммы
    int num, sum = 0;
    printf("Введите числа (EOF для завершения): ");
    while (scanf("%d", &num) == 1) {
        sum += num;
    }

    // Запись суммы в файл
    dprintf(fd, "Сумма: %d\n", sum);
    close(fd);

    // Передача результата родительскому процессу через pipe2 (STDOUT)
    dprintf(STDOUT_FILENO, "Сумма: %d", sum);

    return 0;
}

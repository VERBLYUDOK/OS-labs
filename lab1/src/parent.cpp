#include "lab1.h"

void run_parent_process() {
    int pipe1[2], pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Ошибка создания pipe");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка fork");
        exit(1);
    }

    if (pid > 0) {
        // Родительский процесс

        // Закрывваем неиспользуемое родительским процессом
        close(pipe1[0]);
        close(pipe2[1]);
        char fileName[50];
        std::cout << "Введите имя файла:\n";
        std::cin >> fileName;

        // Отправляем имя файла дочернему процессу
        write(pipe1[1], &fileName, strlen(fileName) + 1);
        //write(pipe1[1], &fileName, 50);
        sleep(1);

        // Отправляем числа дочернему процессу
        float num;
        std::cout << "Введите числа (EOF для завершения):\n";
        while (std::cin >> num) {
            write(pipe1[1], &num, sizeof(num));
        }
        close(pipe1[1]);

        // Ждем завершения дочернего процесса
        wait(NULL);

        // Получаем результат из pipe2
        char buffer[100];
        int bytesRead = read(pipe2[0], buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Завершаем строку
        }
        close(pipe2[0]);

        std::cout << "\nРезультат от дочернего процесса: " << buffer;
    } else {
        // Дочерний процесс

        // Закрываем неиспользуемое
        close(pipe1[1]);
        close(pipe2[0]);

        dup2(pipe1[0], 0); // Чтобы принять fileName через пайп
        dup2(pipe2[1], 1); // Чтобы передать результат родителю через пайп
        close(pipe1[0]);  // Закрываем конец чтения pipe1, он больше не нужен, раз мы переопределили вход
        close(pipe2[1]);  // Закрываем конец записи pipe2

        // Дочерний процесс через exec
        execlp("bin/child", "bin/child", NULL);

        perror("Ошибка exec");
        exit(1);
    }
}

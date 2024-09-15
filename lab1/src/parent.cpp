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

        char fileName[100];
        std::cout << "Введите имя файла: ";
        std::cin >> fileName;

        // Отправляем имя файла дочернему процессу
        write(pipe1[1], &fileName, strlen(fileName) + 1);

        // Отправляем числа дочернему процессу
        float num;
        std::cout << "Введите числа (EOF для завершения): ";
        while (std::cin >> num) {
            write(pipe1[1], &num, sizeof(num));
        }
        
        close(pipe1[1]);

        // Ждем завершения дочернего процесса
        wait(NULL);

        // Получаем результат из pipe2
        char buffer[100];
        read(pipe2[0], buffer, 100);
        close(pipe2[0]);

        std::cout << "Результат от дочернего процесса: " << buffer << std::endl;
    } else {
        // Дочерний процесс

        dup2(pipe1[0], 0); // Чтобы принять fileName через пайп
        dup2(pipe2[1], 1); // Чтобы передать результат родителю через пайп
        close(pipe1[0]);  // Закрываем конец чтения pipe1
        close(pipe2[1]);  // Закрываем конец записи pipe2

        // Дочерний процесс через exec
        char *args[] = {"./child", NULL};
        execvp(args[0], args);
        perror("Ошибка exec");
        exit(1);
    }
}

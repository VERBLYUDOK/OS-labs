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
        close(pipe1[0]);  // Закрываем конец чтения pipe1
        close(pipe2[1]);  // Закрываем конец записи pipe2

        char fileName[100];
        printf("Введите имя файла: ");
        scanf("%s", fileName);

        // Отправляем имя файла дочернему процессу
        write(pipe1[1], fileName, strlen(fileName) + 1);
        close(pipe1[1]);

        // Ждем завершения дочернего процесса
        wait(NULL);

        // Получаем результат из pipe2
        char buffer[100];
        read(pipe2[0], buffer, 100);
        close(pipe2[0]);

        printf("Результат от дочернего процесса: %s\n", buffer);
    } else {
        // Дочерний процесс через exec
        char *args[] = {"./child", NULL};
        execvp(args[0], args);
        perror("Ошибка exec");
        exit(1);
    }
}

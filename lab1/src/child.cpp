#include "lab1.h"

int main() {
    // Чтение имени файла из pipe1 (STDIN)
    char fileName[100];
    read(0, fileName, 100);

    // Открытие файла для записи
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла" << std::endl;
        exit(1);
    }

    // Чтение чисел из pipe1 и подсчет суммы
    float num, sum = 0;
    while (read(0, &num, sizeof(num)) > 0) {
        sum += num;
    }

    // Запись суммы в файл
    file << "Сумма: " << sum << std::endl;
    file.close();  // Закрываем файл

    // Передача результата родительскому процессу через pipe2 (STDOUT)
    std::cout << "Сумма: " << sum << std::endl;
    fflush(stdout); // Очищаем стандартный вывод

    return 0;
}
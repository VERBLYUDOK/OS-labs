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
    // if (read(0, &num, sizeof(num)) == 0) {
    //     std::cout << "я урод";
    // } else {
    //     std::cout << "я x2урод";
    // }
    // while (read(0, &num, sizeof(num)) > 0) {
    //     sum += num;
    // }
    read(0, &num, sizeof(num));
    sum += num;
    read(0, &num, sizeof(num));
    sum += num;

    // Запись суммы в файл
    file << "Сумма: " << sum << '\n';
    file.close();  // Закрываем файл

    // Передача результата родительскому процессу через pipe2 (STDOUT)
    std::cout << "Сумма: " << sum << '\n';

    return 0;
}

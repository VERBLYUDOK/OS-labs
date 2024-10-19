#include <iostream>
#include "determinant_calculator.h"
#include "matrix.h"


int main(int argc, char* argv[]) {
    // Обработка аргументов командной строки
    if (argc != 3 || std::string(argv[1]) != "-t") {
        std::cerr << "Использование: " << argv[0] << " -t <число_потоков>" << std::endl;
        return 1;
    }
    
    int num_threads;
    try {
        num_threads = std::stoi(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: неверное значение числа потоков." << std::endl;
        return 1;
    }

    if (num_threads <= 0) {
        std::cerr << "Ошибка: число потоков должно быть положительным." << std::endl;
        return 1;
    }

    // Ввод матрицы пользователем
    Matrix matrix = inputMatrix();

    // Создание объекта калькулятора и вычисление определителя
    DeterminantCalculator calculator(matrix, num_threads);
    double determinant = calculator.compute();

    // Вывод результата
    std::cout << "Определитель: " << determinant << std::endl;

    return 0;
}

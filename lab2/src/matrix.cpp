#include <iostream>
#include "matrix.h"

Matrix inputMatrix() {
    int n;
    std::cout << "Введите размер матрицы (n x n): ";
    std::cin >> n;

    if (n <= 0) {
        std::cerr << "Размер матрицы должен быть положительным." << std::endl;
        exit(EXIT_FAILURE);
    }

    Matrix matrix(n, std::vector<double>(n));
    std::cout << "Введите элементы матрицы построчно:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "Строка " << i + 1 << ": ";
        for (int j = 0; j < n; ++j) {
            std::cin >> matrix[i][j];
        }
    }

    return matrix;
}
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

extern "C" { // Для обеспечения совместимости при динамической загрузке функций.
    float Derivative(float A, float deltaX);
    int* Sort(int* array, int size);
}

#endif

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

extern "C" { // Совместимость для динамической загрузки
    float Derivative(float A, float deltaX);
    int* Sort(int* array, int size);
}

#endif

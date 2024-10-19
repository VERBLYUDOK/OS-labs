add_executable(lab2 main.cpp src/determinant_calculator.cpp include/determinant_calculator.h src/matrix.cpp include/matrix.h)

target_include_directories(lab2 PRIVATE include)
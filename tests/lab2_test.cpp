#include <determinant_calculator.h>
#include <matrix.h>
#include <gtest/gtest.h>
#include <chrono>


// Проверка корректности однопоточного варианта
TEST(DeterminantCalculatorTest, SingleThreadCorrectness) {
    // 1x1 матрица
    Matrix mat1 = {{5}};
    DeterminantCalculator calc1(mat1, 1);
    double det1 = calc1.compute();
    EXPECT_DOUBLE_EQ(det1, 5.0);

    // 2x2 матрица
    Matrix mat2 = {
        {1, 2},
        {3, 4}
    };
    DeterminantCalculator calc2(mat2, 1);
    double det2 = calc2.compute();
    EXPECT_DOUBLE_EQ(det2, -2.0);

    // 3x3 матрица
    Matrix mat3 = {
        {6, 1, 1},
        {4, -2, 5},
        {2, 8, 7}
    };
    DeterminantCalculator calc3(mat3, 1);
    double det3 = calc3.compute();
    EXPECT_DOUBLE_EQ(det3, -306.0);

    // 4x4 матрица
    Matrix mat4 = {
        {3, 2, 0, 1},
        {4, 0, 1, 2},
        {3, 0, 2, 1},
        {9, 2, 3, 1}
    };
    DeterminantCalculator calc4(mat4, 1);
    double det4 = calc4.compute();
    EXPECT_DOUBLE_EQ(det4, 24.0);
}

// Проверка соответствия результатов однопоточного и многопоточного вариантов
TEST(DeterminantCalculatorTest, SingleVsMultiThreadConsistency) {
    // 3x3 матрица
    Matrix mat = {
        {6, 1, 1},
        {4, -2, 5},
        {2, 8, 7}
    };

    // Однопоточный вариант
    DeterminantCalculator calc_single(mat, 1);
    double det_single = calc_single.compute();

    // Многопоточный вариант (например, 4 потока)
    DeterminantCalculator calc_multi(mat, 4);
    double det_multi = calc_multi.compute();

    EXPECT_DOUBLE_EQ(det_single, det_multi);
}

// Проверка производительности многопоточного варианта
TEST(DeterminantCalculatorTest, MultiThreadPerformance) {
    // 7x7 матрица
    Matrix mat = {
        {1, 0, 2, -1, 3, 2, 0},
        {3, 0, 0, 5, 1, 1, 1},
        {2, 1, 4, -3, 2, -1, 3},
        {1, 0, 5, 0, 0, 1, 0},
        {4, 1, -2, 2, 1, 3, -2},
        {0, 1, 0, 0, 1, 3, -2},
        {1, 1, -2, 5, 1, 0, 1}
    };

    // Измеряем время для однопоточного варианта
    DeterminantCalculator calc_single(mat, 1);
    auto start_single = std::chrono::high_resolution_clock::now();
    double det_single = calc_single.compute();
    auto end_single = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_single = end_single - start_single;

    // Измеряем время для многопоточного варианта (например, 8 потоков)
    DeterminantCalculator calc_multi(mat, 8);
    auto start_multi = std::chrono::high_resolution_clock::now();
    double det_multi = calc_multi.compute();
    auto end_multi = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_multi = end_multi - start_multi;

    // Проверяем, что результаты совпадают
    EXPECT_DOUBLE_EQ(det_single, det_multi);

    // Проверяем, что многопоточный вариант быстрее
    std::cout << "Однопоточный вариант: " << duration_single.count() << " секунд\n";
    std::cout << "Многопоточный вариант: " << duration_multi.count() << " секунд\n";
    
    EXPECT_LT(duration_multi.count(), duration_single.count());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

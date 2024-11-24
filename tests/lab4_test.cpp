#include <gtest/gtest.h>
#include <dlfcn.h>
#include <cmath>
#include <functions.h>

TEST(DerivativeTest, Implementation1) {
    // Тестируем Derivative из lib1
    float A = 0.5f;
    float deltaX = 0.01f;
    float result = Derivative(A, deltaX);
    float expected = (cosf(A + deltaX) - cosf(A)) / deltaX;
    EXPECT_NEAR(result, expected, 1e-5);
}

TEST(DerivativeTest, Implementation2) {
    // Тестируем Derivative из lib2
    const char* pathToLib2 = std::getenv("PATH_TO_LIB2");
    if (!pathToLib2) {
        std::cerr << "Переменная окружения PATH_TO_LIB2 не установлена" << std::endl;
        return exit(1);
    }

    void* handle = dlopen(pathToLib2, RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    using DerivativeFunc = float(*)(float, float);
    DerivativeFunc DerivativeLib2 = reinterpret_cast<DerivativeFunc>(dlsym(handle, "Derivative"));
    ASSERT_NE(DerivativeLib2, nullptr);

    float A = 0.5f;
    float deltaX = 0.01f;
    float result = DerivativeLib2(A, deltaX);
    float expected = (cosf(A + deltaX) - cosf(A - deltaX)) / (2 * deltaX);
    EXPECT_NEAR(result, expected, 1e-5);

    dlclose(handle);
}

TEST(SortTest, Implementation1) {
    // Тестируем Sort из lib1
    int array[] = {5, 2, 3, 1, 4};
    int expected[] = {1, 2, 3, 4, 5};
    int size = sizeof(array) / sizeof(array[0]);

    int* result = Sort(array, size);

    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(SortTest, Implementation2) {
    // Тестируем Sort из lib2
    const char* pathToLib2 = std::getenv("PATH_TO_LIB2");
    if (!pathToLib2) {
        std::cerr << "Переменная окружения PATH_TO_LIB2 не установлена" << std::endl;
        return exit(1);
    }

    void* handle = dlopen(pathToLib2, RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    using SortFunc = int*(*)(int*, int);
    SortFunc SortLib2 = reinterpret_cast<SortFunc>(dlsym(handle, "Sort"));
    ASSERT_NE(SortLib2, nullptr);

    int array[] = {5, 2, 3, 1, 4};
    int expected[] = {1, 2, 3, 4, 5};
    int size = sizeof(array) / sizeof(array[0]);

    int* result = SortLib2(array, size);

    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }

    dlclose(handle);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

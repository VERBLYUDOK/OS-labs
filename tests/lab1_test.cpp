#include <gtest/gtest.h>
#include <lab1.h>

TEST(ParentProcessTest, CheckSumCalculation) {
    const char* testFileName = "output.txt";
    
    // Ввод для родительского процесса
    std::ofstream test_input("test_input.txt");
    test_input << testFileName << "\n7.6 5.5";
    test_input.close();
    std::ifstream test_file("test_input.txt");
    ASSERT_TRUE(test_file.is_open()) << "Не удалось открыть файл";

    // Запуск родительского процесса
    run_parent_process(test_file);  // Передаем поток для тестирования
    test_file.close();

    // Проверяем, что файл был создан программой
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();
    EXPECT_EQ(line, "Сумма: 13.100000") << "Неверный результат суммы в файле";

    // Смываем
    std::remove(testFileName);
    std::remove("test_input.txt");
}

TEST(ParentProcessTest, EmptyInput) {
    const char* testFileName = "empty_test_file.txt";

    // Ввод для родительского процесса
    std::ofstream test_input("empty_input.txt");
    test_input << testFileName;
    test_input.close();
    std::ifstream test_file("empty_input.txt");
    ASSERT_TRUE(test_file.is_open()) << "Не удалось открыть файл";

    // Запуск родительского процесса
    run_parent_process(test_file);  // Передаем поток для тестирования
    test_file.close();

    // Проверяем, что файл был создан программой
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();
    EXPECT_EQ(line, "Сумма: 0.000000") << "Неверный результат для пустого ввода";

    // Удаляем тестовые файлы
    std::remove(testFileName);
    std::remove("empty_input.txt");
}

TEST(ParentProcessTest, LargeNumbersInput) {
    const char* testFileName = "large_test_file.txt";
    
    // Ввод для родительского процесса
    std::ofstream test_input("large_input.txt");
    test_input << testFileName << "\n6.1234 5.230011 1.2 3 .1";
    test_input.close();
    std::ifstream test_file("large_input.txt");
    ASSERT_TRUE(test_file.is_open()) << "Не удалось открыть файл";

    // Запускаем родительский процесс
    run_parent_process(test_file);  // Передаем поток для тестирования
    test_file.close();

    // Проверяем, что файл был создан программой
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();
    EXPECT_EQ(line, "Сумма: 15.653411") << "Неверный результат для больших чисел";

    // Удаляем тестовый файл
    std::remove(testFileName);
    std::remove("large_input.txt");
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
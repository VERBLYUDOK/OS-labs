#include <gtest/gtest.h>
#include <lab1.h>

TEST(ParentProcessTest, CheckSumCalculation) {
    const char* testFileName = "output.txt";
    
    // Ввод
    FILE* input = freopen("test_input.txt", "r", stdin); // Перенаправляем stdin на файл test_input.txt
    fprintf(input, "%s\n7.6 5.5", testFileName);

    // Запускаем родительский процесс
    run_parent_process();

    // Проверяем, что файл был создан
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();
    EXPECT_EQ(line, "Сумма: 13.1") << "Неверный результат суммы в файле";

    fclose(input);

    // Смываем за собой
    std::remove(testFileName);
    std::remove("test_input.txt");
}
TEST(ParentProcessTest, EmptyInput) {
    const char* testFileName = "empty_test_file.txt";
    
    // Ввод
    FILE* input = freopen("empty_input.txt", "r", stdin);
    fprintf(input, "%s\n", testFileName);

    // Запускаем родительский процесс
    run_parent_process();

    // Проверяем, что файл был создан
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();
    EXPECT_EQ(line, "Сумма: 0") << "Неверный результат для пустого ввода";

    std::remove(testFileName);
    std::remove("empty_input.txt");
}
TEST(ParentProcessTest, LargeNumbersInput) {
    const char* testFileName = "large_test_file.txt";
    
    // Ввод
    FILE* input = freopen("large_input.txt", "r", stdin);
    fprintf(input, "%s\n1000000.1 2000000.2", testFileName);

    // Запускаем родительский процесс
    run_parent_process();

    // Проверяем, что файл был создан
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();
    EXPECT_EQ(line, "Сумма: 3000000.3") << "Неверный результат для больших чисел";

    std::remove(testFileName);
    std::remove("large_input.txt");
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
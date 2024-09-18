#include <gtest/gtest.h>
#include <lab1.h>

TEST(ParentProcessTest, CheckSumCalculation) {
    const char* testFileName = "output.txt";
    
    // Ввод для родительского процесса
    //FILE* input = freopen("test_input.txt", "w+", stdin); // freopen для переопределения stdin
    std::ofstream test_input("test_input.txt");
    test_input << testFileName << '\n';  // Название файла для вывода
    test_input << "7.6 5.5";     // Ввод чисел (0 для завершения)
    test_input.close();

    // Перенаправляем стандартный ввод на файл test_input.txt
    std::ifstream input("test_input.txt");
    std::streambuf* cinbuf = std::cin.rdbuf(); // Сохраняем оригинальный std::cin
    std::cin.rdbuf(input.rdbuf()); // Перенаправляем std::cin на файл

    // Запускаем родительский процесс
    run_parent_process();

    std::cin.rdbuf(cinbuf);
    input.close();

    // Проверяем, что файл был создан
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();

    // Проверяем корректность результата
    EXPECT_EQ(line, "Сумма: 13.100000") << "Неверный результат суммы в файле";

    // Удаляем тестовый файл
    //std::remove(testFileName);
}
TEST(ParentProcessTest, EmptyInput) {
    // Устанавливаем переменные
    const char* testFileName = "empty_test_file.txt";
    
    // Ввод для родительского процесса
    FILE* input = freopen("empty_input.txt", "w+", stdin);
    fprintf(input, "%s\n\n", testFileName);

    // Запускаем родительский процесс
    run_parent_process();

    // Проверяем, что файл был создан
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();

    // Проверяем корректность результата
    EXPECT_EQ(line, "Сумма: 0.000000") << "Неверный результат для пустого ввода";

    // Удаляем тестовый файл
    std::remove(testFileName);
}
TEST(ParentProcessTest, LargeNumbersInput) {
    // Устанавливаем переменные
    const char* testFileName = "large_test_file.txt";
    
    // Ввод для родительского процесса
    FILE* input = freopen("large_input.txt", "w+", stdin);
    fprintf(input, "%s\n1000000.1 2000000.2\n", testFileName);

    // Запускаем родительский процесс
    run_parent_process();

    // Проверяем, что файл был создан
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();

    // Проверяем корректность результата
    EXPECT_EQ(line, "Сумма: 3000000.300000") << "Неверный результат для больших чисел";

    // Удаляем тестовый файл
    std::remove(testFileName);
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
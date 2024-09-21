#include <gtest/gtest.h>
#include <lab1.h>

class ParentProcessTest : public ::testing::Test {
protected:
    int saved_stdin;

    // Метод, который сохраняет текущий stdin и переопределяет его на новый файл
    void SetUp() override {
        saved_stdin = dup(STDIN_FILENO); // Сохраняем текущее значение stdin
    }

    // Метод, который восстанавливает исходное значение stdin
    void TearDown() override {
        dup2(saved_stdin, STDIN_FILENO); // Восстанавливаем сохранённый stdin
        close(saved_stdin); // Закрываем временный дескриптор
    }
};

TEST(ParentProcessTest, CheckSumCalculation) {
    const char* testFileName = "output.txt";
    
    // Ввод для родительского процесса
    std::ofstream test_input("test_input.txt");
    test_input << testFileName << "\n7.6 5.5";
    test_input.close();

    // Перенаправляем стандартный ввод на файл test_input.txt
    // FILE* input = freopen("test_input.txt", "r", stdin);

    // Переопределяем stdin на файл test_input.txt
    int input_fd = open("test_input.txt", O_RDONLY);
    ASSERT_NE(input_fd, -1) << "Не удалось открыть файл для ввода";

    // Заменяем stdin на этот файл
    dup2(input_fd, STDIN_FILENO);
    close(input_fd); // Закрываем исходный файловый дескриптор

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

    //fclose(input);

    // Смываем
    //std::remove(testFileName);
    //std::remove("test_input.txt");
}

TEST(ParentProcessTest, EmptyInput) {
    const char* testFileName = "empty_test_file.txt";

    // Ввод для родительского процесса
    std::ofstream test_input("empty_input.txt");
    test_input << testFileName;
    test_input.close();
    
    // Ввод для родительского процесса
    //FILE* input = freopen("empty_input.txt", "r", stdin);

    // Переопределяем stdin на файл test_input.txt
    int input_fd = open("empty_input.txt", O_RDONLY);
    ASSERT_NE(input_fd, -1) << "Не удалось открыть файл для ввода";

    // Заменяем stdin на этот файл
    dup2(input_fd, STDIN_FILENO);
    close(input_fd); // Закрываем исходный файловый дескриптор

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

    //fclose(input);

    // Удаляем тестовый файл
    //std::remove(testFileName);
    //std::remove("empty_input.txt");
}
TEST(ParentProcessTest, LargeNumbersInput) {
    const char* testFileName = "large_test_file.txt";
    
    // Ввод для родительского процесса
    std::ofstream test_input("large_input.txt");
    test_input << testFileName << "\n1000000.1234 2000000.2345";
    test_input.close();

    //FILE* input = freopen("large_input.txt", "r", stdin);

    // Переопределяем stdin на файл test_input.txt
    int input_fd = open("large_input.txt", O_RDONLY);
    ASSERT_NE(input_fd, -1) << "Не удалось открыть файл для ввода";

    // Заменяем stdin на этот файл
    dup2(input_fd, STDIN_FILENO);
    close(input_fd); // Закрываем исходный файловый дескриптор

    // Запускаем родительский процесс
    run_parent_process();

    // Проверяем, что файл был создан
    std::ifstream resultFile(testFileName);
    ASSERT_TRUE(resultFile.good()) << "Файл не был создан";

    // Читаем результат
    std::string line;
    std::getline(resultFile, line);
    resultFile.close();
    EXPECT_EQ(line, "Сумма: 3000000.3579") << "Неверный результат для больших чисел";

    //fclose(input);

    // Удаляем тестовый файл
    //std::remove(testFileName);
    //std::remove("large_input.txt");
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
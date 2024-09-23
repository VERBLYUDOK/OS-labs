add_executable(lab1_test lab1_test.cpp ../lab1/include/lab1.h ../lab1/src/parent.cpp)

# Установка Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  TLS_VERIFY false
)
FetchContent_MakeAvailable(googletest)
target_link_libraries(lab1_test PRIVATE gtest_main)

target_include_directories(lab1_test PRIVATE ../lab1/include)

add_test(lab1_test lab1_test)
include(GoogleTest)
gtest_discover_tests(lab1_test)
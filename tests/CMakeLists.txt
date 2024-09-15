add_executable(lab1_test lab1_test.cpp ../lab1/include/lab1.h ../lab1/src/parent.cpp ../lab1/src/parent.cpp)

# Установка Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  TLS_VERIFY false
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)
target_link_libraries(lab1_test PRIVATE GTest::GTest)

target_include_directories(lab1_test PRIVATE ../lab1/include)

add_test(lab1_test lab1_test)
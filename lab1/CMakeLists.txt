add_executable(main main.cpp src/parent.cpp src/child.cpp include/lab1.h)

target_include_directories(main PRIVATE include)
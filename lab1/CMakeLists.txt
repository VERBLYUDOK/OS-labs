add_executable(main main.cpp src/parent.cpp include/lab1.h)
add_executable(child src/child.cpp include/lab1.h)

target_include_directories(main PRIVATE include)
target_include_directories(child PRIVATE include)
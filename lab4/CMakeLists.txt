add_library(lib1 SHARED lib1.cpp)
set_target_properties(lib1 PROPERTIES OUTPUT_NAME "liblib1")

add_library(lib2 SHARED lib2.cpp)
set_target_properties(lib2 PROPERTIES OUTPUT_NAME "liblib2")

add_executable(program1 program1.cpp)
add_executable(program2 program2.cpp)

target_link_libraries(program1 PRIVATE lib1 m)
target_link_libraries(program2 PRIVATE dl m)

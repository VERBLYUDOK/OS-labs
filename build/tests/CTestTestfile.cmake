# CMake generated Testfile for 
# Source directory: /workspaces/OS-labs/tests
# Build directory: /workspaces/OS-labs/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/workspaces/OS-labs/build/tests/lab1_test[1]_include.cmake")
add_test(lab1_test "lab1_test")
set_tests_properties(lab1_test PROPERTIES  _BACKTRACE_TRIPLES "/workspaces/OS-labs/tests/CMakeLists.txt;16;add_test;/workspaces/OS-labs/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
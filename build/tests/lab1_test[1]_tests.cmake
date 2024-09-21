add_test([=[ParentProcessTest.CheckSumCalculation]=]  /workspaces/OS-labs/build/bin/lab1_test [==[--gtest_filter=ParentProcessTest.CheckSumCalculation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ParentProcessTest.CheckSumCalculation]=]  PROPERTIES WORKING_DIRECTORY /workspaces/OS-labs/build/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ParentProcessTest.EmptyInput]=]  /workspaces/OS-labs/build/bin/lab1_test [==[--gtest_filter=ParentProcessTest.EmptyInput]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ParentProcessTest.EmptyInput]=]  PROPERTIES WORKING_DIRECTORY /workspaces/OS-labs/build/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[ParentProcessTest.LargeNumbersInput]=]  /workspaces/OS-labs/build/bin/lab1_test [==[--gtest_filter=ParentProcessTest.LargeNumbersInput]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ParentProcessTest.LargeNumbersInput]=]  PROPERTIES WORKING_DIRECTORY /workspaces/OS-labs/build/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  lab1_test_TESTS ParentProcessTest.CheckSumCalculation ParentProcessTest.EmptyInput ParentProcessTest.LargeNumbersInput)
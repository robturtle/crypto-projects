enable_testing ()

# simple example code of using gtest
add_executable (gtest_worked gtest.cpp)
target_link_libraries (gtest_worked gtest gtest_main)
add_test (gtest-worked gtest_worked)

# simple example code of using cpp quickcheck
add_executable (cpp_quickcheck_worked quickcheck.cpp)
target_link_libraries (cpp_quickcheck_worked cppqc)
add_test (cpp-quickcheck-worked cpp_quickcheck_worked)

# example of using quickcheck for custom class
add_executable (quickcheck_choose_generator choose_generator.cpp)
target_link_libraries (quickcheck_choose_generator cppqc)
add_test (quickcheck-choose-generator quickcheck_choose_generator)

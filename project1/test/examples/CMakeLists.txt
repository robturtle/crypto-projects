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
add_executable (quickcheck_register_arbitrary register_arbitrary.cpp)
target_link_libraries (quickcheck_register_arbitrary cppqc)
add_test (quickcheck-register-arbitrary quickcheck_register_arbitrary)

# example of using quickcheck generators
add_executable (quickcheck_generators quickcheck_generators.cpp)
target_link_libraries (quickcheck_generators cppqc)
add_test (quickcheck-generators quickcheck_generators)

# example of using setup library
add_executable (setup_test setup_test.cpp)
target_link_libraries (setup_test setup)
add_test (setup-test setup_test)

# example of loading and analysis dictionaries
add_executable (analyze_dictionaries analyze_dictionaries.cpp)
target_link_libraries (analyze_dictionaries setup)

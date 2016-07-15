enable_testing ()
################################################################################
# Test code breaker implementation
################################################################################
add_executable (CodeBreakerTest CodeBreakerTest.cpp)
target_link_libraries (CodeBreakerTest gtest gtest_main cppqc code_breaker)

# for single test
add_executable (ciphertext_sample ciphertext_sample.cpp)
target_link_libraries (ciphertext_sample setup)

# for experiments
add_executable (experiments experiments.cpp)
target_link_libraries (experiments code_breaker)

################################################################################
# test for fixtures
################################################################################
option (fixture-test "Test for sample generators" ON)

if (fixture-test)

  add_executable (KeyTest KeyTest.cpp)
  target_link_libraries (KeyTest gtest gtest_main cppqc setup)
  add_test (key-test KeyTest)

  add_executable (SchedulerTest SchedulerTest.cpp)
  target_link_libraries (SchedulerTest gtest gtest_main cppqc setup)
  add_test (scheduler-test SchedulerTest)
endif ()


################################################################################
# my library function use exmaples
################################################################################
option (testlib-examples "Example code of using gtest and CppQuickCheck" OFF)
if (testlib-examples)
  add_subdirectory (examples)
endif ()

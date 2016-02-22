
################################################################################
# my library function use exmaples
################################################################################
option (testlib-examples "Example code of using gtest and CppQuickCheck" OFF)

if (testlib-examples)
  add_subdirectory (examples)
endif ()

################################################################################
# test for fixtures
################################################################################
option (fixture-test "Test for sample generators" ON)

if (fixture-test)
  enable_testing ()

  add_executable (KeyTest KeyTest.cpp)
  target_link_libraries (KeyTest gtest gtest_main cppqc setup)
  add_test (key-test KeyTest)

  add_executable (SchedulerTest SchedulerTest.cpp)
  target_link_libraries (SchedulerTest gtest gtest_main cppqc setup)
  add_test (scheduler-test SchedulerTest)
endif ()

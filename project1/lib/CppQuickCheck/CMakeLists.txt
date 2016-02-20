cmake_minimum_required(VERSION 2.6)
project(CppQuickCheck)
set(CMAKE_CXX_FLAGS "-O3 -g -Wall -std=c++11")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories("${PROJECT_SOURCE_DIR}/include")

# Examples
option (cppqc_examples "build Cpp Quick Check's examples")
if (cppqc_examples)
  add_subdirectory(examples)
endif ()

add_library(cppqc SHARED src/Arbitrary.cpp)

install(DIRECTORY "include/" DESTINATION "include"
    PATTERN ".*" EXCLUDE)
install(TARGETS cppqc DESTINATION "lib")

# "catch" based unit tests
option (cppqc_test "build Cpp Quick Check's own tests")
if (cppqc_test)
  enable_testing()
  add_executable(all-catch-tests test/catch-main.cpp test/shrink-explosion-protection.cpp)
  add_test(all-catch-tests all-catch-tests)
endif ()

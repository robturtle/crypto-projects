cmake_minimum_required (VERSION 3.4)
project (CryptoProject1)
set (CMAKE_CXX_FLAGS "-std=c++11")
# for CppQuickcheck
set (CMAKE_MACOSX_RPATH 1)

################################################################################
# Testing
################################################################################
option (test "Build all tests." ON)

if (test)
  enable_testing ()

  add_subdirectory (lib/googletest)
  include_directories (${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  add_subdirectory (lib/CppQuickCheck)
  include_directories (${CppQuickCheck_SOURCE_DIR}/include ${CppQuickCheck_SOURCE_DIR})

  add_subdirectory (test)
endif ()

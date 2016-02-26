cmake_minimum_required (VERSION 3.0)
project (CryptoProject1)
set (CMAKE_CXX_FLAGS "-std=c++11")
# for CppQuickcheck
set (CMAKE_MACOSX_RPATH 1)

################################################################################
# Resources
################################################################################
file (COPY resources DESTINATION ${CMAKE_BINARY_DIR})
set (RESOURCE_PATH "\"${CMAKE_BINARY_DIR}/resources/\"")
configure_file (
  ${PROJECT_SOURCE_DIR}/include/config.hpp.in
  ${PROJECT_BINARY_DIR}/include/config.hpp
  )
include_directories (${PROJECT_BINARY_DIR}/include)


################################################################################
# Main
################################################################################
set (BOOST_ROOT ${PROJECT_SOURCE_DIR}/lib)
include_directories (${PROJECT_SOURCE_DIR}/lib)

include_directories (${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR})
add_subdirectory (src)


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

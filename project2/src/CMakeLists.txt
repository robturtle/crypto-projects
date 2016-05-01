cmake_minimum_required (VERSION 3.0)
project (CryptoProject2)
set (CMAKE_CXX_FLAGS "-std=c++11")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")
# for CppQuickCheck
set (CMAKE_MACOSX_RPATH 1)

# HTTPS connection
add_subdirectory (lib/curlpp)
include_directories (lib/curlpp/include)

# arg parsing
include_directories (lib/tclap/include)

add_executable (client client.cpp)
target_link_libraries (client curlpp)

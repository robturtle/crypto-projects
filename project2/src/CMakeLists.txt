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

# base64
add_subdirectory (lib/base64)
include_directories (lib/base64)

add_executable (client client.cpp)
target_link_libraries (client curlpp)

find_package (OpenSSL)
set (OPENSSL_USE_STATIC_LIBS ON)

if (OPENSSL_FOUND)
add_executable (enc main-encrypt.cpp)
target_link_libraries (enc ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} base64)

add_executable (dec main-decrypt.cpp)
target_link_libraries (dec ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} base64)
endif (OPENSSL_FOUND)

cmake_minimum_required (VERSION 2.8)
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

find_package (OpenSSL)
set (OPENSSL_USE_STATIC_LIBS ON)

add_library (crypto crypto.cpp)
target_link_libraries (crypto ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} base64)
install(TARGETS crypto DESTINATION lib)

add_executable (client client.cpp)
target_link_libraries (client curlpp crypto)

add_executable (gen_key gen_key.cpp)
target_link_libraries (gen_key crypto)

add_executable (enc enc.cpp)
target_link_libraries (enc crypto)

add_executable (dec dec.cpp)
target_link_libraries (dec crypto)

install(
  TARGETS gen_key dec enc
  DESTINATION ${PROJECT_SOURCE_DIR}/../bin
  )

add_subdirectory (examples)


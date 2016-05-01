include_directories (${PROJECT_SOURCE_DIR})
add_executable (gcm-test gcm_enc_dec.cpp)
target_link_libraries (gcm-test crypto)

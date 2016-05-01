include_directories (${PROJECT_SOURCE_DIR})

add_executable (gcm-test gcm_enc_dec.cpp)
target_link_libraries (gcm-test crypto)

add_executable (sample-key sample_key.cpp)
target_link_libraries (sample-key crypto)

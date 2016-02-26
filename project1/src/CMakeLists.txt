add_library (setup setup.cpp)
add_library (code_breaker CodeBreaker.cpp)

add_executable (Liu-Xie-decrypt main.cpp)
target_link_libraries (Liu-Xie-decrypt code_breaker setup)

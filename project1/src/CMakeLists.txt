add_library (setup setup.cpp)
add_library (code_breaker CodeBreaker.cpp)
target_link_libraries (code_breaker setup)

add_executable (Liu-Xie-decrypt main.cpp)
target_link_libraries (Liu-Xie-decrypt code_breaker)

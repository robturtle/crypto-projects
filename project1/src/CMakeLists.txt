add_library (setup setup.cpp)
target_link_libraries (setup ${Boost_LIBRARIES})

add_executable (dict_parser parse_dictionaries.cpp)
target_link_libraries (dict_parser setup)
add_custom_target (dict_archive
  COMMAND ./dict_parser
  BYPRODUCTS dictionaries.txt
  DEPENDS dict_parser
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/src)

add_library (code_breaker CodeBreaker.cpp)
target_link_libraries (code_breaker setup)
add_dependencies (code_breaker dict_archive)

add_executable (Liu-Xie-decrypt main.cpp)
target_link_libraries (Liu-Xie-decrypt code_breaker)
add_dependencies (Liu-Xie-decrypt dict_archive)

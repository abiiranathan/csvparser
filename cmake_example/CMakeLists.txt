find_package(csvparser 0.1.0 REQUIRED)

add_executable(cmake_example main.c)
target_link_libraries(cmake_example csvparser::csvparser)

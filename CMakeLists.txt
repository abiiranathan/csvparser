cmake_minimum_required(VERSION 3.5)
project(csvparser VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

include(GNUInstallDirs)
include(CTest)
enable_testing()

add_library(csvparser csvparser.c)
target_link_libraries(csvparser PUBLIC solidc)

if(BUILD_TESTING)
    add_executable(csvparser_test tests/test_csvparser.c)
    target_link_libraries(csvparser_test PRIVATE csvparser)
    target_compile_options(csvparser_test PRIVATE -Wall -Wextra -Werror -Wpedantic)

    add_test(NAME csvparser_test COMMAND csvparser_test)
endif()

if(BUILD_EXAMPLES)
    add_executable(csvparser_example examples/main.c)
    target_link_libraries(csvparser_example PRIVATE csvparser)
    target_compile_options(csvparser_example PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()

set_target_properties(csvparser PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
        PUBLIC_HEADER csvparser.h)

install(TARGETS csvparser
        EXPORT csvparser
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
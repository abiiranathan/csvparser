cmake_minimum_required(VERSION 3.5)
project(csvparser VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
    add_subdirectory(cmake_example)

    add_executable(csvparser_example examples/main.c)
    target_link_libraries(csvparser_example PRIVATE csvparser)
    target_compile_options(csvparser_example PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()

set_target_properties(csvparser PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
        PUBLIC_HEADER csvparser.h)

install(TARGETS csvparser
        EXPORT csvparser_export
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT csvparser_export
        FILE csvparserTargets.cmake
        NAMESPACE csvparser::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/csvparser)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/csvparserConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/csvparser)

# Generate the configuration file for the library
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/csvparserConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/csvparserConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/csvparserConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/csvparser
)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/csvparserConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/csvparser)

# Generate the .pc file for pkg-config
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/csvparser.pc.in
${CMAKE_CURRENT_BINARY_DIR}/csvparser.pc
@ONLY)

# Install the .pc file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/csvparser.pc
DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
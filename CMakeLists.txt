cmake_minimum_required(VERSION 3.14)
project(cpplog VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cpplog INTERFACE)
target_include_directories(cpplog INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

option(CPPLOG_BUILD_EXAMPLES "Build examples" ON)
option(CPPLOG_BUILD_TESTS    "Build tests"    ON)

if(CPPLOG_BUILD_EXAMPLES)
    add_executable(example_basic examples/basic.cpp)
    target_link_libraries(example_basic cpplog)
endif()

if(CPPLOG_BUILD_TESTS)
    enable_testing()
    add_executable(tests tests/test_main.cpp)
    target_link_libraries(tests cpplog)
    add_test(NAME cpplog_tests COMMAND tests)
endif()

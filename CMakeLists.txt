cmake_minimum_required(VERSION 3.16)

project(ustring)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

option(WITH_TESTS "Build tests" YES)
if(WITH_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    include(CTest)
endif()

add_subdirectory(ustring/)
add_subdirectory(exe/)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()

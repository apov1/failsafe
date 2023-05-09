cmake_minimum_required(VERSION 3.10)

project(testBinary)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -pedantic -Wextra -O2)

add_subdirectory(../../Libraries/FSHook fshook)

add_executable(test main.cpp)
target_link_libraries(test PUBLIC FSHook)
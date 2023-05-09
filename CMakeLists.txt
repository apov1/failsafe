cmake_minimum_required(VERSION 3.10)

project(failsafe)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -pedantic -Wextra -g)

enable_testing()

add_subdirectory(Libraries/USBControl)
add_subdirectory(Libraries/FSHook)
add_subdirectory(Failsafe)


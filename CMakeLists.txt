cmake_minimum_required(VERSION 3.14)
project(ReGlob)

set(CMAKE_CXX_STANDARD 17)

add_library(ReGlob INTERFACE)
target_include_directories(ReGlob INTERFACE .)

add_executable(ReGlob_Test test.cpp)

cmake_minimum_required(VERSION 3.12)
project(dyncycles)

set(CMAKE_CXX_STANDARD 14)

add_definitions("-Werror=return-type")

add_executable(dyncycles main.cpp dyncycles.cpp dyncycles.h)

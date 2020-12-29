cmake_minimum_required(VERSION 3.17)
project(CharServerUPD)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(CharServerUPD main.cpp)
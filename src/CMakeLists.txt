cmake_minimum_required(VERSION 3.0)
project(socketServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(a log.h utils.h main.cpp log.cpp utils.cpp)


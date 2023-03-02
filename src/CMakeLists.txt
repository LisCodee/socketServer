cmake_minimum_required(VERSION 3.8)
project(socketServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(server log.h log.cpp utils.h utils.cpp  ConfigParse.cpp ConfigParse.h testConfigParse.cpp)

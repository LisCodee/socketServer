cmake_minimum_required(VERSION 3.8)
project(socketServer)

set(CMAKE_CXX_STANDARD 11)

#add_executable(server log.h log.cpp utils.h utils.cpp  ConfigParse.cpp ConfigParse.h testConfigParse.cpp TcpConnection.cpp TcpConnection.h Channel.cpp Channel.h net.cpp net.h EventLoop.cpp EventLoop.h Platform.cpp Platform.h AsyncLog.cpp AsyncLog.h main.cpp)
add_executable(server AsyncLog.h AsyncLog.cpp main.cpp TimeStamp.cpp TimeStamp.h InetAddress.cpp InetAddress.h Endian.h TimerId.h Timer.cpp Timer.h Callback.h)
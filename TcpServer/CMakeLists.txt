cmake_minimum_required(VERSION 3.2)

project(libtcpserver VERSION 0.1 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 14)

add_library(
    tcpserver
    include/TcpServer/EPoll.h src/EPoll.cpp
    include/TcpServer/TcpSocket.h src/TcpSocket.cpp
    include/TcpServer/TcpServer.h src/TcpServer.cpp
)

target_include_directories(tcpserver
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
    PRIVATE src
)

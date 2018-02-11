cmake_minimum_required(VERSION 3.1)

project(TcpServer)
set (CMAKE_CXX_STANDARD 14)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    EPoll.h EPoll.cpp
    TcpSocket.h TcpSocket.cpp
    TcpServer.h TcpServer.cpp
)

cmake_minimum_required(VERSION 3.1)

project(TcpServer)
set (CMAKE_CXX_STANDARD 14)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    EPoll.cpp
    TcpSocket.cpp
    TcpServer.cpp
)

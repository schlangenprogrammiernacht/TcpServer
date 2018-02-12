set (CMAKE_CXX_STANDARD 14)
find_package(msgpack 2.1.5 REQUIRED)

add_executable(
    MsgPackExample
    src/main.cpp
    src/Game.h src/Game.cpp
    src/Snake.h src/Snake.cpp
    src/TcpProtocol.h
)

target_link_libraries(
    MsgPackExample
    tcpserver
)

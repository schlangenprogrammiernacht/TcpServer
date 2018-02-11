find_package(msgpack 2.1.5 REQUIRED)

add_executable(
    MsgPackExample
    src/main.cpp
    src/Game.h src/Game.cpp
)

target_link_libraries(
    MsgPackExample
    tcpserver
)

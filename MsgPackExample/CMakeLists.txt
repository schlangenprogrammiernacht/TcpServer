set (CMAKE_CXX_STANDARD 14)

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
    -static-libgcc -static-libstdc++
)

set(CMAKE_EXE_LINKER_FLAGS " -static")

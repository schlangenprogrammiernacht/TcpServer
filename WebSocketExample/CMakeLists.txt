set (CMAKE_CXX_STANDARD 17)

find_package(websocketpp)

add_executable(
    WebSocketExample
    src/main.cpp
    src/WebSocketExample.h
    src/WebSocketExample.cpp
)

target_link_libraries(
    WebSocketExample
    tcpserver
    -static-libgcc -static-libstdc++
)

set(CMAKE_EXE_LINKER_FLAGS " -static")

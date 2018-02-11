add_executable(
    BroadcastExample
    src/main.cpp
)

target_link_libraries(
    BroadcastExample
    tcpserver
)

add_executable(
    MsgPackExample
    src/main.cpp
)

target_link_libraries(
    MsgPackExample
    tcpserver
)

#pragma once

#include <TcpServer/TcpServer.h>

class Game
{
    public:
        Game();
        int Main();

    private:
        TcpServer server;
        double x = 0;
        double y = 0;

        bool OnConnectionEstablished(TcpSocket& socket);
        bool OnConnectionClosed(TcpSocket& socket);
        bool OnDataAvailable(TcpSocket& socket);
};

#pragma once

#include <vector>
#include <memory>

#include <TcpServer/TcpServer.h>
#include "Snake.h"

class Game
{
    public:
        Game();
        int Main();

    private:
        TcpServer server;
        std::vector<std::unique_ptr<Snake>> _snakes;

        bool OnConnectionEstablished(TcpSocket& socket);
        bool OnConnectionClosed(TcpSocket& socket);
        bool OnDataAvailable(TcpSocket& socket);
};

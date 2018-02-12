#pragma once

#include <map>
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
        std::map<uint32_t,std::unique_ptr<Snake>> _snakes;
        uint32_t _nextSnakeId = 0;

        bool OnConnectionEstablished(TcpSocket& socket);
        bool OnConnectionClosed(TcpSocket& socket);
        bool OnDataAvailable(TcpSocket& socket);

        void SendFullSnake(TcpSocket& socket, Snake& snake);
};

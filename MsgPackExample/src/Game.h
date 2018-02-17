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
        bool OnTimerInterval();
        void SendMessage(TcpSocket& socket, msgpack::sbuffer& buf);
        void BroadcastMessage(msgpack::sbuffer& buf);

        template <typename T> void SendObject(TcpSocket& socket, const T& obj)
        {
            msgpack::sbuffer buf;
            msgpack::pack(buf, obj);
            SendMessage(socket, buf);
        }

        template <typename T> void BroadcastObject(const T& obj)
        {
            msgpack::sbuffer buf;
            msgpack::pack(buf, obj);
            BroadcastMessage(buf);
        }

        void SendWorldUpdate(TcpSocket& socket);
};

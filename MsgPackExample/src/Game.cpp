#include "Game.h"

#include <msgpack.hpp>
#include <iostream>
#include <string>
#include <vector>

Game::Game()
{
    server.AddConnectionEstablishedListener(
        [this](TcpSocket& socket)
        {
            return OnConnectionEstablished(socket);
        }
    );

    server.AddConnectionClosedListener(
        [this](TcpSocket& socket)
        {
            return OnConnectionClosed(socket);
        }
    );

    server.AddDataAvailableListener(
        [this](TcpSocket& socket)
        {
            return OnDataAvailable(socket);
        }
    );
}

bool Game::OnConnectionEstablished(TcpSocket &socket)
{
    std::vector<std::string> data {{ "Welcome", socket.GetPeerName() }};
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, data);
    socket.Write(sbuf.data(), sbuf.size());
    x = 0;
    y = 0;
    std::cerr << "connection established to " << socket.GetPeer() << std::endl;
    return true;
}

bool Game::OnConnectionClosed(TcpSocket &socket)
{
    std::cerr << "connection to " << socket.GetPeer() << " closed." << std::endl;
    return true;
}

bool Game::OnDataAvailable(TcpSocket &socket)
{
    uint8_t data[1024];
    ssize_t count = socket.Read(data, sizeof(data));
    if (count > 0)
    {
        server.Broadcast(data, static_cast<size_t>(count));
    }
    return true;
}

int Game::Main()
{
    if (!server.Listen(9001))
    {
        return -1;
    }

    while (true)
    {
        if (server.Poll(16) == 0)
        {
            x += 5;
            y += 2.25;

            std::vector<double> data {{x, y}};
            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, data);
            server.Broadcast(sbuf.data(), sbuf.size());
        }
    }
}

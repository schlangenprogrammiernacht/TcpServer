#include "Game.h"

#include <msgpack.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

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
    auto snake = std::make_unique<Snake>(Vector2d {500,400}, 0, 50);
    socket.SetUserData(snake.get());
    _snakes.push_back(std::move(snake));
    std::cerr << "connection established to " << socket.GetPeer() << std::endl;
    return true;
}

bool Game::OnConnectionClosed(TcpSocket &socket)
{
    std::cerr << "connection to " << socket.GetPeer() << " closed." << std::endl;

    auto snake = static_cast<Snake*>(socket.GetUserData());
    for (auto it=_snakes.begin(); it!=_snakes.end(); ++it)
    {
        if (it->get() == snake)
        {
            _snakes.erase(it);
        }
    }

    return true;
}

bool Game::OnDataAvailable(TcpSocket &socket)
{
    char data[1024];
    ssize_t count = socket.Read(data, sizeof(data));
    if (count > 0)
    {
        std::cerr << "read " << count << " bytes" << std::endl;
        msgpack::object_handle oh = msgpack::unpack(data, static_cast<size_t>(count));
        msgpack::object obj = oh.get();
        double heading;
        obj.convert(heading);

        std::cout << "Heading: " << heading << std::endl;

        auto snake = static_cast<Snake*>(socket.GetUserData());
        snake->SetHeading(heading);
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
        if (server.Poll(1000/60) == 0)
        {
            for (auto& snake: _snakes)
            {
                snake->MakeStep();
            }

            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, _snakes);
            server.Broadcast(sbuf.data(), sbuf.size());
            //std::cout << sbuf.size() << std::endl;
        }
    }
}

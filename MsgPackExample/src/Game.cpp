#include "Game.h"

#include <msgpack.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "TcpProtocol.h"

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
    auto id = _nextSnakeId++;
    auto snake = std::make_unique<Snake>(id, Vector2d {0,0}, 0, 150);
    socket.SetUserData(snake.get());
    _snakes[id] = std::move(snake);
    std::cerr << "connection established to " << socket.GetPeer() << std::endl;

    TcpProtocol::InitMessage initMsg { _snakes[id]->Id, _snakes[id]->Heading };
    msgpack::sbuffer initMsgBuf;
    msgpack::pack(initMsgBuf, initMsg);
    SendMessage(socket, initMsgBuf);

    BroadcastFullWorld();
    return true;
}

bool Game::OnConnectionClosed(TcpSocket &socket)
{
    std::cerr << "connection to " << socket.GetPeer() << " closed." << std::endl;

    auto snake = static_cast<Snake*>(socket.GetUserData());
    _snakes.erase(snake->Id);
    BroadcastFullWorld();

    return true;
}

bool Game::OnDataAvailable(TcpSocket &socket)
{
    char data[1024];
    ssize_t count = socket.Read(data, sizeof(data));
    if (count > 0)
    {
        msgpack::object_handle oh = msgpack::unpack(data, static_cast<size_t>(count));
        msgpack::object obj = oh.get();
        double heading;
        obj.convert(heading);

        auto snake = static_cast<Snake*>(socket.GetUserData());
        snake->SetHeading(heading);
    }
    return true;
}

int Game::Main()
{
    if (!server.Listen(9010))
    {
        return -1;
    }

    while (true)
    {
        server.Poll(1000/60);
        TcpProtocol::StepData stepMsg;
        for (auto& kvp: _snakes)
        {
            kvp.second->MakeStep();

            stepMsg.Data.push_back(
                TcpProtocol::StepData::SnakeStep {
                    kvp.first,
                    kvp.second->Heading,
                    kvp.second->Speed
                }
            );
        }

        if (stepMsg.Data.size()>0)
        {
            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, stepMsg);
            BroadcastMessage(sbuf);
        }
    }
}

void Game::SendMessage(TcpSocket &socket, msgpack::sbuffer& buf)
{
    if (buf.size() > 0)
    {
        auto mod = buf.size() % 4;
        if (mod>0)
        {
            buf.write("\x00\x00\x00\x00", mod);
        }

        uint32_t size = htonl(static_cast<uint32_t>(buf.size()));
        socket.Write(&size, sizeof(size), true);
        socket.Write(buf.data(), buf.size(), false);
    }
}

void Game::BroadcastMessage(msgpack::sbuffer& buf)
{
    if (buf.size() > 0)
    {
        auto mod = buf.size() % 4;
        if (mod>0)
        {
            buf.write("\x00\x00\x00\x00", mod);
        }

        uint32_t size = htonl(static_cast<uint32_t>(buf.size()));
        server.Broadcast(&size, sizeof(size), true);
        server.Broadcast(buf.data(), buf.size(), false);
    }
}

void Game::BroadcastFullWorld()
{
    TcpProtocol::FullWorldData fullWorldMsg;
    for (auto& kvp: _snakes)
    {
        fullWorldMsg.Snakes[kvp.first] = kvp.second.get();
    }
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, fullWorldMsg);
    BroadcastMessage(sbuf);
}

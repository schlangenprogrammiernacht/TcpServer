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

    server.AddTimerListener(
        [this](int, uint64_t expirationCount)
        {
            for (uint64_t i=0; i<expirationCount; i++)
            {
                OnTimerInterval();
            }
            return true;
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

    TcpProtocol::GameInfoMessage gameInfo;
    gameInfo.world_size_x = 100000.0;
    gameInfo.world_size_y = 100000.0;
    gameInfo.food_decay_per_frame = 0.1;
    SendObject(socket, gameInfo);

    TcpProtocol::PlayerInfoMessage playerInfo { id };
    SendObject(socket, playerInfo);

    TcpProtocol::BotSpawnMessage botSpawn;
    botSpawn.new_bot.id = id;
    botSpawn.new_bot.name = socket.GetPeerName();
    botSpawn.new_bot.heading = _snakes[id]->Heading;
    botSpawn.new_bot.segment_radius = 1;
    for (auto& seg: _snakes[id]->Segments)
    {
        botSpawn.new_bot.snake_segments.push_back({ seg.X, seg.Y });
    }
    BroadcastObject(botSpawn);
    SendWorldUpdate(socket);

    return true;
}

bool Game::OnConnectionClosed(TcpSocket &socket)
{
    std::cerr << "connection to " << socket.GetPeer() << " closed." << std::endl;

    TcpProtocol::BotKilledMessage msg;
    auto snake = static_cast<Snake*>(socket.GetUserData());
    std::cerr << "snake " << snake->Id << " killed." << std::endl;
    msg.killer_id = 0;
    msg.victim_id = snake->Id;
    _snakes.erase(snake->Id);

    BroadcastObject(msg);
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

bool Game::OnTimerInterval()
{
    TcpProtocol::BotMovedMessage msg;

    for (auto& kvp: _snakes)
    {
        auto &snake = kvp.second;
        snake->MakeStep();

        TcpProtocol::BotMoveItem item;
        item.bot_id = snake->Id;
        item.current_length = static_cast<uint32_t>(snake->Segments.size());
        item.current_segment_radius = 1.0;
        item.new_segments.push_back(TcpProtocol::SnakeSegment { snake->Segments[0].X, snake->Segments[0].Y });
        msg.items.push_back(item);
    }

    if (msg.items.size()>0)
    {
        BroadcastObject(msg);
    }

    return true;
}

int Game::Main()
{
    if (!server.Listen(9010))
    {
        return -1;
    }

    server.AddIntervalTimer(16666);

    while (true)
    {
        server.Poll(1000);
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

void Game::SendWorldUpdate(TcpSocket &socket)
{
    TcpProtocol::WorldUpdateMessage msg;
    for (auto& kvp: _snakes)
    {
        TcpProtocol::Bot bot;
        bot.id = kvp.first;
        bot.name = "";
        bot.heading = kvp.second->Heading;
        bot.segment_radius = 1;
        for (auto& seg: kvp.second->Segments)
        {
            bot.snake_segments.push_back({ seg.X, seg.Y });
        }
        msg.bots.push_back(bot);
    }
    SendObject(socket, msg);
}

#include <TcpServer/TcpServer.h>
#include <msgpack.hpp>
#include <iostream>
#include <string>
#include <vector>

static double x = 0;
static double y = 0;


int main()
{
    TcpServer server;

    server.AddConnectionEstablishedListener(
        [](TcpSocket& socket)
        {
            std::vector<std::string> data {{ "Welcome", socket.GetPeerName() }};
            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, data);
            socket.Write(sbuf.data(), sbuf.size());
            x = 0; y = 0;
            std::cerr << "connection established to " << socket.GetPeer() << std::endl;
            return true;
        }
    );

    server.AddConnectionClosedListener(
        [](TcpSocket& socket)
        {
            std::cerr << "connection to " << socket.GetPeer() << " closed." << std::endl;
            return true;
        }
    );

    server.AddDataAvailableListener(
        [&server](TcpSocket& socket)
        {
            uint8_t data[1024];
            ssize_t count = socket.Read(data, sizeof(data));
            if (count > 0)
            {
                server.Broadcast(data, static_cast<size_t>(count));
            }
            return true;
        }
    );

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

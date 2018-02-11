#include <TcpServer/TcpServer.h>
#include <msgpack.hpp>
#include <iostream>
#include <string>
#include <vector>

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

    server.AddDataReceivedListener(
        [&server](TcpSocket& socket, const void* data, size_t count)
        {
            server.Broadcast(data, count);
            return true;
        }
    );

    if (!server.Listen(9001))
    {
        return -1;
    }

    while (true)
    {
        if (server.Poll(10000) == 0)
        {
            // timeout
            //server.Broadcast("ping\n");
        }
    }

}

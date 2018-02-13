#include <TcpServer/TcpServer.h>

#include <iostream>

int main()
{
    TcpServer server;

    server.AddConnectionEstablishedListener(
        [](TcpSocket& socket)
        {
            socket.Write("Welcome, " + socket.GetPeerName() + "\n");
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
            if (count < 0)
            {
                socket.Close();
                return false;
            }

            socket.Write("Ok\n");
            server.Broadcast(data, static_cast<size_t>(count), false);
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
            server.Broadcast("ping\n");
        }
    }

}

#include "TcpServer.h"

int main()
{
    TcpServer server;

    auto listenerHandle = server.AddDataReceivedListener(
        [&server](TcpSocket& socket, const void* data, size_t count)
        {
            socket.Write("Ok\n");
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
            server.Broadcast("ping\n");
        }
    }

    server.RemoveListener(listenerHandle);
}

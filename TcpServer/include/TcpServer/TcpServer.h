#pragma once

#include <stddef.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <memory>
#include <functional>

#include "EPoll.h"
#include "TcpSocket.h"

class TcpServer
{
    public:
        static const constexpr int LISTEN_BACKLOG = 5;
        typedef std::function<bool(TcpSocket& socket)> ConnectionCallback;
        typedef size_t ListenerHandle;

    public:
        TcpServer();
        ~TcpServer();
        bool Listen(uint16_t port);
        int Poll(int timeout_ms);
        void Broadcast(const void *buf, size_t count);
        void Broadcast(std::string s);
        ListenerHandle AddConnectionEstablishedListener(ConnectionCallback listener);
        ListenerHandle AddConnectionClosedListener(ConnectionCallback listener);
        ListenerHandle AddDataAvailableListener(ConnectionCallback listener);
        void RemoveListener(ListenerHandle listenerHandle);
        EPoll& GetEPoll();

    private:
        EPoll _epoll;
        std::map<int, TcpSocket> _serverSockets;
        std::map<int, TcpSocket> _clientSockets;
        std::map<ListenerHandle, ConnectionCallback> _connectionEstablishedListeners;
        std::map<ListenerHandle, ConnectionCallback> _connectionClosedListeners;
        std::map<ListenerHandle, ConnectionCallback> _dataAvailableListeners;
        ListenerHandle _nextListenerHandle = 0;

        bool EpollEvent(const epoll_event& ev);
        bool ServerSocketEvent(TcpSocket& socket, uint32_t events);
        void RemoveServerSocket(TcpSocket& socket);
        bool ClientSocketEvent(TcpSocket& socket, uint32_t events);
        void RemoveClientSocket(TcpSocket& socket);
        void MakeConnectionCallback(std::map<ListenerHandle, ConnectionCallback>& callbackMap, TcpSocket& socket);

        ListenerHandle MakeListenerHandle();

};

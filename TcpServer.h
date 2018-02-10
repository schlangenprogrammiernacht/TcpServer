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
        static const constexpr size_t MAX_EPOLL_EVENTS = 20;
        typedef std::function<bool(std::shared_ptr<TcpSocket> socket, const void* data, size_t count)> DataReceivedCallback;
        typedef size_t ListenerHandle;

    public:
        TcpServer();
        ~TcpServer();
        bool Listen(uint16_t port);
        int Poll(int timeout_ms);
        void Broadcast(const void *buf, size_t count);
        void Broadcast(std::string s);
        ListenerHandle AddDataReceivedListener(DataReceivedCallback listener);
        void RemoveListener(ListenerHandle hnd);

    private:
        EPoll _epoll;
        std::map<int, std::shared_ptr<TcpSocket>> _serverSockets;
        std::map<int, std::shared_ptr<TcpSocket>> _clientSockets;
        std::map<ListenerHandle, DataReceivedCallback> _dataReceivedListeners;
        ListenerHandle _nextListenerHandle = 0;

        bool EpollEvent(const epoll_event& ev);
        bool ServerSocketEvent(std::shared_ptr<TcpSocket>& socket, uint32_t events);
        void RemoveServerSocket(std::shared_ptr<TcpSocket>& socket);
        bool ClientSocketEvent(std::shared_ptr<TcpSocket>& socket, uint32_t events);
        void RemoveClientSocket(std::shared_ptr<TcpSocket>& socket);
        void OnDataReceived(std::shared_ptr<TcpSocket>& socket, const void* data, size_t count);

};

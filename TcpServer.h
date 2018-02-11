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
        static const constexpr size_t DEFAULT_RECEIVE_BUFFER_SIZE = 8192;
        typedef std::function<bool(TcpSocket& socket)> ConnectionEstablishedCallback;
        typedef std::function<bool(TcpSocket& socket, const void* data, size_t count)> DataReceivedCallback;
        typedef size_t ListenerHandle;

    public:
        TcpServer();
        ~TcpServer();
        void SetReceiveBufferSize(size_t bufferSize);
        bool Listen(uint16_t port);
        int Poll(int timeout_ms);
        void Broadcast(const void *buf, size_t count);
        void Broadcast(std::string s);
        ListenerHandle AddConnectionEstablishedListener(ConnectionEstablishedCallback listener);
        ListenerHandle AddDataReceivedListener(DataReceivedCallback listener);
        void RemoveListener(ListenerHandle listenerHandle);
        EPoll& GetEPoll();

    private:
        EPoll _epoll;
        std::map<int, TcpSocket> _serverSockets;
        std::map<int, TcpSocket> _clientSockets;
        std::vector<uint8_t> _receiveBuffer;
        std::map<ListenerHandle, ConnectionEstablishedCallback> _connectionEstablishedListeners;
        std::map<ListenerHandle, DataReceivedCallback> _dataReceivedListeners;
        ListenerHandle _nextListenerHandle = 0;

        bool EpollEvent(const epoll_event& ev);
        bool ServerSocketEvent(TcpSocket& socket, uint32_t events);
        void RemoveServerSocket(TcpSocket& socket);
        bool ClientSocketEvent(TcpSocket& socket, uint32_t events);
        void RemoveClientSocket(TcpSocket& socket);
        void OnConnectionEstablished(TcpSocket& socket);
        void OnDataReceived(TcpSocket& socket, const void* data, size_t count);

        ListenerHandle MakeListenerHandle();

};

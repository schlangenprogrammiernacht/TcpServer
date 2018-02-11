#include <TcpServer/TcpServer.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <algorithm>
#include <iostream>

bool TcpServer::Listen(uint16_t port)
{
    TcpSocket sock;
    if (
        sock.Create()
        && sock.EnableReuseAddressOption()
        && sock.Bind(port)
        && sock.Listen(LISTEN_BACKLOG)
        && _epoll.AddFileDescriptor(sock.GetFileDescriptor(), EPOLLIN|EPOLLPRI|EPOLLERR)
    )
    {
        int fd = sock.GetFileDescriptor();
        _serverSockets[fd] = sock;
        return true;
    }
    return false;
}

int TcpServer::Poll(int timeout_ms)
{
    return _epoll.Poll(timeout_ms, [this](const epoll_event& ev) { return EpollEvent(ev); });
}

bool TcpServer::EpollEvent(const epoll_event &ev)
{
    auto server_it = _serverSockets.find(ev.data.fd);
    if (server_it != _serverSockets.end())
    {
        return ServerSocketEvent(server_it->second, ev.events);
    }

    auto client_it = _clientSockets.find(ev.data.fd);
    if (client_it != _clientSockets.end())
    {
        return ClientSocketEvent(client_it->second, ev.events);
    }

    return true;
}

bool TcpServer::ServerSocketEvent(TcpSocket &socket, uint32_t events)
{
    int sock = socket.Accept();
    if (sock < 0)
    {
        RemoveServerSocket(socket);
        return false;
    }

    TcpSocket clientSocket(sock);
    if (!_epoll.AddFileDescriptor(clientSocket.GetFileDescriptor(), EPOLLIN))
    {
        return false;
    }

    _clientSockets[sock] = clientSocket;
    MakeConnectionCallback(_connectionEstablishedListeners, _clientSockets[sock]);
    return true;
}

void TcpServer::RemoveServerSocket(TcpSocket &socket)
{
    auto fd = socket.GetFileDescriptor();
    auto it = _serverSockets.find(fd);
    if (it != _serverSockets.end())
    {
        socket.Close();
        _epoll.DeleteFileDescriptor(fd);
        std::cerr << "removing server socket " << fd << std::endl;
        _serverSockets.erase(it);
    }
}

bool TcpServer::ClientSocketEvent(TcpSocket &socket, uint32_t events)
{
    if (events & EPOLLIN)
    {
        MakeConnectionCallback(_dataAvailableListeners, socket);
    }
    return true;
}

void TcpServer::RemoveClientSocket(TcpSocket &socket)
{
    auto fd = socket.GetFileDescriptor();
    auto it = _clientSockets.find(fd);
    if (it != _clientSockets.end())
    {
        MakeConnectionCallback(_connectionClosedListeners, socket);
        socket.Close();
        _epoll.DeleteFileDescriptor(fd);
        _clientSockets.erase(it);
    }
}

void TcpServer::MakeConnectionCallback(std::map<TcpServer::ListenerHandle, TcpServer::ConnectionCallback> &callbackMap, TcpSocket &socket)
{
    std::vector<ListenerHandle> _toDelete;
    for (auto& kvp: callbackMap)
    {
        if (!(kvp.second)(socket))
        {
            _toDelete.push_back(kvp.first);
        }
    }

    for (auto& del: _toDelete)
    {
        callbackMap.erase(del);
    }
}

TcpServer::ListenerHandle TcpServer::MakeListenerHandle()
{
    return _nextListenerHandle++;
}

void TcpServer::Broadcast(const void *buf, size_t count)
{
    for (auto& kvp: _clientSockets)
    {
        if (kvp.second.Write(buf, count) != static_cast<ssize_t>(count))
        {
            RemoveClientSocket(kvp.second);
        }
    }
}

void TcpServer::Broadcast(std::string s)
{
    Broadcast(s.c_str(), s.length());
}

TcpServer::ListenerHandle TcpServer::AddConnectionEstablishedListener(TcpServer::ConnectionCallback listener)
{
    ListenerHandle result = MakeListenerHandle();
    _connectionEstablishedListeners[result] = listener;
    return result;
}

TcpServer::ListenerHandle TcpServer::AddConnectionClosedListener(TcpServer::ConnectionCallback listener)
{
    ListenerHandle result = MakeListenerHandle();
    _connectionClosedListeners[result] = listener;
    return result;
}

TcpServer::ListenerHandle TcpServer::AddDataAvailableListener(TcpServer::ConnectionCallback listener)
{
    ListenerHandle result = MakeListenerHandle();
    _dataAvailableListeners[result] = listener;
    return result;
}

void TcpServer::RemoveListener(TcpServer::ListenerHandle listenerHandle)
{
    _connectionEstablishedListeners.erase(listenerHandle);
    _connectionClosedListeners.equal_range(listenerHandle);
    _dataAvailableListeners.erase(listenerHandle);
}

EPoll &TcpServer::GetEPoll()
{
    return _epoll;
}

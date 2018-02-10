#include "TcpServer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <algorithm>
#include <iostream>

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
}

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
        _serverSockets[fd] = std::make_shared<TcpSocket>(std::move(sock));
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

bool TcpServer::ServerSocketEvent(std::shared_ptr<TcpSocket> &socket, uint32_t events)
{
    int sock = socket->Accept();
    if (sock < 0)
    {
        RemoveServerSocket(socket);
        return false;
    }

    auto clientSocket = std::make_shared<TcpSocket>(sock);
    if (!_epoll.AddFileDescriptor(clientSocket->GetFileDescriptor(), EPOLLIN))
    {
        return false;
    }

    _clientSockets[sock] = clientSocket;
    return true;
}

void TcpServer::RemoveServerSocket(std::shared_ptr<TcpSocket> &socket)
{
    auto it = _serverSockets.find(socket->GetFileDescriptor());
    if (it != _serverSockets.end())
    {
        _epoll.DeleteFileDescriptor(socket->GetFileDescriptor());
        std::cerr << "removing server socket " << socket->GetFileDescriptor() << std::endl;
        _serverSockets.erase(it);
    }
}

bool TcpServer::ClientSocketEvent(std::shared_ptr<TcpSocket> &socket, uint32_t events)
{
    uint8_t buf[8192];
    ssize_t bytes_read = socket->Read(buf, sizeof(buf));
    if (bytes_read <= 0)
    {
        RemoveClientSocket(socket);
        return false;
    }
    OnDataReceived(socket, buf, static_cast<size_t>(bytes_read));
    return true;
}

void TcpServer::RemoveClientSocket(std::shared_ptr<TcpSocket> &socket)
{
    auto it = _clientSockets.find(socket->GetFileDescriptor());
    if (it != _clientSockets.end())
    {
        _epoll.DeleteFileDescriptor(socket->GetFileDescriptor());
        std::cerr << "removing client socket " << socket->GetFileDescriptor() << std::endl;
        _clientSockets.erase(it);
    }
}

void TcpServer::OnDataReceived(std::shared_ptr<TcpSocket> &socket, const void *data, size_t count)
{
    std::vector<ListenerHandle> _toDelete;
    for (auto& kvp: _dataReceivedListeners)
    {
        if (!(kvp.second)(socket, data, count))
        {
            _toDelete.push_back(kvp.first);
        }
    }

    for (auto& del: _toDelete)
    {
        _dataReceivedListeners.erase(del);
    }
}

void TcpServer::Broadcast(const void *buf, size_t count)
{
    for (auto& kvp: _clientSockets)
    {
        if (kvp.second->Write(buf, count) != static_cast<ssize_t>(count))
        {
            RemoveClientSocket(kvp.second);
        }
    }
}

void TcpServer::Broadcast(std::string s)
{
    Broadcast(s.c_str(), s.length());
}

TcpServer::ListenerHandle TcpServer::AddDataReceivedListener(TcpServer::DataReceivedCallback listener)
{
    ListenerHandle result = _nextListenerHandle++;
    _dataReceivedListeners[result] = listener;
    return result;
}

void TcpServer::RemoveListener(TcpServer::ListenerHandle hnd)
{
    _dataReceivedListeners.erase(hnd);
}


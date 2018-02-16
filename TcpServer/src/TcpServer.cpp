#include <TcpServer/TcpServer.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
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

    for (auto timerFd: _timerHandles)
    {
        if (ev.data.fd == timerFd)
        {
            uint64_t expirationCount = 0;
            if (read(timerFd, &expirationCount, sizeof(expirationCount)) < 0)
            {
                perror("timerfd read");
                close(timerFd);
                return false;
            }
            TimerEvent(timerFd, expirationCount);
            return true;
        }
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
    if (!_epoll.AddFileDescriptor(clientSocket.GetFileDescriptor(), EPOLLIN|EPOLLRDHUP|EPOLLHUP))
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
    if (events & (EPOLLERR|EPOLLRDHUP|EPOLLHUP))
    {
        RemoveClientSocket(socket);
        return true;
    }
    else if (events & EPOLLIN)
    {
        MakeConnectionCallback(_dataAvailableListeners, socket);
    }
    return true;
}

void TcpServer::TimerEvent(int timerId, uint64_t expirationCount)
{
    for (auto& kvp: _timerListeners)
    {
        if (!(kvp.second)(timerId, expirationCount))
        {
            return;
        }
    }
}

void TcpServer::RemoveClientSocket(TcpSocket &socket)
{
    auto fd = socket.GetFileDescriptor();
    auto it = _clientSockets.find(fd);
    if (it != _clientSockets.end())
    {
        socket.Close();
        _epoll.DeleteFileDescriptor(fd);
        _clientSockets.erase(it);
        MakeConnectionCallback(_connectionClosedListeners, socket);
    }
}

void TcpServer::MakeConnectionCallback(std::map<TcpServer::ListenerHandle, TcpServer::ConnectionCallback> &callbackMap, TcpSocket &socket)
{
    for (auto& kvp: callbackMap)
    {
        if (!(kvp.second)(socket))
        {
            return;
        }
    }
}

TcpServer::ListenerHandle TcpServer::MakeListenerHandle()
{
    return _nextListenerHandle++;
}

void TcpServer::Broadcast(const void *buf, size_t count, bool more)
{
    for (auto& kvp: _clientSockets)
    {
        if (kvp.second.Write(buf, count, more) != static_cast<ssize_t>(count))
        {
            kvp.second.Close();
        }
    }
}

void TcpServer::Broadcast(std::string s)
{
    Broadcast(s.c_str(), s.length(), false);
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

TcpServer::ListenerHandle TcpServer::AddTimerListener(TcpServer::TimerCallback listener)
{
    ListenerHandle result = MakeListenerHandle();
    _timerListeners[result] = listener;
    return result;
}

void TcpServer::RemoveListener(TcpServer::ListenerHandle listenerHandle)
{
    _connectionEstablishedListeners.erase(listenerHandle);
    _connectionClosedListeners.equal_range(listenerHandle);
    _dataAvailableListeners.erase(listenerHandle);
    _timerListeners.erase(listenerHandle);
}

EPoll &TcpServer::GetEPoll()
{
    return _epoll;
}

int TcpServer::AddIntervalTimer(uint64_t interval_us)
{
    int timerFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timerFd < 0)
    {
        perror("timerfd_create()");
        return -1;
    }

    struct itimerspec ts;
    ts.it_interval.tv_sec = interval_us / 1000000;
    ts.it_interval.tv_nsec = 1000 * (interval_us % 1000000);
    ts.it_value.tv_sec = ts.it_interval.tv_sec;
    ts.it_value.tv_nsec = ts.it_interval.tv_nsec;


    if (timerfd_settime(timerFd, 0, &ts, nullptr) < 0)
    {
        perror("timerfd_settime()");
        close(timerFd);
        return -1;
    }

    if (!_epoll.AddFileDescriptor(timerFd, EPOLLIN))
    {
        return -1;
    }

    _timerHandles.push_back(timerFd);
    return timerFd;
}

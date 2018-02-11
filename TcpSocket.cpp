#include "TcpSocket.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

TcpSocket::TcpSocket()
{
}

TcpSocket::TcpSocket(int fd)
    : _fd(fd)
{
}

TcpSocket::~TcpSocket()
{
}

int TcpSocket::GetFileDescriptor() const
{
    return _fd;
}

bool TcpSocket::Create()
{
    _fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (_fd < 0)
    {
        std::cerr << "cannot create socket: " << errno << std::endl;
        return false;
    }
    return true;
}

bool TcpSocket::EnableReuseAddressOption()
{
    int enable = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        std::cerr << "setsockopt(SO_REUSEADDR) failed: " << errno << std::endl;
        return false;
    }
    return true;
}

bool TcpSocket::Bind(uint16_t port)
{
    struct sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    addr.sin6_addr = in6addr_any;
    if (bind(_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "cannot bind to port " << port << ": " << errno << std::endl;
        return false;
    }
    return true;
}

bool TcpSocket::Listen(int backlog)
{
    if (listen(_fd, backlog)<0)
    {
        std::cerr << "listen() failed: " << errno << std::endl;
        return false;
    }
    return true;
}

int TcpSocket::Accept()
{
    int sock = accept(_fd, nullptr, nullptr);
    if (sock<0)
    {
        std::cerr << "accept() failed: " << errno << std::endl;
    }
    return sock;
}

bool TcpSocket::Close()
{
    if (_fd>=0)
    {
        close(_fd);
        _fd = -1;
    }
    return true;
}

ssize_t TcpSocket::Read(void *buf, size_t count)
{
    return read(_fd, buf, count);
}

ssize_t TcpSocket::Write(const void *buf, size_t count)
{
    return write(_fd, buf, count);
}

ssize_t TcpSocket::Write(const std::string data)
{
    return Write(data.c_str(), data.length());
}

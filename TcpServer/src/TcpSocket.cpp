#include <TcpServer/TcpSocket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>

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

std::pair<std::string, int> TcpSocket::GetPeerNameAndPort()
{
    // taken from https://stackoverflow.com/questions/2064636/getting-the-source-address-of-an-incoming-socket-connection

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    char ipstr[INET6_ADDRSTRLEN];

    if (getpeername(_fd, (struct sockaddr*)&addr, &len) < 0)
    {
        perror("getpeername");
        return std::make_pair<std::string,int>("", 0);
    }

    if (addr.ss_family == AF_INET)
    {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        return std::make_pair<std::string,int>(ipstr, ntohs(s->sin_port));
    }
    else
    {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        return std::make_pair<std::string,int>(ipstr, ntohs(s->sin6_port));
    }
}

std::string TcpSocket::GetPeerName()
{
    return GetPeerNameAndPort().first;
}

int TcpSocket::GetPeerPort()
{
    return GetPeerNameAndPort().second;
}

std::string TcpSocket::GetPeer()
{
    auto p = GetPeerNameAndPort();
    std::ostringstream result;
    result << "[" << p.first << "]:" << p.second;
    return result.str();
}

void TcpSocket::SetUserData(void *userData)
{
    _userData = userData;
}

void *TcpSocket::GetUserData()
{
    return _userData;
}

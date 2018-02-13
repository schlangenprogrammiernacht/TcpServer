#pragma once
#include <unistd.h>
#include <stdint.h>
#include <string>

class TcpSocket
{
    public:
        TcpSocket();
        TcpSocket(int fd);
        TcpSocket(const TcpSocket &other);
        ~TcpSocket();

        int GetFileDescriptor() const;
        bool Create();
        bool EnableReuseAddressOption();
        bool Bind(uint16_t port);
        bool Listen(int backlog);
        int Accept();
        bool Close();
        ssize_t Read(void* buf, size_t count);
        ssize_t Write(const void* buf, size_t count, bool more);
        ssize_t Write(const std::string data);

        void GetPeerNameAndPort();
        std::string GetPeerName();
        int GetPeerPort();
        std::string GetPeer();
        void SetUserData(void* userData);
        void* GetUserData();

    private:
        int _fd = -1;
        void* _userData = nullptr;
        std::string _peerName;
        int _peerPort = -1;
        bool SetBoolOption(int optionName, bool enable);
};


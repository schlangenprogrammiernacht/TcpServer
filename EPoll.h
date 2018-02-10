#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <functional>

class EPoll
{
    public:
        EPoll();
        EPoll(size_t maxEvents);
        ~EPoll();

        int GetFileDescriptor();
        bool AddFileDescriptor(int socket, uint32_t events);
        bool DeleteFileDescriptor(int socket);

        int Poll(int timeout_ms, std::function<bool(const epoll_event& event)>&& callback);

    private:
        int _fd = -1;
        size_t _maxEvents = 20;

        bool AssertCreated();
        void Close();
};


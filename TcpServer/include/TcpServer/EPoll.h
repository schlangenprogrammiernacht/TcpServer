#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <functional>
#include <vector>

class EPoll
{
    public:
        static constexpr const size_t DEFAULT_MAX_EVENTS = 100;

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
        std::vector<epoll_event> _eventBuf;

        bool AssertCreated();
        void Close();
};


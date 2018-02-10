#include "EPoll.h"
#include <vector>
#include <iostream>

EPoll::EPoll()
{
}

EPoll::EPoll(size_t maxEvents)
    : _maxEvents(maxEvents)
{
}

EPoll::~EPoll()
{
    Close();
}

void EPoll::Close()
{
    if (_fd>=0)
    {
        close(_fd);
        _fd = -1;
    }
}

int EPoll::GetFileDescriptor()
{
    if (!AssertCreated())
    {
        return -1;
    }
    return _fd;
}

bool EPoll::AddFileDescriptor(int socket, uint32_t events)
{
    if (!AssertCreated())
    {
        return false;
    }

    epoll_event ev;
    ev.events = events;
    ev.data.fd = socket;
    if (epoll_ctl(_fd, EPOLL_CTL_ADD, socket, &ev) != 0)
    {
        std::cerr << "AddFileDescriptor: epoll_ctl failed: " << errno << std::endl;
        return false;
    }
    return true;
}

bool EPoll::DeleteFileDescriptor(int socket)
{
    if (!AssertCreated())
    {
        return false;
    }

    if (epoll_ctl(_fd, EPOLL_CTL_DEL, socket, nullptr) != 0)
    {
        return false;
    }
    return true;
}

int EPoll::Poll(int timeout_ms, std::function<bool (const epoll_event &)> &&callback)
{
    if (!AssertCreated())
    {
        return false;
    }

    std::vector<epoll_event> events;
    events.resize(_maxEvents);

    int eventsWaiting = epoll_wait(_fd, events.data(), static_cast<int>(_maxEvents), timeout_ms);
    if (eventsWaiting  < 0)
    {
        std::cerr << "epoll_wait() failed: " << errno << std::endl;
        return eventsWaiting;
    }
    if (eventsWaiting  == 0)
    {
        // timeout
        return 0;
    }

    for (int i=0; i<eventsWaiting ; i++)
    {
        auto& event = events[static_cast<size_t>(i)];
        if (!callback(event))
        {
            DeleteFileDescriptor(event.data.fd);
        }
    }

    return eventsWaiting;
}

bool EPoll::AssertCreated()
{
    if (_fd>=0)
    {
        return true;
    }

    _fd = epoll_create(1);
    if (_fd<0)
    {
        std::cerr << "failed to create epoll socket: " << errno << std::endl;
        return false;
    }

    return true;
}

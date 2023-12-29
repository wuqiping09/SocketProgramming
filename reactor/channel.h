#ifndef CHANNEL_H
#define CHANNEL_H

#include "epoll.h"
#include <sys/epoll.h>

class Epoll;

class Channel {
public:
    Channel(int fd, Epoll *ep);
    ~Channel();
    const int fd() const;
    const bool inepoll() const;
    const uint32_t event() const;
    const uint32_t revent() const;
    void setEdgeTrigger();
    void enableRead();
    void setinepoll();
    void setrevent(uint32_t ev);

private:
    int m_fd;
    Epoll *m_ep;
    bool m_inepoll;
    uint32_t m_event;
    uint32_t m_revent;
};

#endif
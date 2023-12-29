#ifndef EPOLL_H
#define EPOLL_H

#include "channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll {
public:
    Epoll();
    ~Epoll();
    std::vector<Channel*> loop(int timeout = -1);
    void updateChannel(Channel *channel);

private:
    static constexpr int MAX_EVENTS = 100;
    int m_epollfd;
    epoll_event m_events[MAX_EVENTS];
};

#endif
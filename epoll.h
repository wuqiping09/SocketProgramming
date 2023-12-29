#ifndef EPOLL_H
#define EPOLL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>

class Epoll {
public:
    Epoll();
    ~Epoll();
    void addfd(int fd, uint32_t op);
    std::vector<epoll_event> loop(int timeout = -1);

private:
    static constexpr int MAX_EVENTS = 100;
    int m_epollfd;
    epoll_event m_events[MAX_EVENTS];
};

#endif
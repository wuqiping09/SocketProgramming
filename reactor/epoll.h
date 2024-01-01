#ifndef EPOLL_H
#define EPOLL_H

#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unordered_map>
#include <memory>

class Channel;

class Connection;

class Epoll {
public:
    Epoll();
    ~Epoll();
    std::vector<Channel*> loop(int timeout = -1);
    void updateChannel(Channel *channel);
    void addConnection(int fd, std::shared_ptr<Connection> &connection);
    void delConnection(int fd);
    void delAllConnections();

private:
    static constexpr int MAX_EVENTS = 100;
    int m_epollfd;
    epoll_event m_events[MAX_EVENTS];
    std::unordered_map<int, std::shared_ptr<Connection>> m_connections;
};

#endif
#ifndef EPOLL_H
#define EPOLL_H

#include "channel.h"
#include "socket.h"
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

class Epoll {
public:
    Epoll();
    ~Epoll();
    std::vector<Channel*> loop(int timeout = -1);
    void updateChannel(Channel *channel);
    void addSocket(int fd, std::shared_ptr<Socket> &socket);
    void addChannel(int fd, std::shared_ptr<Channel> &channel);
    void eraseSocket(int fd);
    void eraseChannel(int fd);
    void eraseAll();

private:
    static constexpr int MAX_EVENTS = 100;
    int m_epollfd;
    epoll_event m_events[MAX_EVENTS];
    std::unordered_map<int, std::shared_ptr<Socket>> m_sockets;
    std::unordered_map<int, std::shared_ptr<Channel>> m_channels;
};

#endif
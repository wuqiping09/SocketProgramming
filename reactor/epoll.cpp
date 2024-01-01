#include "epoll.h"
#include <iostream>

Epoll::Epoll(): m_epollfd(-1) {
    m_epollfd = epoll_create(1);
    if (m_epollfd == -1) {
        perror("epoll_create");
        exit(1);
    }
}

Epoll::~Epoll() {
    close(m_epollfd);
}

std::vector<Channel*> Epoll::loop(int timeout) {
    std::vector<Channel*> channels;

    int infds = epoll_wait(m_epollfd, m_events, Epoll::MAX_EVENTS, timeout);
    if (infds < 0) {
        perror("epoll_wait");
        exit(1);
    }
    if (infds == 0) {
        std::cout << "epoll timeout" << std::endl;
        return channels;
    }

    for (int i = 0; i < infds; ++i) {
        Channel *p = static_cast<Channel*>(m_events[i].data.ptr);
        p->setrevent(m_events[i].events);
        channels.emplace_back(p);
    }
    return channels;
}

void Epoll::updateChannel(Channel *channel) {
    epoll_event ev;
    ev.data.ptr = static_cast<void*>(channel);
    ev.events = channel->event();

    if (channel->inepoll()) { // already in epoll
        // modify channel in epoll
        if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, channel->fd(), &ev) == -1) {
            perror("epoll_ctl");
            exit(1);
        }
    } else {
        // add channel to epoll
        if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, channel->fd(), &ev) == -1) {
            perror("epoll_ctl");
            exit(1);
        }
        channel->setinepoll();
    }
}

void Epoll::addConnection(int fd, std::shared_ptr<Connection> &connection) {
    m_connections[fd] = connection;
}

void Epoll::delConnection(int fd) {
    m_connections.erase(fd);
}

void Epoll::delAllConnections() {
    m_connections.clear();
}
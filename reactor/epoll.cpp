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

void Epoll::addSocket(int fd, std::shared_ptr<Socket> &socket) {
    m_sockets[fd] = socket;
}

void Epoll::addChannel(int fd, std::shared_ptr<Channel> &channel) {
    m_channels[fd] = channel;
}

void Epoll::eraseSocket(int fd) {
    m_sockets.erase(fd);
}

void Epoll::eraseChannel(int fd) {
    m_channels.erase(fd);
}

void Epoll::eraseAll() {
    m_sockets.clear();
    m_channels.clear();
}
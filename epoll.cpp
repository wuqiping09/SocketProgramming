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

void Epoll::addfd(int fd, uint32_t op) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = op;
    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl");
        exit(1);
    }
}

std::vector<epoll_event> Epoll::loop(int timeout) {
    std::vector<epoll_event> evs;

    int infds = epoll_wait(m_epollfd, m_events, Epoll::MAX_EVENTS, timeout);
    if (infds < 0) {
            perror("epoll_wait");
            exit(1);
        }
    if (infds == 0) {
        std::cout << "epoll timeout" << std::endl;
        return evs;
    }

    for (int i = 0; i < infds; ++i) {
        evs.emplace_back(m_events[i]);
    }
    return evs;
}
#include "channel.h"

Channel::Channel(int fd, Epoll *ep): m_fd(fd), m_ep(ep), m_inepoll(false), m_event(0), m_revent(0) {

}

Channel::~Channel() {

}

const int Channel::fd() const {
    return m_fd;
}

const bool Channel::inepoll() const {
    return m_inepoll;
}

const uint32_t Channel::event() const {
    return m_event;
}

const uint32_t Channel::revent() const {
    return m_revent;
}

void Channel::setEdgeTrigger() {
    m_event |= EPOLLET;
}

void Channel::enableRead() {
    m_event |= EPOLLIN;
    m_ep->updateChannel(this);
}

void Channel::setinepoll() {
    m_inepoll = true;
}

void Channel::setrevent(uint32_t ev) {
    m_revent = ev;
}
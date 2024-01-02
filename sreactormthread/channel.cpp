#include "channel.h"
#include "inetaddress.h"
#include "socket.h"
#include "processor.h"
#include <iostream>
#include <thread>
#include <future>

Channel::Channel(int fd, const std::shared_ptr<Epoll> &ep): m_fd(fd), m_ep(ep), m_inepoll(false), m_event(0), m_revent(0) {

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

void Channel::handleEvent() {
    if (m_revent & EPOLLRDHUP) { // client disconnect, some OS may not support this signal
        std::cout << "clientsock " << m_fd << " disconnect" << std::endl;
        close(m_fd); // close clientsock
        m_ep->delConnection(m_fd);
    } else if (m_revent & (EPOLLIN | EPOLLPRI)) { // read event
        readCallBack();
    } else if (m_revent & EPOLLOUT) { // write event
        // do nothing
    } else { // error
        std::cout << "clientsock " << m_fd << " error" << std::endl;
        close(m_fd);
    }
}

void Channel::newData() {
    // read
    InputData data;
    char buffer[1024];
    // use a loop to read all data when using edge trigger
    while (true) {
        memset(static_cast<void*>(buffer), 0, sizeof(buffer));
        ssize_t readbytes = read(m_fd, static_cast<void*>(buffer), sizeof(buffer));
        if (readbytes == 0) { // client disconnect
            std::cout << "clientsock " << m_fd << " disconnect" << std::endl;
            close(m_fd); // close clientsock
            m_ep->delConnection(m_fd);
            return;
        } else if (readbytes > 0) { // read success
            strcpy(data.s + data.len, buffer);
            data.len += strlen(buffer);
        } else if (readbytes == -1 && errno == EINTR) { // interrupt when reading
            break;
        } else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // all data has been read
            break;
        }
    }
    std::cout << "clientsock " << m_fd << " send: " << data.s << std::endl;

    // process
    Processor processor(data);
    ResultData result = std::async(&Processor::processData, &processor).get();

    // send
    send(m_fd, static_cast<void*>(result.s), result.len, 0);
}

void Channel::setReadCallBack(std::function<void()> f) {
    readCallBack = f;
}
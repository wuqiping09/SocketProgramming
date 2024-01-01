#include "channel.h"
#include "inetaddress.h"
#include "socket.h"
#include <iostream>

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
        // if a socket is closed, it will automatically be deleted from epollfd
        // remove socket pointer and channel pointer from epoll, thus destruct them
        m_ep->eraseSocket(m_fd);
        m_ep->eraseChannel(m_fd);
    } else if (m_revent & (EPOLLIN | EPOLLPRI)) { // read event
        readCallBack();
    } else if (m_revent & EPOLLOUT) { // write event
        // do nothing
    } else { // error
        std::cout << "clientsock " << m_fd << " error" << std::endl;
        close(m_fd);
    }
}

// void Channel::newConnect(Socket *serversock) {
//     InetAddress clientaddr;
//     std::shared_ptr<Socket> clientsock = std::make_shared<Socket>(serversock->accept(clientaddr));
//     std::cout << "clientsock = " << clientsock->fd() << std::endl;
//     std::cout << "client ip = " << clientaddr.ip() << " , port = " << clientaddr.port() << std::endl;
//     // add clientsock to epoll
//     std::shared_ptr<Channel> clientChannel = std::make_shared<Channel>(clientsock->fd(), m_ep);
//     clientChannel->enableRead();
//     clientChannel->setEdgeTrigger();
//     clientChannel->setReadCallBack(std::bind(&Channel::newData, clientChannel));
//     // store socket pointer and channel pointer to epoll
//     clientChannel->addSocket(clientsock->fd(), clientsock);
//     clientChannel->addChannel(clientChannel->fd(), clientChannel);
// }

void Channel::newData() {
    char buffer[1024];
    // use a loop to read all data when using edge trigger
    while (true) {
        memset(static_cast<void*>(buffer), 0, sizeof(buffer));
        ssize_t readbytes = read(m_fd, static_cast<void*>(buffer), sizeof(buffer));
        if (readbytes == 0) { // client disconnect
            std::cout << "clientsock " << m_fd << " disconnect" << std::endl;
            close(m_fd); // close clientsock
            // if a socket is closed, it will automatically be deleted from epollfd
            // remove socket pointer and channel pointer from epoll, thus destruct them
            m_ep->eraseSocket(m_fd);
            m_ep->eraseChannel(m_fd);
            break;
        } else if (readbytes > 0) { // read success
            std::cout << "clientsock " << m_fd << " send: " << buffer << std::endl;
            send(m_fd, static_cast<void*>(buffer), strlen(buffer), 0);
        } else if (readbytes == -1 && errno == EINTR) { // interrupt when reading
            break;
        } else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // all data has been read
            break;
        }
    }
}

void Channel::setReadCallBack(std::function<void()> f) {
    readCallBack = f;
}

void Channel::addSocket(int fd, std::shared_ptr<Socket> &socket) {
    m_ep->addSocket(fd, socket);
}

void Channel::addChannel(int fd, std::shared_ptr<Channel> &channel) {
    m_ep->addChannel(fd, channel);
}

void Channel::eraseSocket(int fd) {
    m_ep->eraseSocket(fd);
}

void Channel::eraseChannel(int fd) {
    m_ep->eraseChannel(fd);
}
#include "socket.h"

int createfd() {
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listenfd == -1) {
        perror("socket");
        exit(1);
    }
    return listenfd;
}

Socket::Socket(int _fd): m_fd(_fd) {

}

Socket::~Socket() {
    close(m_fd);
}

int Socket::fd() const {
    return m_fd;
}

void Socket::setreuseaddr(bool flag) {
    if (flag) {
        int opt = 1;
        setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
    }
}

void Socket::setreuseport(bool flag) {
    if (flag) {
        int opt = 1;
        setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
    }
}

void Socket::settcpnodelay(bool flag) {
    if (flag) {
        int opt = 1;
        setsockopt(m_fd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
    }
}

bool Socket::bind(const InetAddress &addr) {
    if (::bind(m_fd, addr.addr(), sizeof(sockaddr)) != 0) {
        perror("bind");
        close(m_fd);
        exit(1);
    }
    return true;
}

bool Socket::listen(int maxN) {
    if (::listen(m_fd, maxN) != 0) {
        perror("listen");
        close(m_fd);
        exit(1);
    }
    return true;
}

int Socket::accept(InetAddress &addr) {
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(sockaddr_in);
    int clientsock = accept4(m_fd, reinterpret_cast<sockaddr*>(&peeraddr), &len, SOCK_NONBLOCK);
    if (clientsock < 0) {
        perror("accept");
        return false;
    }
    addr = peeraddr;
    return clientsock;
}
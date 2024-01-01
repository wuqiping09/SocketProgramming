#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "socket.h"
#include "epoll.h"

class Acceptor {
public:
    Acceptor(const std::shared_ptr<Epoll> &ep);
    ~Acceptor();
    void accept(std::shared_ptr<Socket> &serversock);

private:
    std::shared_ptr<Epoll> m_ep;
};

#endif
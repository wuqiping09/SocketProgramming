#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "eventloop.h"
#include "socket.h"
#include "channel.h"

class TCPServer {
public:
    TCPServer(const std::string &ip, const uint16_t port);
    ~TCPServer();
    void start();

private:
    EventLoop m_el;
    std::shared_ptr<Acceptor> acceptor;
    std::shared_ptr<Socket> listensock;
    std::shared_ptr<Channel> channel;
};

#endif
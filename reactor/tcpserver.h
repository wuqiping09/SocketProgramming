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
};

#endif
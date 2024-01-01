#include "tcpserver.h"

TCPServer::TCPServer(const std::string &ip, const uint16_t port): m_el(), acceptor(new Acceptor(m_el.getep())) {
    listensock = std::make_shared<Socket>(createfd());
    InetAddress serveraddr(ip, port);
    listensock->setreuseaddr(true);
    listensock->setreuseport(true);
    listensock->settcpnodelay(true);
    listensock->bind(serveraddr);
    listensock->listen();

    channel = std::make_shared<Channel>(listensock->fd(), m_el.getep());
    channel->enableRead();
    channel->setReadCallBack(std::bind(&Acceptor::accept, acceptor.get(), listensock));
}

TCPServer::~TCPServer() {
    m_el.getep()->delAllConnections();
}

void TCPServer::start() {
    m_el.run();
}
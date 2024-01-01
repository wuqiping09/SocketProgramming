#include "acceptor.h"
#include "channel.h"
#include <memory>
#include <iostream>

Acceptor::Acceptor(const std::shared_ptr<Epoll> &ep): m_ep(ep) {

}

Acceptor::~Acceptor() {

}

void Acceptor::accept(std::shared_ptr<Socket> &serversock) {
    InetAddress clientaddr;
    std::shared_ptr<Socket> clientsock = std::make_shared<Socket>(serversock->accept(clientaddr));
    std::cout << "clientsock = " << clientsock->fd() << std::endl;
    std::cout << "client ip = " << clientaddr.ip() << " , port = " << clientaddr.port() << std::endl;
    // add clientsock to epoll
    std::shared_ptr<Channel> clientChannel = std::make_shared<Channel>(clientsock->fd(), m_ep);
    clientChannel->enableRead();
    clientChannel->setEdgeTrigger();
    clientChannel->setReadCallBack(std::bind(&Channel::newData, clientChannel));
    // store socket pointer and channel pointer to epoll
    clientChannel->addSocket(clientsock->fd(), clientsock);
    clientChannel->addChannel(clientChannel->fd(), clientChannel);
}
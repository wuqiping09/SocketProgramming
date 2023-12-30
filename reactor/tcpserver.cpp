 #include "tcpserver.h"
 
TCPServer::TCPServer(const std::string &ip, const uint16_t port) {
    Socket *socket = new Socket(createfd());
    InetAddress serveraddr(ip, static_cast<unsigned short>(port));
    socket->setreuseaddr(true);
    socket->setreuseport(true);
    socket->settcpnodelay(true);
    socket->bind(serveraddr);
    socket->listen();

    Channel *channel = new Channel(socket->fd(), m_el.getep());
    channel->enableRead();
    channel->setReadCallBack(std::bind(&Channel::newConnect, channel, socket));
}

TCPServer::~TCPServer() {
    
}

void TCPServer::start() {
    m_el.run();
}
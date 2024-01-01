 #include "tcpserver.h"
 
TCPServer::TCPServer(const std::string &ip, const uint16_t port): m_el(), acceptor(new Acceptor(m_el.getep())) {
    std::shared_ptr<Socket> socket = std::make_shared<Socket>(createfd());
    InetAddress serveraddr(ip, port);
    socket->setreuseaddr(true);
    socket->setreuseport(true);
    socket->settcpnodelay(true);
    socket->bind(serveraddr);
    socket->listen();

    std::shared_ptr<Channel> channel = std::make_shared<Channel>(socket->fd(), m_el.getep());
    channel->enableRead();
    channel->setReadCallBack(std::bind(&Acceptor::accept, acceptor.get(), socket));

    m_el.getep()->addSocket(socket->fd(), socket);
    m_el.getep()->addChannel(socket->fd(), channel);
}

TCPServer::~TCPServer() {
    m_el.getep()->eraseAll();
}

void TCPServer::start() {
    m_el.run();
}
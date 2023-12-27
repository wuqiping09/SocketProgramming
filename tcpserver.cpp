#include "tcpserver.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

TCPServer::TCPServer(): m_listenfd(-1), m_clientfd(-1) {

}

TCPServer::~TCPServer() {
    closeClientfd();
    closeListenfd();
}

bool TCPServer::init(const unsigned short port) {
    if (m_listenfd != -1) {
        return false;
    }
    m_port = port;

    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenfd == -1) {
        return false;
    }
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);
    if (bind(m_listenfd, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) != 0) {
        closeListenfd();
        return false;
    }
    if (::listen(m_listenfd, MAX_CONNECTION) != 0) {
        closeListenfd();
        return false;
    }
    return true;
}

bool TCPServer::accept() {
    struct sockaddr_in caddr;
    socklen_t addrlen = sizeof(caddr);
    m_clientfd = ::accept(m_listenfd, reinterpret_cast<sockaddr*>(&caddr), &addrlen);
    if (m_clientfd == -1) {
        closeListenfd();
        return false;
    }
    m_ip = inet_ntoa(caddr.sin_addr);
    return true;
}

bool TCPServer::send(const std::string &buffer) {
    if (m_clientfd == -1) {
        return false;
    }
    if (::send(m_clientfd, buffer.data(), buffer.size(), 0) <= 0) {
        return false;
    }
    return true;
}

bool TCPServer::recv(std::string &buffer, const std::size_t maxlen) {
    buffer.clear();
    buffer.resize(maxlen);
    int readlen = ::recv(m_clientfd, &buffer[0], buffer.size(), 0);
    if (readlen <= 0) {
        buffer.clear();
        return false;
    }
    buffer.resize(readlen);
    return true;
}

bool TCPServer::closeClientfd() {
    if (m_clientfd == -1) {
        return false;
    }
    close(m_clientfd);
    m_clientfd = -1;
    return true;
}

bool TCPServer::closeListenfd() {
    if (m_listenfd == -1) {
        return false;
    }
    close(m_listenfd);
    m_listenfd = -1;
    return true;
}

const std::string& TCPServer::clientIP() const {
    return m_ip;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./tcpserver serverPort" << std::endl;
        return -1;
    }

    TCPServer tcpserver;
    if (!tcpserver.init(atoi(argv[1]))) {
        perror("init");
        return -1;
    }

    if (!tcpserver.accept()) {
        perror("accept");
        return -1;
    }
    std::cout << "Client connected, ip = " << tcpserver.clientIP() << std::endl;

    while (true) {
        std::string buffer;
        if (!tcpserver.recv(buffer, 1024)) {
            perror("recv");
            break;
        }
        std::cout << "Message received: " << buffer << std::endl;
        buffer = "OK";
        if (!tcpserver.send(buffer)) {
            perror("send");
            break;
        }
        std::cout << "Message sent: " << buffer << std::endl;
    }

    return 0;
}
#include "tcpserver.h"
#include "fileinfo.h"
#include <iostream>
#include <fstream>
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

bool TCPServer::send(void *buffer, const std::size_t len) {
    if (m_clientfd == -1) {
        return false;
    }
    if (::send(m_clientfd, buffer, len, 0) <= 0) {
        return false;
    }
    return true;
}

bool TCPServer::recv(void *buffer, const std::size_t maxlen) {
    int readlen = ::recv(m_clientfd, buffer, maxlen, 0);
    if (readlen <= 0) {
        return false;
    }
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

bool TCPServer::recvFile(const char *filename, const std::size_t len) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        std::cout << "Fail to open " << filename << std::endl;
        return false;
    }
    std::size_t totalbytes = 0;
    std::size_t numbytes = 0;
    char buffer[10];
    while (true) {
        numbytes = len - totalbytes > 10 ? 10 : len - totalbytes;
        if (!recv(static_cast<void*>(buffer), numbytes)) {
            return false;
        }
        out.write(buffer, numbytes);
        totalbytes += numbytes;
        if (totalbytes == len) {
            break;
        }
    }
    out.close();
    return true;
}

const std::string& TCPServer::clientIP() const {
    return m_ip;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./tcpserver serverPort outputFile" << std::endl;
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
        FileInfo fileinfo;
        if (!tcpserver.recv(static_cast<void*>(&fileinfo), sizeof(fileinfo))) {
            perror("recv");
            break;
        }
        std::cout << "Fileinfo received:" << std::endl;
        std::cout << "filename = " << fileinfo.filename << std::endl;
        std::cout << "filesize = " << fileinfo.filesize << std::endl;

        std::string ok = "OK";
        if (!tcpserver.send(static_cast<void*>(&ok), ok.size())) {
            perror("send");
            break;
        }
        std::cout << "Message sent: " << ok << std::endl;

        if (!tcpserver.recvFile(argv[2], fileinfo.filesize)) {
            std::cout << "Fail to receive file" << std::endl;
            break;
        }
        std::cout << "File received" << std::endl;
    }

    return 0;
}
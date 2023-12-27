#include "tcpclient.h"
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

TCPClient::TCPClient(): m_clientfd(-1) {

}

TCPClient::~TCPClient() {
    close();
}

bool TCPClient::connect(const std::string &ip, const unsigned short port) {
    if (m_clientfd != -1) {
        return false;
    }
    m_ip = ip;
    m_port = port;

    m_clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_clientfd == -1) {
        return false;
    }

    struct hostent *h;
    if ((h = gethostbyname(ip.data())) == nullptr) {
        close();
        return false;
    }
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    memcpy(&serveraddr.sin_addr, h->h_addr, h->h_length);
    serveraddr.sin_port = htons(port);

    // :: indicates using the libaray function, not the member function
    if (::connect(m_clientfd, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) != 0) {
        close();
        return false;
    }

    return true;
}

bool TCPClient::send(void *buffer, const std::size_t len) {
    if (m_clientfd == -1) {
        return false;
    }
    if (::send(m_clientfd, buffer, len, 0) <= 0) {
        return false;
    }
    return true;
}

bool TCPClient::recv(void *buffer, const std::size_t maxlen) {
    int readlen = ::recv(m_clientfd, buffer, maxlen, 0);
    if (readlen <= 0) {
        return false;
    }
    return true;
}

bool TCPClient::sendFile(const char *filename, const std::size_t len) {
    std::ifstream in(filename, std::fstream::binary);
    if (!in.is_open()) {
        std::cout << "Fail to open " << filename << std::endl;
        return false;
    }
    std::size_t numbytes = 0;
    std::size_t totalbytes = 0;
    char buffer[10];
    while (true) {
        memset(static_cast<void*>(buffer), 0, sizeof(buffer));
        numbytes = len - totalbytes > 10 ? 10 : len - totalbytes;
        in.read(buffer, numbytes);
        if (!send(static_cast<void*>(buffer), numbytes)) {
            return false;
        }
        totalbytes += numbytes;
        if (totalbytes == len) {
            break;
        }
    }
    in.close();
    return true;
}

bool TCPClient::close() {
    if (m_clientfd == -1) {
        return false;
    }
    ::close(m_clientfd);
    m_clientfd = -1;
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cout << "Usage: ./tcpclient serverIP serverPort filename filesize" << std::endl;
        return -1;
    }

    TCPClient tcpClient;
    if (!tcpClient.connect(std::string(argv[1]), atoi(argv[2]))) {
        perror("connect");
        return -1;
    }

    FileInfo fileinfo;
    strcpy(fileinfo.filename, argv[3]);
    fileinfo.filesize = static_cast<std::size_t>(atoi(argv[4]));

    if (!tcpClient.send(static_cast<void*>(&fileinfo), sizeof(fileinfo))) {
        perror("send");
        return -1;
    }
    
    char buffer[1024];
    if (tcpClient.recv(static_cast<void*>(buffer), sizeof(buffer)) <= 0) {
        perror("recv");
        return -1;
    }

    if (tcpClient.sendFile(fileinfo.filename, fileinfo.filesize) <= 0) {
        perror("send");
        return -1;
    }

    return 0;
}
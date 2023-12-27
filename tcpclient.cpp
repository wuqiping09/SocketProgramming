#include "tcpclient.h"
#include <iostream>
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

bool TCPClient::send(const std::string &buffer) {
    if (m_clientfd == -1) {
        return false;
    }
    if (::send(m_clientfd, buffer.data(), buffer.size(), 0) <= 0) {
        return false;
    }
    return true;
}

bool TCPClient::recv(std::string &buffer, const std::size_t maxlen) {
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

bool TCPClient::close() {
    if (m_clientfd == -1) {
        return false;
    }
    ::close(m_clientfd);
    m_clientfd = -1;
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./tcpclient serverIP serverPort" << std::endl;
        return -1;
    }

    TCPClient tcpClient;
    if (!tcpClient.connect(std::string(argv[1]), atoi(argv[2]))) {
        perror("connect");
        return -1;
    }
    for (int i = 0; i < 3; ++i) {
        std::string buffer = "This is the " + std::to_string(i + 1) + "th message";
        if (!tcpClient.send(buffer)) {
            perror("send");
            break;
        }
        std::cout << "Message sent: " << buffer << std::endl;
        buffer.clear();
        if (tcpClient.recv(buffer, 1024) <= 0) {
            perror("recv");
            break;
        }
        std::cout << "Message received: " << buffer << std::endl;
        sleep(1);
    }
    return 0;
}
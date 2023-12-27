#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>

class TCPServer {
public:
    TCPServer();
    ~TCPServer();
    bool init(const unsigned short port);
    bool accept();
    bool send(void *buffer, const std::size_t len);
    bool recv(void *buffer, const std::size_t maxlen);
    bool closeClientfd();
    bool closeListenfd();
    bool recvFile(const char *filename, const std::size_t len);
    const std::string& clientIP() const;

private:
    static constexpr int MAX_CONNECTION = 5;
    int m_listenfd;
    int m_clientfd;
    std::string m_ip; // client ip address
    unsigned short m_port;
};

#endif
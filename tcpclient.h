#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>

class TCPClient {
public:
    TCPClient();
    ~TCPClient();
    bool connect(const std::string &ip, const unsigned short port);
    bool send(void *buffer, const std::size_t len);
    bool recv(void *buffer, const std::size_t maxlen);
    bool sendFile(const char *filename, const std::size_t len);
    bool close();

private:
    int m_clientfd;
    std::string m_ip; // server ip address
    unsigned short m_port;
};

#endif
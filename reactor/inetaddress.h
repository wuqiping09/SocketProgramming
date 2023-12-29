#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress {
public:
    InetAddress();
    InetAddress(const std::string &ip, const unsigned short port);
    InetAddress(const sockaddr_in &addr);
    ~InetAddress();
    const std::string ip() const;
    const unsigned short port() const;
    const sockaddr* addr() const;
    InetAddress& operator=(const sockaddr_in &addr);

private:
    sockaddr_in m_addr;
};

#endif
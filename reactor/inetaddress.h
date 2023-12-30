#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress {
public:
    InetAddress();
    InetAddress(const std::string &ip, const uint16_t port);
    InetAddress(const sockaddr_in &addr);
    ~InetAddress();
    const std::string ip() const;
    const uint16_t port() const;
    const sockaddr* addr() const;
    InetAddress& operator=(const sockaddr_in &addr);

private:
    sockaddr_in m_addr;
};

#endif
#include "inetaddress.h"

InetAddress::InetAddress() {

}

InetAddress::InetAddress(const std::string &ip, const uint16_t port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_addr.sin_port = htons(port);
}

InetAddress::InetAddress(const sockaddr_in &addr): m_addr(addr) {

}

InetAddress::~InetAddress() {

}

const std::string InetAddress::ip() const {
    return std::string(inet_ntoa(m_addr.sin_addr));
}

const uint16_t InetAddress::port() const {
    return ntohs(m_addr.sin_port);
}

const sockaddr* InetAddress::addr() const {
    return reinterpret_cast<const sockaddr*>(&m_addr);
}

InetAddress& InetAddress::operator=(const sockaddr_in &addr) {
    m_addr = addr;
    return *this;
}
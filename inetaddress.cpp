#include "inetaddress.h"

InetAddress::InetAddress(const std::string &ip, const unsigned short port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_addr.sin_port = htons(static_cast<uint16_t>(port));
}

InetAddress::InetAddress(const sockaddr_in &addr): m_addr(addr) {

}

InetAddress::~InetAddress() {

}

const std::string InetAddress::ip() const {
    return std::string(inet_ntoa(m_addr.sin_addr));
}

const unsigned short InetAddress::port() const {
    return static_cast<const unsigned short>(ntohs(m_addr.sin_port));
}

const sockaddr* InetAddress::addr() const {
    return reinterpret_cast<const sockaddr*>(&m_addr);
}
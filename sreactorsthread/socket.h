#ifndef SOCKET_H
#define SOCKET_H

#include "inetaddress.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int createfd();

class Socket {
public:
    Socket();
    Socket(int _fd);
    ~Socket();
    int fd() const;
    void setreuseaddr(bool flag);
    void setreuseport(bool flag);
    void settcpnodelay(bool flag);
    bool bind(const InetAddress &addr);
    bool listen(int maxN = 128);
    int accept(InetAddress &addr);

private:
    const int m_fd;
};

#endif
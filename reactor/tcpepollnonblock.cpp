#include "inetaddress.h"
#include "socket.h"
#include "epoll.h"
#include "channel.h"
#include "eventloop.h"
#include <iostream>
#include <memory>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./tcpepollnonblock1 ip port" << std::endl;
        return -1;
    }

    Socket socket(createfd());
    InetAddress serveraddr(std::string(argv[1]), static_cast<unsigned short>(atoi(argv[2])));
    socket.setreuseaddr(true);
    socket.setreuseport(true);
    socket.settcpnodelay(true);
    socket.bind(serveraddr);
    socket.listen();

    EventLoop eventloop;

    std::unique_ptr<Channel> channel = std::make_unique<Channel>(socket.fd(), eventloop.getep());
    channel->enableRead();
    channel->setReadCallBack(std::bind(&Channel::newConnect, channel.get(), &socket));

    eventloop.run();

    return 0;
}
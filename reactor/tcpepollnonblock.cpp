#include "inetaddress.h"
#include "socket.h"
#include "epoll.h"
#include "channel.h"
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

    Epoll epoll;
    std::unique_ptr<Channel> channel = std::make_unique<Channel>(socket.fd(), &epoll);
    channel->enableRead();
    channel->setReadCallBack(std::bind(&Channel::newConnect, channel.get(), &socket));

    while (true) {
        std::vector<Channel*> channels = epoll.loop();

        // traverse returned events
        for (auto &ch : channels) {
            ch->handleEvent();
        }
    }

    return 0;
}
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>

int initserver(unsigned short port) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);
    if (bind(listenfd, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) != 0) {
        perror("bind");
        close(listenfd);
        return -1;
    }

    constexpr int MAX_CONNECTION = 5;
    if (listen(listenfd, MAX_CONNECTION) != 0) {
        perror("listen");
        close(listenfd);
        return -1;
    }

    return listenfd;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./tcpselect port" << std::endl;
    }

    int listensock = initserver(static_cast<unsigned short>(atoi(argv[1])));
    std::cout << "listensock = " << listensock << std::endl;
    if (listensock == -1) {
        std::cout << "init fail" << std::endl;
        return -1;
    }

    // there are three kinds of read event:
    // 1. client connect
    // 2. server has data to recv
    // 3. client disconnect
    // there is one kind of write event:
    // 1. server's send buffer still has space to write

    int epollfd = epoll_create(1); // any parameter greater than 0 is ok
    epoll_event ev;
    ev.data.fd = listensock;
    ev.events = EPOLLIN; // EPOLLIN is read event, EPOLLOUT is write event
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listensock, &ev);
    epoll_event evs[10]; // store returned events

    while (true) {
        int infds = epoll_wait(epollfd, evs, 10, -1); // -1 means no timeout
        if (infds < 0) {
            perror("epoll");
            break;
        }
        if (infds == 0) {
            std::cout << "epoll timeout" << std::endl;
            continue;
        }

        // traverse returned events
        for (int i = 0; i < infds; ++i) {
            if (evs[i].data.fd == listensock) { // client try to connect
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientsock = accept(listensock, reinterpret_cast<sockaddr*>(&clientaddr), &len);
                if (clientsock < 0) {
                    perror("accept");
                    continue;
                }
                std::cout << "clientsock = " << clientsock << std::endl;
                ev.data.fd = clientsock;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock, &ev);
            } else { // client might send data or might disconnect
                char buffer[1024];
                memset(static_cast<void*>(buffer), 0, sizeof(buffer));
                if (recv(evs[i].data.fd, static_cast<void*>(buffer), sizeof(buffer), 0) <= 0) {
                    std::cout << "clientsock " << evs[i].data.fd << " disconnect" << std::endl;
                    close(evs[i].data.fd); // close clientsock
                    // if a socket is closed, it will automatically be deleted from epollfd
                } else {
                    std::cout << "clientsock " << evs[i].data.fd << " send: " << buffer << std::endl;
                    send(evs[i].data.fd, static_cast<void*>(buffer), strlen(buffer), 0);
                }
            }
        }
    }

    close(listensock);
    return 0;
}
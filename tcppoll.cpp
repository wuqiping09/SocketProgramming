#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <poll.h>

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

    pollfd fds[2048];
    for (int i = 0; i < 2048; ++i) {
        fds[i].fd = -1;
    }
    fds[listensock].fd = listensock;
    fds[listensock].events = POLLIN; // POLLIN is read event, POLLOUT is write event
    int maxfd = listensock;

    while (true) {
        int infds = poll(fds, maxfd + 1, 5000); // timeout is 5 second
        if (infds < 0) {
            perror("poll");
            break;
        }
        if (infds == 0) {
            std::cout << "poll timeout" << std::endl;
            continue;
        }

        // traverse the array
        for (int eid = 0; eid <= maxfd; ++eid) {
            if (fds[eid].fd == -1) {
                continue;
            }
            if ((fds[eid].revents & POLLIN) == 0) { // no read event
                continue;
            }
            if (eid == listensock) { // client try to connect
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientsock = accept(listensock, reinterpret_cast<sockaddr*>(&clientaddr), &len);
                if (clientsock < 0) {
                    perror("accept");
                    continue;
                }
                std::cout << "clientsock = " << clientsock << std::endl;
                fds[clientsock].fd = clientsock;
                fds[clientsock].events = POLLIN;
                maxfd = std::max(maxfd, clientsock);
            } else { // client might send data or might disconnect
                char buffer[1024];
                memset(static_cast<void*>(buffer), 0, sizeof(buffer));
                if (recv(eid, static_cast<void*>(buffer), sizeof(buffer), 0) <= 0) {
                    std::cout << "clientsock " << eid << " disconnect" << std::endl;
                    close(eid); // close clientsock
                    fds[eid].fd = -1;
                    if (eid == maxfd) {
                        for (int i = maxfd - 1; i > 0; --i) {
                            if (fds[i].fd != -1) {
                                maxfd = i;
                                break;
                            }
                        }
                    }
                } else {
                    std::cout << "clientsock " << eid << " send: " << buffer << std::endl;
                    send(eid, static_cast<void*>(buffer), strlen(buffer), 0);
                }
            }
        }
    }

    return 0;
}
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

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

    fd_set readfds; // 1024 bits bitmap
    FD_ZERO(&readfds); // set every bit in bitmap to 0
    FD_SET(listensock, &readfds); // set listensock bit to 1
    int maxfd = listensock; // the max valid bit in bitmap

    while (true) {
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        fd_set temp = readfds; // copy the bitmap, since select will write the bitmap
        //int infds = select(maxfd + 1, &temp, nullptr, nullptr, &timeout);
        int infds = select(maxfd + 1, &temp, nullptr, nullptr, nullptr); // no timeout
        if (infds < 0) {
            perror("select");
            break;
        }
        if (infds == 0) {
            std::cout << "select timeout" << std::endl;
            continue;
        }

        // traverse the bitmap
        for (int eid = 0; eid <= maxfd; ++eid) {
            if (FD_ISSET(eid, &temp) == 0) { // 0 means no event
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
                FD_SET(clientsock, &readfds); // set clientsock bit to 1
                maxfd = std::max(maxfd, clientsock);
            } else { // client might send data or might disconnect
                char buffer[1024];
                memset(static_cast<void*>(buffer), 0, sizeof(buffer));
                if (recv(eid, static_cast<void*>(buffer), sizeof(buffer), 0) <= 0) {
                    std::cout << "clientsock " << eid << " disconnect" << std::endl;
                    close(eid); // close clientsock
                    FD_CLR(eid, &readfds); // set clientsock bit to 0
                    if (eid == maxfd) {
                        for (int i = maxfd - 1; i > 0; --i) {
                            if (FD_ISSET(i, &readfds)) {
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

    close(listensock);
    return 0;
}
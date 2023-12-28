#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>

// void setNonBlock(int fd) {
//     fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
// }

int initserver(unsigned short port) {
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listenfd == -1) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));

    struct sockaddr_in serveraddr;
    memset(static_cast<void*>(&serveraddr), 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);
    if (bind(listenfd, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) != 0) {
        perror("bind");
        close(listenfd);
        return -1;
    }

    constexpr int MAX_CONNECTION = 128;
    if (listen(listenfd, MAX_CONNECTION) != 0) {
        perror("listen");
        close(listenfd);
        return -1;
    }

    return listenfd;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./tcpepollnonblock port" << std::endl;
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
    ev.events = EPOLLIN; // EPOLLIN is read event, EPOLLOUT is write event, default is level trigger
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
            if (evs[i].events & EPOLLRDHUP) { // client disconnect, some OS may not support this signal
                std::cout << "clientsock " << evs[i].data.fd << " disconnect" << std::endl;
                close(evs[i].data.fd); // close clientsock
                // if a socket is closed, it will automatically be deleted from epollfd
            } else if (evs[i].events & (EPOLLIN | EPOLLPRI)) { // read event
                if (evs[i].data.fd == listensock) { // client try to connect
                    struct sockaddr_in clientaddr;
                    socklen_t len = sizeof(clientaddr);
                    int clientsock = accept4(listensock, reinterpret_cast<sockaddr*>(&clientaddr), &len, SOCK_NONBLOCK);
                    if (clientsock < 0) {
                        perror("accept");
                        continue;
                    }
                    std::cout << "clientsock = " << clientsock << std::endl;
                    ev.data.fd = clientsock;
                    ev.events = EPOLLIN | EPOLLET; // edge trigger
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock, &ev);
                } else { // client might send data or might disconnect
                    char buffer[1024];
                    // use a loop to read all data when using edge trigger
                    while (true) {
                        memset(static_cast<void*>(buffer), 0, sizeof(buffer));
                        ssize_t readbytes = read(evs[i].data.fd, static_cast<void*>(buffer), sizeof(buffer));
                        if (readbytes == 0) { // client disconnect
                            std::cout << "clientsock " << evs[i].data.fd << " disconnect" << std::endl;
                            close(evs[i].data.fd); // close clientsock
                            // if a socket is closed, it will automatically be deleted from epollfd
                            break;
                        } else if (readbytes > 0) { // read success
                            std::cout << "clientsock " << evs[i].data.fd << " send: " << buffer << std::endl;
                            send(evs[i].data.fd, static_cast<void*>(buffer), strlen(buffer), 0);
                        } else if (readbytes == -1 && errno == EINTR) { // interrupt when reading
                            continue;
                        } else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // all data has been read
                            break;
                        }
                    }
                }
            } else if (evs[i].events & EPOLLOUT) { // write event
                continue;
            } else { // error
                std::cout << "clientsock " << evs[i].data.fd << " error" << std::endl;
                close(evs[i].data.fd);
            }
        }
    }

    close(listensock);
    return 0;
}
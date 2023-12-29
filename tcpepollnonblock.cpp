#include "inetaddress.h"
#include "socket.h"
#include "epoll.h"
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
    epoll.addfd(socket.fd(), EPOLLIN); // read event and level trigger
    std::vector<epoll_event> evs;

    while (true) {
        evs = epoll.loop();

        // traverse returned events
        for (auto &ev : evs) {
            if (ev.events & EPOLLRDHUP) { // client disconnect, some OS may not support this signal
                std::cout << "clientsock " << ev.data.fd << " disconnect" << std::endl;
                close(ev.data.fd); // close clientsock
                // if a socket is closed, it will automatically be deleted from epollfd
            } else if (ev.events & (EPOLLIN | EPOLLPRI)) { // read event
                if (ev.data.fd == socket.fd()) { // client try to connect
                    InetAddress clientaddr;
                    Socket *clientsock = new Socket(socket.accept(clientaddr)); // it is a problem that no delete for this instance
                    std::cout << "clientsock = " << clientsock->fd() << std::endl;
                    std::cout << "client ip = " << clientaddr.ip() << " , port = " << clientaddr.port() << std::endl;
                    // add clientsock to epoll
                    epoll.addfd(clientsock->fd(), EPOLLIN | EPOLLET); // read event and edge trigger
                } else { // client might send data or might disconnect
                    char buffer[1024];
                    // use a loop to read all data when using edge trigger
                    while (true) {
                        memset(static_cast<void*>(buffer), 0, sizeof(buffer));
                        ssize_t readbytes = read(ev.data.fd, static_cast<void*>(buffer), sizeof(buffer));
                        if (readbytes == 0) { // client disconnect
                            std::cout << "clientsock " << ev.data.fd << " disconnect" << std::endl;
                            close(ev.data.fd); // close clientsock
                            // if a socket is closed, it will automatically be deleted from epollfd
                            break;
                        } else if (readbytes > 0) { // read success
                            std::cout << "clientsock " << ev.data.fd << " send: " << buffer << std::endl;
                            send(ev.data.fd, static_cast<void*>(buffer), strlen(buffer), 0);
                        } else if (readbytes == -1 && errno == EINTR) { // interrupt when reading
                            continue;
                        } else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // all data has been read
                            break;
                        }
                    }
                }
            } else if (ev.events & EPOLLOUT) { // write event
                continue;
            } else { // error
                std::cout << "clientsock " << ev.data.fd << " error" << std::endl;
                close(ev.data.fd);
            }
        }
    }

    return 0;
}
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./server1 serverPort" << std::endl;
        return -1;
    }

    // a socket is a file descriptor
    // at most 1024 opened file descriptor
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket");
        return -1;
    }

    // sockaddr has the same size as sockaddr_in, so we can reinterpret_cast them
    // we use sockaddr_in to set sin_family, sin_addr, sin_port, but most functions use sockaddr as parameter
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));
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

    int clientfd = accept(listenfd, 0, 0);
    if (clientfd == -1) {
        perror("accept");
        close(listenfd);
        return -1;
    }
    std::cout << "Client connected" << std::endl;

    char buffer[1024];
    while (true) {
        int iret = 0;
        memset(buffer, 0, sizeof(buffer));
        if ((iret = recv(clientfd, buffer, sizeof(buffer), 0)) <= 0) {
            std::cout << "iret = " << iret << std::endl;
            break;
        }
        std::cout << "Message received: " << buffer << std::endl;
        std::string temp = "OK";
        strcpy(buffer, temp.c_str());
        if ((iret = send(clientfd, buffer, strlen(buffer), 0)) <= 0) {
            perror("send");
            break;
        }
        std::cout << "Message sent: " << buffer << std::endl;
    }

    close(clientfd);
    close(listenfd);
    return 0;
}
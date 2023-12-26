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
    if (argc != 3) {
        std::cout << "Usage: ./client1 serverIP serverPort" << std::endl;
        return -1;
    }

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        perror("socket");
        return -1;
    }

    struct hostent *h;
    if ((h = gethostbyname(argv[1])) == 0) {
        std::cout << "gethostbyname failed" << std::endl;
        close(socketfd);
        return -1;
    }
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    memcpy(&serveraddr.sin_addr, h->h_addr, h->h_length);
    serveraddr.sin_port = htons(atoi(argv[2]));

    if (connect(socketfd, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) != 0) {
        perror("connect");
        close(socketfd);
        return -1;
    }

    char buffer[1024];
    for (int i = 0; i < 3; ++i) {
        int iret = 0;
        memset(buffer, 0, sizeof(buffer));
        std::string temp = "This is the " + std::to_string(i + 1) + "th message";
        strcpy(buffer, temp.c_str());
        if ((iret = send(socketfd, buffer, strlen(buffer), 0)) <= 0) {
            perror("send");
            break;
        }
        std::cout << "Message sent: " << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer));
        if ((iret = recv(socketfd, buffer, sizeof(buffer), 0)) <= 0) {
            std::cout << "iret = " << iret << std::endl;
            break;
        }
        std::cout << "Message received: " << buffer << std::endl;
        sleep(1);
    }

    close(socketfd);
    return 0;
}
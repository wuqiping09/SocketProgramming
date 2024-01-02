#include "tcpserver.h"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./main ip port" << std::endl;
        return -1;
    }

    TCPServer tcpserver(std::string(argv[1]), atoi(argv[2]));
    tcpserver.start();

    return 0;
}
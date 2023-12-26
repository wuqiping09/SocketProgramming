#include <string>

class TCPClient {
public:
    TCPClient();
    ~TCPClient();
    bool connect(const std::string &ip, const unsigned short port);
    bool send(const std::string &buffer);
    bool recv(std::string &buffer, const std::size_t maxlen);
    bool close();

private:
    int m_clientfd;
    std::string m_ip; // server ip address
    unsigned short m_port;
};
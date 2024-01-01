#ifndef CONNECTION_H
#define CONNECTION_H

#include "socket.h"
#include "channel.h"
#include <memory>

class Channel;

class Connection {
public:
    Connection(const std::shared_ptr<Socket> &socket, const std::shared_ptr<Channel> &channel);
    ~Connection();

private:
    std::shared_ptr<Socket> m_socket;
    std::shared_ptr<Channel> m_channel;
};

#endif
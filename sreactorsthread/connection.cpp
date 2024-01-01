#include "connection.h"

Connection::Connection(const std::shared_ptr<Socket> &socket, const std::shared_ptr<Channel> &channel): m_socket(socket), m_channel(channel) {

}

Connection::~Connection() {

}
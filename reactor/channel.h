#ifndef CHANNEL_H
#define CHANNEL_H

#include "epoll.h"
#include "socket.h"
#include <sys/epoll.h>
#include <functional>
#include <memory>

class Epoll;

class Channel {
public:
    Channel(int fd, const std::shared_ptr<Epoll> &ep);
    ~Channel();
    const int fd() const;
    const bool inepoll() const;
    const uint32_t event() const;
    const uint32_t revent() const;
    void setEdgeTrigger();
    void enableRead();
    void setinepoll();
    void setrevent(uint32_t ev);
    void handleEvent();
    void newConnect(Socket *serversock);
    void newData();
    void setReadCallBack(std::function<void()> f);
    void addSocket(int fd, std::shared_ptr<Socket> &socket);
    void addChannel(int fd, std::shared_ptr<Channel> &channel);
    void eraseSocket(int fd);
    void eraseChannel(int fd);

private:
    int m_fd;
    std::shared_ptr<Epoll> m_ep;
    bool m_inepoll;
    uint32_t m_event;
    uint32_t m_revent;
    std::function<void()> readCallBack;
};

#endif
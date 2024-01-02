#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "epoll.h"
#include "acceptor.h"
#include <memory>

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void run();
    const std::shared_ptr<Epoll>& getep() const;

private:
    std::shared_ptr<Epoll> ep;
};

#endif
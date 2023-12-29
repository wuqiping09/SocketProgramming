#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "epoll.h"
#include <memory>

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void run();
    Epoll* getep() const;

private:
    std::unique_ptr<Epoll> ep;
};

#endif
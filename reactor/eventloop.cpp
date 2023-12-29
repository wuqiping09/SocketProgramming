#include "eventloop.h"

EventLoop::EventLoop(): ep(new Epoll) {

}

EventLoop::~EventLoop() {

}

void EventLoop::run() {
    while (true) {
        std::vector<Channel*> channels = ep->loop();

        // traverse returned events
        for (auto &ch : channels) {
            ch->handleEvent();
        }
    }
}

Epoll* EventLoop::getep() const {
    return ep.get();
}
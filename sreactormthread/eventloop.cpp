#include "eventloop.h"

EventLoop::EventLoop(): ep(new Epoll()) {

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

const std::shared_ptr<Epoll>& EventLoop::getep() const {
    return ep;
}
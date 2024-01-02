#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <mutex>
#include <queue>

template <class T>
class SafeQueue {
public:
    SafeQueue();
    SafeQueue(SafeQueue& other);
    ~SafeQueue();
    const bool empty();
    const int size();
    void enqueue(T& t);
    bool dequeue(T& t);

private:
  std::queue<T> m_queue;
  std::mutex m_mutex;
};

#endif
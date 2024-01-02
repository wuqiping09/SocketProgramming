#include "safequeue.h"
#include <functional>

template <class T>
SafeQueue<T>::SafeQueue() {

}

template <>
SafeQueue<std::function<void()>>::SafeQueue() {

}

template <class T>
SafeQueue<T>::SafeQueue(SafeQueue& other) {

}

template <class T>
SafeQueue<T>::~SafeQueue() {

}

template <>
SafeQueue<std::function<void()>>::~SafeQueue() {

}

template <class T>
const bool SafeQueue<T>::empty() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

template <>
const bool SafeQueue<std::function<void()>>::empty() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

template <class T>
const int SafeQueue<T>::size() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_queue.size();
}

template <>
const int SafeQueue<std::function<void()>>::size() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_queue.size();
}

template <class T>
void SafeQueue<T>::enqueue(T& t) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_queue.push(t);
}

template <>
void SafeQueue<std::function<void()>>::enqueue(std::function<void()>& t) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_queue.push(t);
}

template <class T>
bool SafeQueue<T>::dequeue(T& t) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
        return false;
    }
    t = std::move(m_queue.front());
    m_queue.pop();
    return true;
}

template <>
bool SafeQueue<std::function<void()>>::dequeue(std::function<void()>& t) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
        return false;
    }
    t = std::move(m_queue.front());
    m_queue.pop();
    return true;
}
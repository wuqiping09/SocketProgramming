#include "threadworker.h"

ThreadWorker::ThreadWorker(std::shared_ptr<ThreadPool> &pool, const int id): m_pool(pool), m_id(id) {

}

void ThreadWorker::operator()() {
    std::function<void()> func;
    bool dequeued;
    while (!m_pool->isshutdown()) {
        {
            std::unique_lock<std::mutex> lock(m_pool->conditionmutex());
            if (m_pool->queue().empty()) {
                m_pool->conditionlock().wait(lock);
            }
            dequeued = m_pool->queue().dequeue(func);
        }
        if (dequeued) {
            func();
        }
    }
}
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "safequeue.h"
#include "threadworker.h"
#include "resultdata.h"
#include "processor.h"
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool {
public:
    static std::shared_ptr<ThreadPool> get();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool& operator=(const ThreadPool &) = delete;
    ThreadPool& operator=(ThreadPool &&) = delete;
    ~ThreadPool();
    void init();
    void shutdown();
    const bool isshutdown() const;
    SafeQueue<std::function<void()>>& queue();
    std::mutex& conditionmutex();
    std::condition_variable& conditionlock();

    template<class F, class...Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype((f(args...)))>;

private:
    static std::shared_ptr<ThreadPool> instance;
    bool m_shutdown;
    SafeQueue<std::function<void()>> m_queue;
    std::vector<std::thread> m_threads;
    std::mutex m_conditional_mutex;
    std::condition_variable m_conditional_lock;

    static constexpr int NUM_THREAD = 5;
    ThreadPool(const int n_threads);
};

#endif
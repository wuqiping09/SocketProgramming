#include "threadpool.h"

std::shared_ptr<ThreadPool> ThreadPool::instance = nullptr;

std::shared_ptr<ThreadPool> ThreadPool::get() {
    if(instance == nullptr) {
        //instance = std::make_shared<ThreadPool>(ThreadPool::NUM_THREAD);
        instance = std::shared_ptr<ThreadPool>(new ThreadPool(ThreadPool::NUM_THREAD));
    }
    return instance;
}

ThreadPool::ThreadPool(const int n_threads): m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false) {
    
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::init() {
    for (int i = 0; i < m_threads.size(); ++i) {
      m_threads[i] = std::thread(ThreadWorker(instance, i));
    }
}

void ThreadPool::shutdown() {
    m_shutdown = true;
    m_conditional_lock.notify_all();
    for (int i = 0; i < m_threads.size(); ++i) {
        if(m_threads[i].joinable()) {
            m_threads[i].join();
        }
    }
}

const bool ThreadPool::isshutdown() const {
    return m_shutdown;
}

SafeQueue<std::function<void()>>& ThreadPool::queue() {
    return m_queue;
}

std::mutex& ThreadPool::conditionmutex() {
    return m_conditional_mutex;
}

std::condition_variable& ThreadPool::conditionlock() {
    return m_conditional_lock;
}

template<class F, class...Args>
auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
    std::function<void()> wrapper_func = [task_ptr]() {
        (*task_ptr)();
    };
    m_queue.enqueue(wrapper_func);
    m_conditional_lock.notify_one();
    return task_ptr->get_future();
}
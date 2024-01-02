#ifndef THREADWORKER_H
#define THREADWORKER_H

#include "threadpool.h"
#include <memory>

class ThreadPool;

class ThreadWorker {
public:
    ThreadWorker(std::shared_ptr<ThreadPool> &pool, const int id);
    void operator()();

private:
    int m_id;
    std::shared_ptr<ThreadPool> m_pool;
};

#endif
#include "Logger.h"
#include "ThreadPool.h"
#include "Eventloop.h"
#include "LoopThread.h"

ThreadPool::ThreadPool(Eventloop *loop, int num) : ownerloop_(loop),
                                                   numThreads_(num),
                                                   next_(0)
{
    if (num <= 0)
    {
        LOG_FATAL("%s", "ThreadPool: numThreads error");
    }
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start()
{
    LOG_TRACE("ThreadPool: creating %d threads", numThreads_);
    for (int i = 0; i < numThreads_; i++)
    {
        auto t = std::make_unique<LoopThread>();
        loops_.push_back(t->getLoop());
        threads_.push_back(std::move(t));
    }
}

void ThreadPool::stop()
{
    LOG_TRACE("%s", "ThreadPool: stopping all threads");
    //this calls each thread's destructor automatically
    threads_.clear();
    //invalid now
    loops_.clear();
    LOG_TRACE("%s", "ThreadPool: all threads stopped");
}

Eventloop *ThreadPool::getNext()
{
    auto ret = loops_[next_];
    next_ = (next_ + 1) % loops_.size();
    return ret;
}
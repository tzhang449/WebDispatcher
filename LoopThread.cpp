#include "LoopThread.h"
#include "Eventloop.h"
#include "Logger.h"

LoopThread::LoopThread() : looping_(false)
{
}

LoopThread::~LoopThread()
{
    stop();
}

Eventloop *LoopThread::getLoop()
{
    return loop_;
}

void LoopThread::start()
{
    looping_ = true;
    thread_ = std::thread(&LoopThread::threadFunc, this);

    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock);
}

void LoopThread::stop()
{
    looping_ = false;
    loop_->quit();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

void LoopThread::threadFunc()
{
    Eventloop loop;
    {
        std::lock_guard<std::mutex> guard(mutex_);

        loop_ = &loop;
        cond_.notify_all();
    }
    LOG_TRACE("LoopThread %d started", ThreadID::getThreadID());
    while (looping_)
    {
        loop.loop();
    }
}
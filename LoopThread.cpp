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
    return &loop_;
}

void LoopThread::start()
{
    looping_ = true;
    thread_ = std::thread(&LoopThread::threadFunc, this);
}

void LoopThread::stop()
{
    looping_ = false;
    loop_.quit();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

void LoopThread::threadFunc()
{    
    LOG_TRACE("LoopThread %d started",ThreadID::getThreadID());
    while (looping_)
    {
        loop_.loop();
    }
}
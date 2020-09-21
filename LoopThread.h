#ifndef _LOOPTHREAD_H
#define _LOOPTHREAD_H

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "Noncopyable.h"
#include "Eventloop.h"

class LoopThread : Noncopyable
{
public:
    LoopThread();
    ~LoopThread();

    Eventloop *getLoop();
    void start();
    void stop();

private:
    void threadFunc();

    Eventloop *loop_;
    std::thread thread_;
    std::atomic<bool> looping_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif
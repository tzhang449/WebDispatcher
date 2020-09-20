#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <vector>
#include <memory>

#include "Noncopyable.h"

class Eventloop;
class LoopThread;

class ThreadPool : Noncopyable
{
public:
    ThreadPool(Eventloop *owner, int num);
    ~ThreadPool();


    void start();
    void stop();

    Eventloop *getNext();

private:
    Eventloop *ownerloop_;
    int numThreads_;

    std::vector<std::unique_ptr<LoopThread>> threads_;
    std::vector<Eventloop *> loops_;

    int next_;
};

#endif
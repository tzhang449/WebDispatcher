#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <atomic>

#include "Noncopyable.h"

class Eventloop : Noncopyable
{
public:
    Eventloop();
    ~Eventloop();

    void loop();

private:
    std::atomic<bool> looping;

    std::unique_ptr<Epoller> epoller_;
};

#endif
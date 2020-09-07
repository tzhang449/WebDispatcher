#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include "atomic.h"

#include "Noncopyable.h"

class Eventloop: Noncopyable{
public: 
    Eventloop();
    ~Eventloop();

    void loop();

private:
    atomic<bool> looping;
};

#endif
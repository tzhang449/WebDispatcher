#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <atomic>
#include <vector>
#include <functional>
#include <mutex>

#include "Noncopyable.h"
#include "Channel.h"

class Epoller;

class ThreadID
{
public:
    static pid_t getThreadID();
};

class Eventloop : Noncopyable
{
public:
    typedef std::function<void()> Functor;

    Eventloop();
    ~Eventloop();

    void loop();
    void quit();

    void runInLoop(Functor func);

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

private:
    bool inLoopThread();

    void wakeUp();
    void wakedUp();

    static const int EpollTimeOutMs = 100000;

    typedef std::vector<Channel *> ChannelList;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> callingPendings_;

    std::unique_ptr<Epoller> epoller_;

    ChannelList actives_;

    std::mutex mutex_;

    std::vector<Functor> pendings_;

    pid_t ownerThread_;

    int wakeupFd_;
    Channel wakeupChannel_;
};

#endif
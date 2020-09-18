#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <atomic>
#include <vector>

#include "Noncopyable.h"

class Epoller;
class Channel;

class Eventloop : Noncopyable
{
public:
    Eventloop();
    ~Eventloop();

    void loop();
    void quit();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

private:
    static const int EpollTimeOutMs = 100000;

    typedef std::vector<Channel *> ChannelList;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;

    std::unique_ptr<Epoller> epoller_;

    ChannelList actives_;
};

#endif
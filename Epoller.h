#ifndef _EPOLLER_H
#define _EPOLLER_H

#include <map>
#include <memory>

#include "Noncopyable.h"

class Epoller : Noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    Epoller(Eventloop *loop);
    ~Epoller();

    void poll(int timeoutMs, ChannelList *active);

private:
    static const int InitEventListSize = 1000;

    void fillActive(int numEvents, ChannelList *active);

    Eventloop &ownerLoop_;
    std::map<int, Channel*> channels_;

    int epollfd_;
    std::vector<epoll_event> events_;
};

#endif

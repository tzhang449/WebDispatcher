#ifndef _EPOLLER_H
#define _EPOLLER_H

#include "sys/epoll.h"

#include <map>
#include <memory>

#include "Noncopyable.h"

class Channel;
class Eventloop;

class Epoller : Noncopyable
{
public:
    //Mark for the state of the Channel (Channel.index_)
    typedef std::vector<Channel *> ChannelList;

    Epoller(Eventloop *loop);
    ~Epoller();

    void poll(int timeoutMs, ChannelList *active);

    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    enum
    {
        NEW,
        ADDED,
        DELETED
    };

private:
    //Initial Eventlist size
    static const int InitEventListSize = 1000;

    void fillActive(int numEvents, ChannelList *active);

    void update(int op, Channel *channel);

    const char *opToStr(int op);

    Eventloop *loop_;

    int epollfd_;
    std::vector<epoll_event> events_;
};

#endif

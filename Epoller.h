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
    typedef std::vector<Channel *> ChannelList;

    Epoller(Eventloop *loop);
    ~Epoller();

    void poll(int timeoutMs, ChannelList *active);

    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    //possible state of the channel. stored in channel.index_
    enum
    {
        NEW,
        ADDED,
        DELETED
    };

private:
    //initial Eventlist size
    static const int InitEventListSize = 1000;

    void fillActive(int numEvents, ChannelList *active);

    void update(int op, Channel *channel);

    const char *opToStr(int op);

    Eventloop *loop_;

    int epollfd_;
    std::vector<epoll_event> events_;
};

#endif

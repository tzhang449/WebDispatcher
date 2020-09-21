#include "sys/epoll.h"
#include "unistd.h"

#include "cstring"

#include "Logger.h"
#include "Epoller.h"
#include "Eventloop.h"
#include "Channel.h"

Epoller::Epoller(Eventloop *loop) : loop_(loop),
                                    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
                                    events_(InitEventListSize)
{
    if (epollfd_ < 0)
        LOG_SYSFATAL("Epoller: %s", strerror(errno));
}

Epoller::~Epoller()
{
    ::close(epollfd_);
}

void Epoller::poll(int timeoutMs, ChannelList *active)
{
    LOG_TRACE("Epoller: fd total count %d", events_.size());

    int numEvents = ::epoll_wait(epollfd_,
                                 events_.data(),
                                 static_cast<int>(events_.size()),
                                 timeoutMs);

    int savedErrno = errno;

    if (numEvents > 0)
    {
        LOG_TRACE("Epoller: %d events available", numEvents);
        fillActive(numEvents, active);
    }
    else if (numEvents == 0)
    {
        LOG_TRACE("%s", "Epoller: no active events");
    }
    else
    {
        if (savedErrno != EINTR)
        {
            LOG_FATAL("Epoller: %s", strerror(savedErrno));
        }
    }
}
void Epoller::updateChannel(Channel *channel)
{   
    
    LOG_TRACE("Epoller: updateChannel event={%s}", channel->eventsToStr(channel->fd(), channel->events()).c_str());
    switch (channel->index())
    {
    case NEW:
    case DELETED:
        update(EPOLL_CTL_ADD, channel);
        channel->setIndex(ADDED);
        break;
    case ADDED:
        if (channel->events() == Channel::NOEVENT)
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(DELETED);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
        break;
    default:
        LOG_FATAL("%s", "Epoller: channel with invalid index");
        break;
    }
}

void Epoller::removeChannel(Channel *channel)
{
    LOG_TRACE("Epoller: fd=%d index=%d", channel->fd(), channel->index());
    if (channel->index() == ADDED)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(NEW);
}

void Epoller::fillActive(int numEvents, ChannelList *active)
{
    for (int i = 0; i < numEvents; i++)
    {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        active->push_back(channel);
    }
}

void Epoller::update(int op, Channel *channel)
{
    struct epoll_event ev;
    ev.events = channel->events();
    ev.data.ptr = channel;
    LOG_TRACE("Epoller: epoll_ctl op=%s event={%s}",
              opToStr(op),
              channel->eventsToStr(channel->fd(), channel->events()).c_str());
    if (::epoll_ctl(epollfd_, op, channel->fd(), &ev) < 0)
    {
        if (op == EPOLL_CTL_DEL)
        {
            LOG_SYSERROR("Epoller: epoll_ctl op=%s fd=%d error(%s)", opToStr(op), channel->fd(), strerror(errno));
        }
        else
        {
            LOG_SYSFATAL("Epoller: epoll_ctl op=%s fd=%d error(%s)", opToStr(op), channel->fd(), strerror(errno));
        }
    }
}

const char *Epoller::opToStr(int op)
{
    switch (op)
    {
    case EPOLL_CTL_ADD:
        return "ADD";
    case EPOLL_CTL_DEL:
        return "DEL";
    case EPOLL_CTL_MOD:
        return "MOD";
    default:
        break;
    }
    //should not reach here
    return "UNKNOWN";
}
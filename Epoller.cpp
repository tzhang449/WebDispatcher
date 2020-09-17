#include "sys/epoll.h"
#include "unistd.h"

#include "cstring"

#include "Logger.h"
#include "Epoller.h"
#include "Eventloop.h"

Epoller::Epoller(Eventloop *loop) : ownerLoop_(loop),
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
    } else if(numEvents ==0){
        LOG_TRACE("%s","Epoller: no active events");
    } else{
        if(savedErrno != EINTR){
            LOG_FATAL("Epoller: %s",strerror(savedErrno));
        }
    }
}

void Epoller::fillActive(int numEvents, ChannelList *active){
    //to do
}
#include "sys/epoll.h"

#include "sstream"

#include "Logger.h"
#include "Channel.h"
#include "Eventloop.h"
#include "Epoller.h"

Channel::Channel(Eventloop *loop, int fd) : loop_(loop),
                                            fd_(fd),
                                            events_(NOEVENT),
                                            revents_(NOEVENT),
                                            index_(Epoller::NEW)
{
}

Channel::~Channel()
{
}

void Channel::handleEvent()
{
    LOG_TRACE("Channel: event = {%s}", eventsToStr(fd_, revents_).c_str());
    if ((revents_ & EPOLLHUP && !(revents_ & EPOLLIN)))
    {
        if (closeCb_)
            closeCb_();
    }
    if (revents_ & (EPOLLERR))
    {
        if (ErrCb_)
            ErrCb_();
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        if (readCb_)
            readCb_();
    }
    if (revents_ & (EPOLLOUT))
    {
        if (writeCb_)
            writeCb_();
    }
}

void Channel::setReadCb(EventCb cb)
{
    readCb_ = std::move(cb);
}

void Channel::setWriteCb(EventCb cb)
{
    writeCb_ = std::move(cb);
}

void Channel::setCloseCb(EventCb cb)
{
    closeCb_ = std::move(cb);
}

void Channel::setErrCb(EventCb cb)
{
    ErrCb_ = std::move(cb);
}

void Channel::enableRead()
{
    events_ |= READEVENT;
    update();
}

void Channel::enableWrite()
{
    events_ |= WRITEEVENT;
    update();
}

void Channel::disableAll()
{
    events_ = NOEVENT;
    update();
}

void Channel::removeAllCb()
{
    readCb_ = nullptr;
    writeCb_ = nullptr;
    closeCb_ = nullptr;
    ErrCb_ = nullptr;
}

int Channel::index()
{
    return index_;
}

void Channel::setIndex(int index)
{
    index_ = index;
}

int Channel::fd()
{
    return fd_;
}

int Channel::events()
{
    return events_;
}

void Channel::setRevents(int revents)
{
    revents_ = revents;
}

void Channel::update()
{
    loop_->updateChannel(this);
}

std::string Channel::eventsToStr(int fd, int events)
{
    std::stringstream oss;
    oss << fd << ": ";
    if (events & EPOLLET)
        oss << "ET ";
    if (events & EPOLLIN)
        oss << "IN ";
    if (events & EPOLLPRI)
        oss << "PRI ";
    if (events & EPOLLOUT)
        oss << "OUT ";
    if (events & EPOLLHUP)
        oss << "HUP ";
    if (events & EPOLLRDHUP)
        oss << "RDHUP ";
    if (events & EPOLLERR)
        oss << "ERR ";
    return oss.str();
}

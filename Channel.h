#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "sys/epoll.h"

#include "functional"
#include "string"

#include "Noncopyable.h"

class Eventloop;

class Channel : Noncopyable
{
public:
    typedef std::function<void()> EventCb;

    Channel(Eventloop *loop, int fd);
    ~Channel();

    void setReadCb(EventCb cb);
    void setWriteCb(EventCb cb);
    void setCloseCb(EventCb cb);
    void setErrCb(EventCb cb);

    void enableRead();
    void enableWrite();
    void disableAll();

    void removeAllCb();

    void setRevents(int revents);

    void handleEvent();

    int index();

    void setIndex(int index);

    int fd();

    int events();

    std::string eventsToStr(int fd, int events);

    static const int NOEVENT = 0;
    static const int READEVENT = EPOLLIN | EPOLLPRI | EPOLLET;
    static const int WRITEEVENT = EPOLLOUT | EPOLLET;

private:
    void update();

    void handle();

    Eventloop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;
    EventCb readCb_;
    EventCb writeCb_;
    EventCb closeCb_;
    EventCb ErrCb_;
};

#endif
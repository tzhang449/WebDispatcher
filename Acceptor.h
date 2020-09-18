#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H
#include <netinet/in.h>

#include <functional>

#include "Channel.h"

class Eventloop;

class Acceptor
{
    typedef std::function<void(int, struct sockaddr_in)> ConnCb;

    Acceptor(Eventloop *loop, unsigned short port, ConnCb cb);

    void listen();

private:
    void onRead();
    void setSockOpt(int op);

    Eventloop *loop_;
    int listenfd_;
    Channel channel_;
    ConnCb connCb_;
    int idlefd_;
};

#endif
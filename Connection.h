#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <memory>

#include "Channel.h"

class Eventloop;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    Connection(Eventloop *loop, int fd);
    ~Connection();

    void init();

private:
    void handleRead();
    void close();

    int fd_;
    Eventloop *loop_;
    Channel channel_;
};

#endif
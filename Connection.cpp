#include "unistd.h"

#include "cstdio"

#include "Logger.h"
#include "Connection.h"

Connection::Connection(Eventloop *loop, int fd) : fd_(fd),
                                                  loop_(loop),
                                                  channel_(loop, fd)
{
}

Connection::~Connection()
{
    LOG_TRACE("Connection: close fd=%d", fd_);
    ::close(fd_);
}

void Connection::init()
{
    channel_.setReadCb(std::bind(&Connection::handleRead, shared_from_this()));
    channel_.enableRead();
}

void Connection::handleRead()
{
    char buf[100];
    int len = 0;
    while ((len = ::read(fd_, buf, sizeof(buf))) > 0)
    {
        char message[100];
        snprintf(message, len, "%s", buf);
        LOG_TRACE("Connection: message from fd=%d \"%s\"", fd_, message);
        ::write(fd_, buf, len);
    }
    close();
}

void Connection::close()
{
    LOG_TRACE("Connection: conn closed with fd=%d", fd_);
    std::shared_ptr<Connection> guard = shared_from_this();
    channel_.disableAll();
    channel_.removeAllCb();
}
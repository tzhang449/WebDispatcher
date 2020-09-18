#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "cstring"
#include "functional"
#include "vector"

#include "Logger.h"
#include "Acceptor.h"

Acceptor::Acceptor(Eventloop *loop,
                   unsigned short port,
                   ConnCb cb = nullptr) : loop_(loop),
                                          listenfd_(::socket(AF_INET,
                                                             SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)),
                                          channel_(loop, listenfd_),
                                          connCb_(cb),
                                          idlefd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    if (listenfd_ < 0)
    {
        LOG_SYSFATAL("%s", "Acceptor: create socket failed");
    }

    setSockOpt(SO_REUSEADDR);
    setSockOpt(SO_REUSEPORT);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int ret = ::bind(listenfd_,
                     reinterpret_cast<sockaddr *>(&servaddr),
                     static_cast<socklen_t>(sizeof(servaddr)));

    if (ret < 0)
    {
        LOG_SYSFATAL("%s", "Acceptor: socket bind failed");
    }

    LOG_TRACE("Acceptor: create socket on port %d", port);

    channel_.setReadCb(std::bind(&Acceptor::onRead, this));
}

void Acceptor::listen()
{
    int ret = ::listen(listenfd_, SOMAXCONN);

    if (ret < 0)
    {
        LOG_SYSFATAL("%s", "Acceptor: listen on socket failed");
    }

    LOG_TRACE("%s", "Acceptor: start listening");

    channel_.enableRead();
}

void Acceptor::onRead()
{
    struct sockaddr_in addr;
    socklen_t len = static_cast<socklen_t>(sizeof(addr));
    std::vector<std::pair<int, struct sockaddr_in>> connections;

    while (1)
    {
        int connfd = ::accept4(listenfd_,
                               reinterpret_cast<sockaddr *>(&addr),
                               &len,
                               SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (connfd < 0)
        {
            int savedErrno = errno;
            switch (savedErrno)
            {
            case EINTR:
                continue;
                break;
            // No more fd available. To handle this,
            // reserve an idlefd for the most recent conn and close
            // it immedieately.
            case EMFILE:
                LOG_ERROR("%s", "Acceptor: No more fd available. Close the current connection");
                close(idlefd_);
                idlefd_ = ::accept(listenfd_, NULL, NULL);
                close(idlefd_);
                idlefd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
                continue;
                break;
            case EAGAIN:
                break;
            case ECONNABORTED:
            case EPROTO:
            case EPERM:
                LOG_SYSERROR("Acceptor: accept error(%s)", strerror(savedErrno));
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                LOG_SYSFATAL("Acceptor: accept fatal(%s)", strerror(savedErrno));
                break;
            default:
                LOG_SYSFATAL("%s", "Acceptor: accept unkwown error");
                break;
            }
            break;
        }
        else
        {
            char buf[20];
            LOG_TRACE("Acceptor: connection from %s, port %d",
                      inet_ntop(AF_INET,
                                &addr.sin_addr,
                                buf,
                                static_cast<socklen_t>(sizeof(buf))),
                      ntohs(addr.sin_port));
            connections.push_back({connfd, addr});
        }
    }
    if (connCb_)
    {
        for (auto pair : connections)
        {
            connCb_(pair.first, pair.second);
        }
    }
    else
    {
        for (auto pair : connections)
        {
            if (::close(pair.first) < 0)
            {
                LOG_SYSERROR("%s", "Acceptor: close conn failed");
            }
        }
    }
}

void Acceptor::setSockOpt(int op)
{
    int optval = 1;
    int ret = ::setsockopt(listenfd_,
                           SOL_SOCKET, op,
                           &optval,
                           static_cast<socklen_t>(sizeof optval));
    if (ret < 0)
        LOG_SYSERROR("%s", "Acceptor: set socket option(%d) failed", op);
}

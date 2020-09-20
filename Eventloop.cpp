#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

#include <memory>
#include <cstring>
#include <functional>

#include "Logger.h"
#include "Eventloop.h"
#include "Epoller.h"
#include "Channel.h"

namespace
{
    class IgnoreSigPipe
    {
    public:
        IgnoreSigPipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
            LOG_TRACE("%s", "Ignore SIGPIPE");
        }
    };
    IgnoreSigPipe initObj;
} // namespace

pid_t ThreadID::getThreadID()
{
    thread_local static pid_t threadID;
    if (threadID == 0)
    {
        threadID = ::syscall(SYS_gettid);
    }
    return threadID;
}

Eventloop::Eventloop() : looping_(false),
                         quit_(false),
                         callingPendings_(false),
                         epoller_(std::make_unique<Epoller>(this)),
                         ownerThread_(ThreadID::getThreadID()),
                         wakeupFd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
                         wakeupChannel_(this, wakeupFd_)
{
    if (wakeupFd_ < 0)
        LOG_SYSFATAL("Eventloop: wakeupFd create failed(%s)", strerror(errno));
    wakeupChannel_.setReadCb(std::bind(&Eventloop::wakedUp, this));
    wakeupChannel_.enableRead();
}

Eventloop::~Eventloop()
{
}

void Eventloop::loop()
{
    looping_ = true;

    LOG_TRACE("Eventloop %p start looping", this);

    while (!quit_)
    {
        //epoll
        actives_.clear();
        epoller_->poll(EpollTimeOutMs, &actives_);
        for (Channel *channel : actives_)
        {
            channel->handleEvent();
        }
        //doing pending functors
        std::vector<Functor> functors;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            functors.swap(pendings_);
            callingPendings_ = true;
        }
        for (auto &functor : functors)
        {
            functor();
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callingPendings_ = false;
        }
    }

    LOG_TRACE("Eventloop %p stop looping", this);

    looping_ = false;
}

void Eventloop::quit()
{
    quit_ = true;
}

void Eventloop::runInLoop(Functor func)
{
    if (inLoopThread())
    {
        func();
    }
    else
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pendings_.push_back(std::move(func));
        }
        wakeUp();
    }
}

void Eventloop::updateChannel(Channel *channel)
{
    epoller_->updateChannel(channel);
}

void Eventloop::removeChannel(Channel *channel)
{
    epoller_->removeChannel(channel);
}

bool Eventloop::inLoopThread()
{
    return ownerThread_ == ThreadID::getThreadID();
}

void Eventloop::wakeUp()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("Eventloop: wakeup writes %d bytes", n);
    }
}

void Eventloop::wakedUp()
{
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("Eventloop: wakeup reads %d bytes", n);
    }
}
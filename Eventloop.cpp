#include <poll.h>

#include <memory>

#include "Logger.h"
#include "Eventloop.h"
#include "Epoller.h"
#include "Channel.h"

Eventloop::Eventloop() : looping_(false),
                         quit_(false),
                         epoller_(std::make_unique<Epoller>(this))
{
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
        actives_.clear();
        epoller_->poll(EpollTimeOutMs, &actives_);
        for (Channel *channel : actives_)
        {
            channel->handleEvent();
        }
    }

    LOG_TRACE("Eventloop %p stop looping", this);

    looping_ = false;
}

void Eventloop::quit()
{
    quit_ = true;
}

void Eventloop::updateChannel(Channel *channel)
{
    epoller_->updateChannel(channel);
}

void Eventloop::removeChannel(Channel *channel)
{
    epoller_->removeChannel(channel);
}
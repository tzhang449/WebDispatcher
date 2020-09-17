#include <poll.h>

#include "Logger.h"
#include "Eventloop.h"
#include "Epoller.h"

Eventloop::Eventloop() : looping(false)
{
    
}

Eventloop::~Eventloop()
{
}

void Eventloop::loop()
{
    looping = true;
    ::poll(nullptr, 0, 5 * 1000);

    LOG_TRACE("EventLoop %p stop looping", this);
    looping = false;
}
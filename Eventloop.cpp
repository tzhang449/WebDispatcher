#include <poll.h>

#include "Eventloop.h"

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
    looping = false;
}
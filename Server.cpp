#include <functional>

#include "Server.h"
#include "Logger.h"

using namespace std::placeholders;

Server::Server(int port, int num) : loop_(),
                                    acceptor_(&loop_, port, std::bind(&Server::newConnection, this, _1, _2)),
                                    looping_(false),
                                    pool_(&loop_, num)
{
}

Server::~Server()
{
    stop();
}

void Server::start()
{
    LOG_TRACE("%s", "Server: server starting");
    looping_ = true;
    thread_ = std::thread(&threadFunc, this);
}

void Server::stop()
{
    LOG_TRACE("%s", "Server: server stopping");
    looping_ = false;
    loop_.quit();
    if (thread_.joinable())
        thread_.join();
    LOG_TRACE("%s", "Server: server stopped");
}

void Server::threadFunc()
{
    while (looping_)
    {
        loop_.loop();
    }
}

void Server::newConnection(int connfd, struct sockaddr_in addr)
{
    Eventloop *nextLoop = pool_.getNext();
}
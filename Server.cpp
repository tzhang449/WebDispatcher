#include <functional>

#include "Server.h"
#include "Logger.h"
#include "Connection.h"

using namespace std::placeholders;

Server::Server(int port, int num) : acceptor_(&loop_, port, std::bind(&Server::newConnection, this, _1, _2)),
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
    thread_ = std::thread(&Server::threadFunc, this);
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
    pool_.start();
    acceptor_.listen();
    while (looping_)
    {
        loop_.loop();
    }
}

void Server::newConnection(int connfd, struct sockaddr_in addr)
{
    Eventloop *loop = pool_.getNext();
    LOG_TRACE("Server: new Conn added into loop %p", loop);
    auto guard = std::make_shared<Connection>(loop, connfd);
    loop_.runInLoop(std::bind(&Connection::init, guard));
}
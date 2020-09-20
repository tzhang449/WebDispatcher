#ifndef _SERVER_H
#define _SERVER_H
#include <netinet/in.h>

#include <thread>

#include "Acceptor.h"
#include "Eventloop.h"
#include "Noncopyable.h"
#include "ThreadPool.h"

class Server : Noncopyable
{
public:
    Server(int port, int threadNum_);
    ~Server();

    void start();
    void stop();

private:
    void threadFunc();

    void newConnection(int connfd, struct sockaddr_in addr);

    Eventloop loop_;
    Acceptor acceptor_;
    std::thread thread_;
    //Server belongs to its owner thread. No contention here.
    bool looping_;

    ThreadPool pool_;
};

#endif
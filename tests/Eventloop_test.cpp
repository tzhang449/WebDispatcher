#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstdio>
#include <thread>
#include <functional>

#include "Eventloop.h"
#include "Logger.h"
#include "Channel.h"

//This test tests the behavior of the main reactor 
// on an echo server

void onRead(int listenfd)
{
    const int MAXLINE = 1000;
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    char buff[MAXLINE];

    connfd = accept(listenfd, (sockaddr *)&cliaddr, &len);
    LOG_TRACE("echo: connection from %s, port %d",
              inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
              ntohs(cliaddr.sin_port));

    ssize_t n;
    do
    {
        while ((n = read(connfd, buff, MAXLINE)) > 0)
        {
            char message[MAXLINE];
            snprintf(message, n-1, "%s", buff);
            LOG_TRACE("Echo: read from %d with message(%s)",
                      connfd, message);
            write(connfd, buff, n);
        }

        if (n < 0 && errno != EINTR)
            LOG_FATAL("%s", "echo: read error");
    } while (n < 0 && errno == EINTR);
}

int main()
{
    int listenfd;
    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1984);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 10);

    Eventloop loop;

    Channel channel(&loop, listenfd);
    channel.setReadCb(std::bind(onRead, channel.fd()));
    channel.enableRead();

    loop.loop();
}
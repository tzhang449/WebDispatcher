#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "Logger.h"
#include "Acceptor.h"
#include "Eventloop.h"

void onConnection(int connfd, struct sockaddr_in addr)
{
    char buf[20];
    LOG_TRACE("Echo server: connection address %s , port %d",
              inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf)),
              ntohs(addr.sin_port));

    const int MAXLINE = 1000;
    char buff[1000];
    ssize_t n;
    do
    {
        while ((n = read(connfd, buff, MAXLINE)) > 0)
        {
            char message[MAXLINE];
            snprintf(message, n - 1, "%s", buff);
            LOG_TRACE("Echo server: read from %d with message(%s)",
                      connfd, message);
            write(connfd, buff, n);
        }

        if (n < 0 && errno != EAGAIN)
            LOG_FATAL("Echo server: read error(%s)", strerror(errno));
    } while (n < 0 && errno == EAGAIN);
}
int main()
{
    Eventloop loop;
    Acceptor acceptor(&loop, 1984, &onConnection);
    acceptor.listen();
    loop.loop();
}
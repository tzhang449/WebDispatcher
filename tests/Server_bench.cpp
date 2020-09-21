#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"

#include "chrono"
#include "iostream"
#include "cassert"

#include "Logger.h"
#include "Server.h"

int main()
{
    const int port = 1984;
    const int threadNum = 4;
    Server server(port, threadNum);
    server.start();

    ::sleep(5);

    const int numMessage = 100;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1984);
    ::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < numMessage; i++)
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        ::connect(fd,
                  reinterpret_cast<struct sockaddr *>(&addr),
                  static_cast<socklen_t>(sizeof(addr)));
        ::write(fd, &i, sizeof(i));
        int val;
        ::read(fd, &val, sizeof(val));

        assert(i == val);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "QPS is:" << 1000000*(numMessage / static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())) << std::endl;
}
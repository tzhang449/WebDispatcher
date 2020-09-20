#include "string"
#include "iostream"

#include "Logger.h"
#include "Server.h"

int main()
{
    const int port = 1984;
    const int threadNum = 4;
    Server server(port, threadNum);
    server.start();

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.compare("exit") == 0)
        {
            server.stop();
            break;
        }
    }
}
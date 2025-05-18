#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include "Server.hpp"

int main(void)
{
    struct pollfd stdinfd{.fd = STDIN_FILENO, .events = POLLIN};
    int result;
    Server server;
    
    while (1)
    {
        result = poll(&stdinfd, 1, 200);
        if (result)
        {
            std::getline(std::cin, str);
        }
        server.pollFds();
    }

    return 0;
}

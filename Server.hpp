#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <vector>
#include <iostream>
#include "Client.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define RESET "\x1B[0m"
#define CYN   "\x1B[36m"
std::string str;
char msg[1024];

class Server
{
    private:
        std::vector<struct pollfd> _fds;
        std::vector<Client>        _clients;
        struct sockaddr_in         _address;
        socklen_t                  _addressLen = sizeof(_address);

    public:
        Server();
        int pollFds();
        int acceptClient();
};

int Server::acceptClient()
{
    struct pollfd client;
    
    client.events = POLLIN + POLLOUT;
    client.fd = accept(_fds.at(0).fd, (struct sockaddr*)&_address, &_addressLen);
    if (client.fd <= 0)
    {
        std::cerr << "accept call failed!" << std::endl;
        exit (1);
    }
    _fds.emplace_back(client);
    std::cout << "New client is here!" << std::endl;
    return 0;
}

int Server::pollFds()
{
    int result;

    result = poll(_fds.data(), _fds.size(), 200);
    if (result < 0)
    {
        std::cerr << "poll failed" << std::endl;
        exit (1);       // change theeeese
    }
    int i = 0;
    for (auto fd = _fds.begin(); fd != _fds.end() && i < result; fd++)
    {
        if (fd->revents & POLLIN || fd->revents & POLLOUT)
        {
            if (fd->fd == _fds.at(0).fd)
            {
                acceptClient();
            }
            else
            {
                if (fd->revents & POLLIN)
                {
                    int msgSize = read(fd->fd, msg, 1023);
                    msg[msgSize] = 0;
                    printf("%s%s%s\n",CYN, msg, RESET);
                }
                if (fd->revents & POLLOUT && str.length())
                {
                    send(fd->fd, str.c_str(), str.length(), 0);
                    str.clear();
                }

            }
            ++i;
        }
    }
    return result;
}

Server::Server() : 
    _address{.sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr = INADDR_ANY}
{
    struct pollfd sockfd; 
    int retVal;
    int optVal = 1;
    socklen_t addressLen = sizeof(_address);

    sockfd.events = POLLIN;
    sockfd.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd.fd == -1)
    {
        std::cerr << "Socket call failed!" << std::endl;
        exit(1);  // change theseeee
    }
    retVal = setsockopt(sockfd.fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optVal, sizeof(optVal)); 
    if (retVal != 0)
    {
        std::cerr << "Setsockopt call failed!" << std::endl;
        exit(1);  // change theseeee
    }

    retVal = bind(sockfd.fd, (struct sockaddr*)&_address, addressLen);
    if (retVal < 0)
    {
        std::cerr << "bind call failed!" << std::endl;
        exit(1);  // change theseeee
    }

    retVal = listen(sockfd.fd, 3);
    if (retVal < 0)
    {
        std::cerr << "listen call failed!" << std::endl;
        exit(1);  // change theseeee
    }
    _fds.emplace_back(sockfd);

}


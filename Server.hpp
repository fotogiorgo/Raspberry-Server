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
    return 0;
}

int Server::pollFds()
{
    int result;

    result = poll(_fds.data(), _fds.size(), 100);
    if (result == -1)
    {
        std::cerr << "poll failed" << std::endl;
        exit (1);       // change theeeese
    }
    for (int i = 0; i < result;)
    {
        if (_fds.at(i).revents & POLLIN || _fds.at(i).revents & POLLOUT)
        {
            if (i == 0)
            {
                acceptClient();
            }
            else
            {
                if (_fds.at(i).revents & POLLIN)
                {
                    int msgSize = read(_fds.at(i).fd, msg, 1023);
                    msg[msgSize] = 0;
                    printf("%s\n", msg);
                }
                else if (str.length())
                {
                    send(_fds.at(i).fd, str.c_str(), str.length(), 0);
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


#pragma once
#include <string>

class Client
{
    private:
        int _fd;
        std::string response;
    public:
        Client(int fd) : _fd(fd){}
};

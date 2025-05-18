#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>

#define RESET "\x1B[0m"
#define CYN   "\x1B[36m"

int main(void)
{
    int sockfd; 
    int retVal;
    int optVal = 1;
    struct sockaddr_in address{.sin_family = AF_INET,
                               .sin_port = htons(8080)};

    socklen_t addressLen = sizeof(address);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cerr << "Socket call failed!" << std::endl;
        return 1;
    }
    retVal = inet_pton(AF_INET, "192.168.1.113", &address.sin_addr);
    if (retVal <= 0)
    {
        std::cerr << "inet_pton call failed!" << std::endl;
        return 3;
    }

    retVal = connect(sockfd, (struct sockaddr*)&address, addressLen);
    if (retVal < 0)
    {
        std::cerr << "connect call failed!" << std::endl;
        return 3;
    }


    std::string str;
    char msg[1024];
    size_t msgSize;
    int result;

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO; 
    fds[0].events = POLLIN; 
    fds[1].fd = sockfd; 
    fds[1].events = POLLIN + POLLOUT; 

    while (1)
    {
        result = poll(fds, 2, 50);
        if (result == -1)
        {
            std::cerr << "poll failed!" << std::endl;
            return 1;
        }
        else if (result)
        {
            for (auto fd : fds)
            {
                if (fd.fd == STDIN_FILENO && fd.revents & POLLIN)
                {
                    std::getline(std::cin, str);
                }
                else if(fd.fd == sockfd && fd.revents & POLLIN)
                {
                    msgSize = read(sockfd, msg, 1023);
                    msg[msgSize] = 0;
                    printf("%s%s%s\n",CYN, msg, RESET);
                }
                else if(fd.fd == sockfd && fd.revents & POLLOUT && str.length())
                {
                    send(sockfd, str.c_str(), str.length(), 0);
                    str.clear();
                }
            }
        }
    }


    
    std::cout << "seems like everything went well!\nPress Enter to end!";

    close(sockfd);
    std::cout << "closed FD" << std::endl;
    return 0;
}

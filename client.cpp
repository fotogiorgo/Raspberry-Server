#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>

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
    retVal = inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
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
    while (1)
    {
        std::cin >> str;
        send(sockfd, str.c_str(), str.length(), 0);
        msgSize = read(sockfd, msg, 1023);
        msg[msgSize] = 0;
        printf("%s\n", msg);
    }


    
    std::cout << "seems like everything went well!\nPress Enter to end!";

    close(sockfd);
    std::cout << "closed FD" << std::endl;
    return 0;
}

#include "../include/wearable.h"

UDP::UDP()
{
}

UDP::~UDP()
{
    close();
}

int UDP::init_as_client()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(MIDDLEWARE_UDP_PORT);
    inet_pton(AF_INET, ip_address, &server_addr.sin_addr);

    return NO_ERROR;
}

int UDP::init_as_server()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return NO_ERROR;
}

int UDP::send(const char *data, size_t len)
{
    ssize_t sent_bytes = sendto(sockfd, data, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0)
    {
        perror("sendto");
        return ERROR;
    }
    return NO_ERROR;
}

int UDP::receive(char *buffer, size_t len, int flags)
{
    addr_len = sizeof(client_addr);
    ssize_t received_bytes = recvfrom(sockfd, buffer, len, flags, (struct sockaddr *)&client_addr, &addr_len);
    if (received_bytes < 0)
    {
        perror("recvfrom");
        return ERROR;
    }
    return received_bytes;
}

void UDP::close()
{
    if (sockfd >= 0)
    {
        ::close(sockfd);
        sockfd = -1;
    }
}
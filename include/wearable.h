/**
 * @file wearable.h
 *
 * @brief Header file for the wearable device.
 * @author Azzam Wildan (2025)
 */

#ifndef WEARABLE_H
#define WEARABLE_H

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "simple_fsm.h"
#include "custom_time.h"

#include "stdio.h"
#include "sys/ioctl.h"
#include "termios.h"

/**
 * Error conditions
 */

#define NO_ERROR 0
#define ERROR -1

/**
 * Network definitions
 */

#define HW_IO_UDP_PORT 0x1122
#define HW_IO_ID 0x12

#define MIDDLEWARE_UDP_PORT 0x2233
#define MIDDLEWARE_ID 0x23

#define CHATBOT_UDP_PORT 0x3344

/**
 * Hardware definitions
 */

#define BTN_VOLUME_UP 0x01
#define BTN_VOLUME_DOWN 0x02
#define BTN_RECORD_AUDIO 0x04
#define BTN_INTERFACE 0x08
#define BTN_TICTOC 0x10

/**
 * Some kind of misc definitions
 */

#define DEFAULT_AUDIO_FILE "/tmp/audio_in.wav"
#define LOCALHOST_ADDR "127.0.0.1"

/**
 * Netif Class
 */

class UDP
{
public:
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    char buffer[1024];
    socklen_t addr_len;

    int port;
    const char *ip_address;

public:
    UDP();
    ~UDP();

    int init_as_client();
    int init_as_server();
    int send(const char *data, size_t len);
    int receive(char *buffer, size_t len, int flags = 0);
    void close();
};

/**
 * Another class that i too lazy to create a new file
 */

class KBhit
{
public:
    /**
     * @brief Check if a key is pressed.
     *
     * @brief It's an unblocking function that returns the number of bytes waiting in the input buffer.
     *
     * @return int The number of bytes waiting in the input buffer.
     */
    int kbhit()
    {
        static const int STDIN = 0;
        static bool initialized = false;

        if (!initialized)
        {
            termios term;
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }

        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
    }
};

#endif
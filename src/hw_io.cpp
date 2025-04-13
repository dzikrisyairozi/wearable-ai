/**
 * @file hw_io.cpp
 *
 * @brief Hardware IO for the wearable device.
 *
 * @author Azzam Wildan (2025)
 */

#include "../include/wearable.h"

uint8_t button_state = 0;
uint8_t send_msg[2] = {0, 0};

UDP udp;
KBhit kbhit;

// ================================================================================================================

int init_all();
void process_hw();

// ================================================================================================================

int main(int argc, char *argv[])
{
    if (init_all() == ERROR)
    {
        fprintf(stderr, "Failed to initialize all\n");
        return ERROR;
    }
    while (1)
    {
        process_hw();

        send_msg[1] = button_state;
        int send_bytes = udp.send((const char *)send_msg, sizeof(send_msg));

        usleep(100000); // Sleep for 100ms
    }

    return NO_ERROR;
}

int init_all()
{
    udp.port = MIDDLEWARE_UDP_PORT;
    udp.ip_address = LOCALHOST_ADDR;

    if (udp.init_as_client() == ERROR)
    {
        fprintf(stderr, "Failed to initialize UDP client\n");
        return ERROR;
    }

    send_msg[0] = HW_IO_ID;

    return NO_ERROR;
}

// ================================================================================================================

void process_hw()
{
    static uint8_t btn_tictoc = 0;

    button_state &= ~BTN_TICTOC;
    button_state |= (btn_tictoc << 4);

    /* Testing */
    if (kbhit.kbhit())
    {
        char c = std::cin.get();
        switch (c)
        {
        case '1':
            button_state |= BTN_VOLUME_UP;
            break;
        case '2':
            button_state |= BTN_VOLUME_DOWN;
            break;
        case '3':
            button_state |= BTN_RECORD_AUDIO;
            break;
        case '4':
            button_state &= ~BTN_RECORD_AUDIO;
            break;
        case '5':
            btn_tictoc = 1;
            break;
        default:
            break;
        }
    }

    btn_tictoc = ~btn_tictoc;
}
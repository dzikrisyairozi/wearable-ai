/**
 * @file middleware.cpp
 *
 * @brief Middleware for the wearable device.
 * @brief This executable will produce an audio input as a WAV file and proceed udp trigger to higher level of the system.
 *
 * @author Azzam Wildan (2025)
 */

#include "../include/wearable.h"

#define TIME_PERIOD_US 100000
#define HYST_RECORD_IN_CNTR_THR 3  // 300ms
#define HYST_RECORD_OUT_CNTR_THR 3 // 300ms

MachineState record_fsm;
UDP udp_from_hw_io;

uint8_t button_state = 0;

// ================================================================================================================

int init_all();
void process_record_fsm();
void process_udp_server();
void process_args(int argc, char *argv[]);

// ================================================================================================================

int main(int argc, char *argv[])
{
    process_args(argc, argv);

    if (init_all() == ERROR)
    {
        fprintf(stderr, "Failed to initialize all\n");
        return ERROR;
    }
    while (1)
    {
        process_udp_server();
        process_record_fsm();

        usleep(TIME_PERIOD_US); // Sleep for 100ms
    }

    return NO_ERROR;
}

int init_all()
{
    udp_from_hw_io.port = MIDDLEWARE_UDP_PORT;
    udp_from_hw_io.ip_address = "";

    if (udp_from_hw_io.init_as_server() == ERROR)
    {
        fprintf(stderr, "Failed to initialize UDP client\n");
        return ERROR;
    }

    return NO_ERROR;
}

// ================================================================================================================

void process_args(int argc, char *argv[])
{
    if (argc > 1)
    {
        udp_from_hw_io.ip_address = argv[1];
    }
    else
    {
        udp_from_hw_io.ip_address = LOCALHOST_ADDR;
    }
}

void process_udp_server()
{
    int recv_bytes = udp_from_hw_io.receive(udp_from_hw_io.buffer, sizeof(udp_from_hw_io.buffer), 0);
    if (recv_bytes > 0)
    {
        if (udp_from_hw_io.buffer[0] == HW_IO_ID)
        {
            button_state = udp_from_hw_io.buffer[1];
            printf("Received button state: %02X\n", button_state);
        }
    }
}

void process_record_fsm()
{
    static uint16_t hysteresis_btn_in = 0;
    static uint16_t hysteresis_btn_out = 0;
    switch (record_fsm.value)
    {
        /* Wait for record button to be hold */
    case 0:
    {
        if ((button_state & BTN_RECORD_AUDIO) == BTN_RECORD_AUDIO)
        {
            hysteresis_btn_in++;
        }

        if (hysteresis_btn_in > HYST_RECORD_IN_CNTR_THR)
        {
            hysteresis_btn_in = 0;
            record_fsm.value = 1;
            printf("Record button pressed\n");
        }
        break;
    }

        /* Record here */
    case 1:
    {
        if ((button_state & BTN_RECORD_AUDIO) == 0)
        {
            hysteresis_btn_out++;
        }

        if (hysteresis_btn_out > HYST_RECORD_OUT_CNTR_THR)
        {
            hysteresis_btn_out = 0;
            record_fsm.value = 2;
            printf("Record button released\n");
        }
        break;
    }
        /* Stop recording and saving an audio file */
    }
}

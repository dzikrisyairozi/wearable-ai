/**
 * @file middleware.cpp
 *
 * @brief Middleware for the wearable device.
 * @brief This executable will produce an audio input as a WAV file and proceed udp trigger to higher level of the system.
 *
 * @author Azzam Wildan (2025)
 */

#include "../include/wearable.h"
#include "../include/audio_interface.h"

#define TIME_PERIOD_US 100000
#define HYST_RECORD_IN_CNTR_THR 3  // 300ms
#define HYST_RECORD_OUT_CNTR_THR 3 // 300ms
#define HYST_CHATBOT_HIGH_TRIGGER 0
#define HYST_CHATBOT_LOW_TRIGGER 0
#define AUDIO_OUT_FILENAME "recorded_audio.wav"

UDP udp_from_hw_io;
UDP udp_to_chatbot;
MachineState record_fsm;
AudioRecorder audio_recorder;

uint8_t button_state = 0;
uint8_t udp_chatbot_msg[2] = {0, 0};

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

    udp_to_chatbot.port = CHATBOT_UDP_PORT;
    udp_to_chatbot.ip_address = LOCALHOST_ADDR;

    if (udp_to_chatbot.init_as_client() == ERROR)
    {
        fprintf(stderr, "Failed to initialize UDP server\n");
        return ERROR;
    }

    udp_chatbot_msg[0] = MIDDLEWARE_ID;

    if (!audio_recorder.init())
    {
        fprintf(stderr, "Failed to initialize audio recorder\n");
        return ERROR;
    }

    audio_recorder.audio_out_filename = AUDIO_OUT_FILENAME;

    printf("Middleware initialized successfully\n");

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
            // printf("Received button state: %02X\n", button_state);
        }
    }
}

void process_record_fsm()
{
    static uint8_t hysteresis_btn_in = 0;
    static uint8_t hysteresis_btn_out = 0;
    static uint8_t hysteresis_chatbot_high = 0;
    static uint8_t hysteresis_chatbot_low = 0;
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
            audio_recorder.start_recording();
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

        /* Stop recording and saving an audio file */
        if (hysteresis_btn_out > HYST_RECORD_OUT_CNTR_THR || audio_recorder.poll_audio_record() == ERROR)
        {
            audio_recorder.stop_recording();
            hysteresis_btn_out = 0;
            hysteresis_chatbot_high = 0;
            record_fsm.value = 2;
            printf("Record button released\n");
        }

        break;
    }

    /* Trigger udp to high level */
    case 2:
    {
        printf("Reset audio & trigger chatbot\n");
        audio_recorder.reset_recording();

        udp_chatbot_msg[1] = 0x01;
        udp_to_chatbot.send((const char *)udp_chatbot_msg, sizeof(udp_chatbot_msg));

        if (++hysteresis_chatbot_high > HYST_CHATBOT_HIGH_TRIGGER)
        {
            hysteresis_chatbot_high = 0;
            hysteresis_chatbot_low = 0;
            record_fsm.value = 3;
        }
        break;
    }

    /* Reset trigger udp to high level */
    case 3:
    {
        printf("Reset trigger\n");
        udp_chatbot_msg[1] = 0x00;
        udp_to_chatbot.send((const char *)udp_chatbot_msg, sizeof(udp_chatbot_msg));

        if (++hysteresis_chatbot_low > HYST_CHATBOT_LOW_TRIGGER)
        {
            hysteresis_chatbot_low = 0;
            record_fsm.value = 4;
        }
        break;
    }

    /* Safety wait for release */
    case 4:
    {
        printf("Wait for release\n");
        if ((button_state & BTN_RECORD_AUDIO) == 0)
        {
            hysteresis_btn_out++;
        }

        if (hysteresis_btn_out > HYST_RECORD_OUT_CNTR_THR)
        {
            record_fsm.value = 0;
            printf("Record button released\n");
        }
        break;
    }
    }
}

/**
 * @file audio_interface.h
 *
 * @author ChatGPT zzz
 */

#ifndef AUDIO_INTERFACE_H
#define AUDIO_INTERFACE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/select.h>
#include <unistd.h>
#include "portaudio.h"

// Audio configuration definitions
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512
#define NUM_CHANNELS 2
#define SAMPLE_FORMAT paInt16 // 16-bit integer samples

typedef short SAMPLE;

#pragma pack(push, 1)
struct WAVHeader
{
    char riff[4];           // "RIFF"
    uint32_t chunkSize;     // File size - 8 bytes
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t subChunk1Size; // 16 for PCM
    uint16_t audioFormat;   // 1 for PCM
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;      // sampleRate * numChannels * bitsPerSample/8
    uint16_t blockAlign;    // numChannels * bitsPerSample/8
    uint16_t bitsPerSample; // Typically 16 bits
    char data[4];           // "data"
    uint32_t subChunk2Size; // NumSamples * numChannels * bitsPerSample/8
};
#pragma pack(pop)

class AudioRecorder
{
public:
    std::string audio_out_filename;

public:
    // Constructor: optionally specify the maximum number of frames to record.
    // (Default is 10 seconds worth of audio.)
    AudioRecorder(unsigned long maxFrames = SAMPLE_RATE * 10)
        : totalFrames(0), maxFrames(maxFrames), stream(nullptr) {}

    // Destructor: ensure the stream is stopped, closed, and PortAudio is terminated.
    ~AudioRecorder()
    {
        if (stream)
        {
            Pa_StopStream(stream);
            Pa_CloseStream(stream);
        }
        Pa_Terminate();
    }

    // Initialize PortAudio and open the input stream.
    bool init()
    {
        PaError err = Pa_Initialize();
        if (err != paNoError)
        {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << "\n";
            return false;
        }

        PaStreamParameters inputParameters;
        inputParameters.device = Pa_GetDefaultInputDevice();
        if (inputParameters.device == paNoDevice)
        {
            std::cerr << "Error: No default input device.\n";
            return false;
        }
        inputParameters.channelCount = NUM_CHANNELS;
        inputParameters.sampleFormat = SAMPLE_FORMAT;
        inputParameters.suggestedLatency =
            Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
        inputParameters.hostApiSpecificStreamInfo = nullptr;

        err = Pa_OpenStream(&stream,
                            &inputParameters, // input parameters
                            nullptr,          // no output parameters
                            SAMPLE_RATE,
                            FRAMES_PER_BUFFER,
                            paClipOff,
                            AudioRecorder::paCallback, // static callback function
                            this);                     // pass instance pointer as userData
        if (err != paNoError)
        {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << "\n";
            return false;
        }
        return true;
    }

    /**
     * @author Azzam Wildan (2025)
     */
    int start_recording()
    {
        PaError err = Pa_StartStream(stream);
        if (err != paNoError)
        {
            std::cerr << "Error starting stream: " << Pa_GetErrorText(err) << "\n";
            return -1;
        }

        return 0;
    }

    /**
     * @author Azzam Wildan (2025)
     */
    int poll_audio_record()
    {
        if (totalFrames >= maxFrames)
        {
            std::cout << "Maximum recording frames reached.\n";
            return -1;
        }
        return 0;
    }

    /**
     * @author Azzam Wildan (2025)
     */
    int stop_recording()
    {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError)
        {
            std::cerr << "Error stopping stream: " << Pa_GetErrorText(err) << "\n";
            return -1;
        }

        if (!saveToWavFile(audio_out_filename))
        {
            std::cerr << "Error saving audio file.\n";
            return -1;
        }

        return 0;
    }

    /**
     * @author Azzam Wildan (2025)
     */
    void reset_recording()
    {
        recordedSamples.clear();
        totalFrames = 0;
    }

private:
    // Static PortAudio callback wrapper; it calls the instance's recordCallback.
    static int paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData)
    {
        AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(userData);
        return recorder->recordCallback(inputBuffer, outputBuffer, framesPerBuffer,
                                        timeInfo, statusFlags);
    }

    // Non-static callback that is called by PortAudio to process incoming audio.
    int recordCallback(const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo *timeInfo,
                       PaStreamCallbackFlags statusFlags)
    {
        (void)outputBuffer;
        (void)timeInfo;
        (void)statusFlags;
        const SAMPLE *in = static_cast<const SAMPLE *>(inputBuffer);

        if (inputBuffer == nullptr)
        {
            // If no input is available, push zeros.
            for (unsigned long i = 0; i < framesPerBuffer * NUM_CHANNELS; ++i)
            {
                recordedSamples.push_back(0);
            }
        }
        else
        {
            // Append the incoming audio samples.
            for (unsigned long i = 0; i < framesPerBuffer * NUM_CHANNELS; ++i)
            {
                recordedSamples.push_back(in[i]);
            }
        }
        totalFrames += framesPerBuffer;
        return paContinue;
    }

    // Write the recorded samples to a raw PCM file.
    bool saveToFile(const std::string &filename)
    {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile)
        {
            std::cerr << "Failed to open file " << filename << " for writing.\n";
            return false;
        }
        outFile.write(reinterpret_cast<const char *>(recordedSamples.data()),
                      recordedSamples.size() * sizeof(SAMPLE));
        if (!outFile)
        {
            std::cerr << "Error writing file " << filename << ".\n";
            return false;
        }
        std::cout << "Recording complete. Recorded " << totalFrames << " frames.\n";
        std::cout << "Audio saved to " << filename << "\n";
        return true;
    }

    // Save the recorded PCM data to a WAV file by writing a WAV header.
    bool saveToWavFile(const std::string &filename)
    {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile)
        {
            std::cerr << "Failed to open file " << filename << " for writing.\n";
            return false;
        }

        WAVHeader header;
        // Prepare header fields.
        std::memcpy(header.riff, "RIFF", 4);
        std::memcpy(header.wave, "WAVE", 4);
        std::memcpy(header.fmt, "fmt ", 4);
        std::memcpy(header.data, "data", 4);

        header.subChunk1Size = 16; // PCM
        header.audioFormat = 1;    // Linear PCM
        header.numChannels = NUM_CHANNELS;
        header.sampleRate = SAMPLE_RATE;
        header.bitsPerSample = 16; // for paInt16
        header.byteRate = SAMPLE_RATE * NUM_CHANNELS * header.bitsPerSample / 8;
        header.blockAlign = NUM_CHANNELS * header.bitsPerSample / 8;
        header.subChunk2Size = recordedSamples.size() * sizeof(SAMPLE);
        header.chunkSize = 4 + (8 + header.subChunk1Size) + (8 + header.subChunk2Size);

        // Write the WAV header.
        outFile.write(reinterpret_cast<const char *>(&header), sizeof(WAVHeader));
        // Write the recorded PCM data.
        outFile.write(reinterpret_cast<const char *>(recordedSamples.data()),
                      recordedSamples.size() * sizeof(SAMPLE));
        outFile.close();

        if (!outFile)
        {
            std::cerr << "Error writing file " << filename << ".\n";
            return false;
        }
        std::cout << "Recording complete. Recorded " << totalFrames << " frames.\n";
        std::cout << "Audio saved to " << filename << "\n";
        return true;
    }

    // Member variables to store recorded samples, the total number of frames,
    // the maximum number of frames to record, and the PortAudio stream.
    std::vector<SAMPLE> recordedSamples;
    unsigned long totalFrames;
    unsigned long maxFrames;
    PaStream *stream;
};

#endif // AUDIO_INTERFACE_H
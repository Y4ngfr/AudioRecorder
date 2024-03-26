#include <stdio.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <time.h>
#include <sndfile.h>
#include <getopt.h>
#include "WAVRecorder.h"
#include "WAVAudioWriter.h"
#include "InputReader.h"

void recordingSystem(InputData inputData);
int threadFunction(void* argData);

int main(int argc, char* argv[])
{
    Uint8 inputReadingReturn;
    InputData inputData;

    inputReadingReturn = readInput(argc, argv, &inputData);

    switch(inputReadingReturn)
    {
        case EXIT_SUCCESS:
            recordingSystem(inputData);
            break;

        case EXIT_FAILURE:
            exit(1);
            break;

        case EXIT_ALTERNATIVE:
            exit(0);
            break;
    }

    return 0;
}

void recordingSystem(InputData inputData)
{
    AudioDevice *recordingDevice;
    Uint32 byteRate, allocationSize;
    Uint8* audioBuffer;

    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("Erro ao iniciar SDL. Erro: %s\n", SDL_GetError());
        exit(1);
    }

    if(openRecordingDevice(&recordingDevice, inputData.deviceIndex) < 0)
    {
        SDL_Quit();
        exit(1);
    }

    byteRate = getBytesPerSecond(recordingDevice->obtainedSpec);
    allocationSize = byteRate * ((Uint32)inputData.recordingTime + 1);
    audioBuffer = (Uint8*)SDL_malloc(allocationSize * sizeof(Uint8));

    if(audioBuffer == NULL)
    {
        printf("Erro ao alocar buffer de gravação\n");
        closeDevice(recordingDevice);
        SDL_Quit();
        exit(1);
    }

    if(inputData.repetitions == 0)
    {
        int endRecording = 0;
        SDL_Thread *thread = SDL_CreateThread(threadFunction, "thread", &endRecording);

        printf("Gravando e salvando (Pressione Enter para saír)...\n");

        while(1)
        {
            Uint32 bufferLength;

            bufferLength = byteRate * inputData.recordingTime;

            record(recordingDevice, audioBuffer, bufferLength);

            if(saveRecordingAudio(inputData.directory, recordingDevice,
            bufferLength) < 0)
            {
                free(audioBuffer);
                closeDevice(recordingDevice);
                SDL_Quit();
                exit(1);
            }                    
    
            if(endRecording) break;
        }

        SDL_WaitThread(thread, NULL);
    }

    while(inputData.repetitions > 0)
    {
        Uint32 bufferLength;

        bufferLength = byteRate * inputData.recordingTime;

        printf("Gravando...\n");
        record(recordingDevice, audioBuffer, bufferLength);

        printf("Salvando...\n");
        if(saveRecordingAudio(inputData.directory, recordingDevice,
        bufferLength) < 0)
        {
            free(audioBuffer);
            closeDevice(recordingDevice);
            SDL_Quit();
            exit(1);
        }

        inputData.repetitions--;
    }

    free(audioBuffer);
    closeDevice(recordingDevice);
    SDL_Quit();
}

int threadFunction(void* argData)
{
    while(getchar() != '\n')
    {
        SDL_Delay(10);
    }

    *((int*)argData) = 1;

    return 0;
}
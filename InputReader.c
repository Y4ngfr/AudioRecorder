#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <getopt.h>
#include "InputReader.h"

int readInput(int argc, char* argv[], InputData* inputData)
{
    int option, opt_flag, error = 0;
    Uint8 opt_o = 0, opt_r = 0, opt_t = 0, opt_i = 0;
    FILE *fp;

    Option long_options[] = {
        {"help", no_argument, &opt_flag, 0},
        {"devices", no_argument, &opt_flag, 1}
    };

    if(argc == 1)
    {
        printHelp();
        return EXIT_ALTERNATIVE;
    }

    do{
        option = getopt_long(argc, argv, "i:t:o:r:", long_options, NULL);

        switch(option)
        {
            case 0:
                if(opt_flag == 1) listDevices();

                if(opt_flag == 0) printHelp();

                return EXIT_ALTERNATIVE;

            case 't':
            {
                char *auxarg;
                opt_t = 1;

                auxarg = (char*)malloc(strlen(optarg)*sizeof(char));

                if(auxarg == NULL){
                    printf("Erro na opção -t, não foi possível obter valor do parâmetro\n");
                    error = 1;
                    break;
                }

                sprintf(auxarg, "%.2f", strtod(optarg, NULL));
                inputData->recordingTime = strtod(auxarg, NULL);

                free(auxarg);

                if(inputData->recordingTime == 0){
                    printf("Erro na opção -t: argumento inválido `%s`\n", optarg);
                    error = 1;
                }
            }   
            break;

            case 'r':
            {
                int isZero;
                opt_r = 1;

                inputData->repetitions = atoi(optarg);

                isZero = !strcmp(optarg, "0");

                if(inputData->repetitions == 0 && !isZero){
                    printf("Erro na opção -r: argumento inválido `%s`\n", optarg);
                    error = 1;
                }
            }
            break;
                
            case 'i':
            {
                int isZero;
                opt_i = 1;

                inputData->deviceIndex = atoi(optarg);

                isZero = !strcmp(optarg, "0");

                if(inputData->deviceIndex == 0 && !isZero){
                    printf("Erro na opção -i: argumento inválido `%s`\n", optarg);
                    error = 1;
                }
            }
            break;

            case 'o':
                opt_o = 1;
                inputData->directory = optarg;
                break;

            case '?':
                if(isprint(optopt)){
                    printf("Opção `-%c` não reconhecida\n", optopt);
                }
                else{
                    printf("Caractere `\\x%x` não reconhecido\n", optopt);
                }

                error = 1;

                break;
                
            case -1:
                break;
        }

    }while(option != -1);

    if(opt_o == 0){
        printf("Erro: opção -o ausente\n");
        error = 1;
    }

    if(opt_r == 0){
        printf("Erro: opção -r ausente\n");
        error = 1;
    }

    if(opt_i == 0){
        printf("Erro: opção -i ausente\n");
        error = 1;
    }

    if(opt_t == 0){
        printf("Erro: opção -t ausente\n");
        error = 1;
    }

    if(error)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void printHelp()
{
    printf("Uso: gravador [options] file...\n");
    printf("Options:\n");
    printf("--help                       Mostra informações de ajuda\n");
    printf("--devices                    Lista os dispositivos de gravação\n");
    printf("-t <arg>                     Define o tempo de gravação em segundos\n");
    printf("-o <directory>               Define o diretório de saída\n");
    printf("-r <arg>                     Define quantas vezes a gravação se repete (definir como 0 para repetir indefinidamente)\n");
    printf("-i <arg>                     Define o índice do dispositivo de gravação\n");
    printf("Exemplo: gravador -i 0 -o meu/diretorio/ -t 5 -r 10\n");
    printf("Grava 10 áudios de 5 segundos com o dispositivo de gravação 0 e coloca em meu/diretorio\n");
}

void listDevices()
{
    int numberOfRecordingDevices, i;

    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("%s\n", SDL_GetError());
        return;
    }

    numberOfRecordingDevices = SDL_GetNumAudioDevices(SDL_TRUE);

    if(numberOfRecordingDevices < 1)
    {
        printf("Falha ao encontrar dispositivos de gravação de áudio\n");
        return;
    }

    printf("Dispositivos de gravação de áudio:\n");

    for(i=0; i < numberOfRecordingDevices; i++)
    {
        const char* deviceName;

        deviceName = SDL_GetAudioDeviceName(i, SDL_TRUE);

        printf("index: %d; Device: %s\n", i, deviceName);
    }   

    SDL_Quit();
}
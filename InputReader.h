#ifndef _INPUT_READER_H
#define _INPUT_READER_H

#define EXIT_ALTERNATIVE 2

/**
 * Define a estrutura `struct option` da biblioteca getopt como apenas Option
*/
typedef struct option Option;

/**
 * Todos os valores dessa estrutura são definidos pela função readInput() 
*/
typedef struct InputData{

    int deviceIndex; /** Índice do dispositivo de gravação */
    int repetitions; /** Número de repetições da gravação */
    double recordingTime; /** Tempo de duração da gravação em segundos */
    char* directory; /** Diretório de saída dos arquivos de gravação */

} InputData;

/**
 *  Lê e trata a entrada da linha de comando.
 *  \param argc Número de parâmetros de linha de comando (mesmo da função main)
 *  \param argv Vetor que contém os argumentos de linha de comando (mesmo da função main)  
 *  \param inputData Estrutura onde os valores de entrada serão definidos
 *  \returns Retorna EXIT_SUCCESS caso não ocorra nenhum erro e tenham sido enviados
 *           apenas opções curtas na linha de comando, EXIT_FAILURE caso tenha ocorrido algum erro e
 *           EXIT_ALTERNATIVE caso --help ou --devices tenha sido enviado na linha de comando
 * */
int readInput(int argc, char* argv[], InputData* inputData);

/**
 * Imprime uma lista dos dispositivos de gravação detectados precedidos pelo seu indice
*/
void listDevices();

/**
 * Imprime as informações de ajuda
*/
void printHelp();

#endif
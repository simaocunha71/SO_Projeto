#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../config.c"
#include "../task.c"

#define BUFFERSIZE 1024

/**
 * @brief Inicializa o servidor com o carregamento das configurações do ficheiro de configuração
 * @param config_filename Ficheiro de configuração
 * @param binarys_foldername Pasta com os binários a executar
 * @return Configuração com todos os binários a executar na forma de lista ligada
 */
CONFIG start_server(char* config_filename, char* binarys_foldername);

//DEBUG
void printConfigs(CONFIG c);

/**
 * @brief Executa vários binários com recurso a pipes anónimos, guardando o resultado no ficheiro de output
 * @param c Lista ligada de configurações
 * @param input Ficheiro de input
 * @param output Ficheiro de output
 * @param binaries_array Array de binários a executar
 * @param number_of_commands Número de binários a executar
 * @return int 0 se sucesso, -1 caso contrário
 */
int execute_commands_in_pipeline(CONFIG c, char* input, char* output, char** binaries_array, int number_of_commands);

void statusfunction();

int sendtoclient(int nr);

void create_status_message(int fileDescriptor, CONFIG c, Queue q, int vetor_instances_original[]);
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
 * @brief Inicializa o servidor, criando a lista ligada CONFIG
 * @param config_filename Ficheiro de configuração
 * @param binarys_folder Pasta onde estão os ficheiros objeto dos binários a executar
 * @return CONFIG Lista ligada CONFIG
 */
CONFIG start_server(char* config_filename, char* binarys_folder);

/**
 * @brief Executa uma pipeline de binários a executar com recurso a pipes anónimos
 * @param c Lista ligada CONFIG
 * @param input Ficheiro de input
 * @param output Ficheiro de output
 * @param binaries_array Array de binários a executar
 * @param number_of_commands Tamanho do array de binários a executar
 * @return int 0 se sucesso
 */
int execute_commands_in_pipeline(CONFIG c, char* input, char* output, char** binaries_array, int number_of_commands);

/**
 * @brief Função principal encarregue de criar a mensagem do comando status
 * @param fileDescriptor Cliente que vai receber a resposta
 * @param c Lista ligada de configurações
 * @param q Queue de tasks
 * @param vetor_instances_original Array que contém as instâncias originais na mesma ordem que ocorrem na lista ligada original 
 */
void create_status_message(int fileDescriptor, CONFIG c, Queue q, int vetor_instances_original[]);

/**
 * @brief Fecha o servidor
 * @param signum Sinal
 */
void close_server(int signum);
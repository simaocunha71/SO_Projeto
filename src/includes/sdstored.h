#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../config.c"

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
 * @brief Executa os comandos dados pelo cliente
 * 
 * @param nc numero de comandos
 * @param cmd lista dos comandos
 * @param path local para executar os comandos
 * @param input file input
 * @param output file output
 * @return 0 if everything goes right
 */
int execcommands(int nc,char **cmd, char* path, char* input, char* output);
void statusfunction();
int sendtoclient(int nr);
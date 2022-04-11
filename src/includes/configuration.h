#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

#define MAX_CONFIGS 6

/**
 * @brief Struct que constitui um binário e o número máximo de instâncias que podem ocorrer (a usar os valores do enunciado)
 */
typedef struct configuration{
    char* binary_name;
    int max_instances;
}CONFIG;

/**
 * @brief Struct que constitui todos os binários que temos ao dispor
 */
typedef struct configurations{
    CONFIG config[MAX_CONFIGS];
}*CONFIGURATIONS;

/**
 * @brief Cria uma configuração
 * @param line Linha do ficheiro de configuração
 * @return CONFIG Struct config criada
 */
CONFIG create_config (char* line);

/**
 * @brief Carrega todas as structs das configurações para a struct principal
 * @param config_filename Nome do ficheiro de configuração
 * @return CONFIGURATIONS Struct configurations creada
 */
CONFIGURATIONS load_configurations (char* config_filename);

/**
 * @brief Função que recebe o binário a executar e executa-o num ficheiro de input, guardando o resultado num outro ficheiro
 * @param binary_to_execute Binário a executar (ficheiros objetos que estão na pasta obj) da struct CONFIG
 * @param file_to_use Ficheiro a usar com o binário
 * @param new_file Nome do novo ficheiro
 */
void execute_config (CONFIG c, const char* file_to_use, const char* new_file);

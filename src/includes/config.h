#include "../utilities.c"

/**
 * @brief Lista ligada que constitui um binário e o número máximo de instâncias que podem ocorrer (a usar os valores do enunciado)
 */
typedef struct configuration{
    char* binary_name;
    char* path_name;
    int max_instances;
    struct configuration *next;
}*CONFIG;

/**
 * @brief Cria uma configuração
 * @param line Linha do ficheiro de configuração
 * @param binarys_folder Pasta com os binários a executar
 * @param c Lista das configurações
 * @return CONFIG Lista com a configuração criada
 */
CONFIG create_config (char* line, char* binarys_folder, CONFIG c);

/**
 * @brief Carrega todas as structs das configurações para a struct principal
 * @param config_filename Nome do ficheiro de configuração
 * @param binarys_folder Pasta que contém todos os binários a executar
 * @return CONFIGURATIONS Struct configurations criada
 */
CONFIG load_configurations (char* config_filename, char* binarys_folder);

/**
 * @brief Função que recebe o binário a executar e executa-o num ficheiro de input, guardando o resultado num outro ficheiro
 * @param cs Lista ligada dos binários a executar
 * @param binary_name Nome do binário a executar
 * @param file_to_use Ficheiro a usar com o binário
 * @param new_file Nome do novo ficheiro
 */
void execute_config (CONFIG cs, char* binary_name, char* file_to_use, char* new_file);

/**
 * @brief Devolve o path para o respetivo binário
 * @param binary_name Nome do binário
 * @param binary_folder Pasta onde se encontra todos os binários (ficheiros objeto)
 * @return char* Path do binário a executar
 */
char* get_binary_filename (char* binary_name, char* binary_folder);

/**
 * @brief Devolve a configuração com um determinado nome
 * @param binary_name Nome do binário que procuramos
 * @param cs Lista de configurações
 * @return CONFIG Configuração pretendida
 */
CONFIG get_Config(char* binary_name, CONFIG cs);
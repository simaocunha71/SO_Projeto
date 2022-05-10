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
 * @brief Função que recebe o binário a executar e executa-o
 * @param cs Lista ligada dos binários a executar
 * @param binary_name Nome do binário a executar
 */
void execute_config (CONFIG cs, char* binary_name);

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

/**
 * @brief Incrementa ou decrementa o nº de instancias de um binario, consoante queremos libertá-lo ou decrementá-lo
 * @param c Configuração de binários
 * @param binary Nome do binário
 * @param operation "dec" se queremos decrementar; "inc" se queremos incrementar
 */
void changeInstances(CONFIG c, char* binary, char* operation);

/**
 * @brief Dado um array de binários e a lista ligada de configurações, verifica se pode executar todos os binários do array
 * @param c Lista ligada de binários
 * @param binaries_array Array de binários a executar
 * @return int 1 se sucesso, 0 caso contrário
 */
int canExecuteBinaries(CONFIG c, char** binaries_array,int number_of_commands);

int match_binary_with_instances(CONFIG cs, char* binary_name);

/**
 * @brief Altera o numero das instancias (decrementa) de cada binario quando um pedido que os use começa a ser efetuado
 */
void request_enter(CONFIG cs, char** binaries_array, int number_of_binaries);

/**
 * @brief Altera o numero das instancias (incrementa) de cada binario quando um pedido que os use acaba de ser efetuado
 */
void request_out(CONFIG cs, char** binaries_array, int number_of_binaries);

void print_num(CONFIG c);

int size_array(CONFIG c);

int get_original_inst(int indice, int vetor_instances_original[]);

char* get_status_from_config(CONFIG c, int vetor_instances_original[]);
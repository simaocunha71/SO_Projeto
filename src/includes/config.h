#include "../utilities.c"

typedef struct configuration{
    char* binary_name;
    char* path_name;
    int max_instances;
    struct configuration *next;
}*CONFIG;

/**
 * @brief Cria uma configuração sob a forma de lista ligada, efetuando parse de uma linha do ficheiro de configuração
 * @param line Linha do ficheiro de configuração
 * @param binarys_folder Pasta onde estão os ficheiros objeto dos binários a executar
 * @param c Lista ligada
 * @return CONFIG Lista ligada resultante
 */
CONFIG create_config (char* line, char* binarys_folder, CONFIG c);

/**
 * @brief Lê o ficheiro de configuração, criando a lista ligada de configurações
 * @param config_filename Ficheiro de configuração
 * @param binarys_folder Pasta onde estão os ficheiros objeto dos binários a executar
 * @return CONFIG Lista ligada resultante
 */
CONFIG load_configurations (char* config_filename, char* binarys_folder);

/**
 * @brief Devolve o path para o ficheiro objeto de um dado binário
 * @param binary_name Nome do binário
 * @param binary_folder Pasta onde estão os ficheiros objeto dos binários a executar
 * @return char* Path para o ficheiro objeto de um dado binário
 */
char* get_binary_filename (char* binary_name, char* binary_folder);

/**
 * @brief Executa um dado binário
 * @param cs Configuração que contém todos os binários
 * @param binary_name Nome do binário a executar
 */
void execute_config (CONFIG cs, char* binary_name);

/**
 * @brief Devolve uma cópia de um nó de uma configuração
 * @param binary_name Nome do binário
 * @param cs Lista ligada de configurações
 * @return CONFIG Nó pretendido
 */
CONFIG get_Config(char* binary_name, CONFIG cs);

/**
 * @brief Altera as instâncias de um dado binário, consoante a operação dada
 * @param c Lista ligada de configurações
 * @param binary Nome do binário
 * @param operation "dec": decrementa; "inc": incrementa
 */
void changeInstances(CONFIG c, char* binary, char* operation);

/**
 * @brief Verifica se todos os binários contidos no array de binários podem ser executados
 * @param c Lista ligada de configurações
 * @param binaries_array Array de binários
 * @param number_of_commands Tamanho do array de binários
 * @return int 1 se verdade, 0 caso contrário
 */
int canExecuteBinaries(CONFIG c, char** binaries_array, int number_of_commands);

/**
 * @brief Decrementa instâncias dos binários contidos no array (task entrará na queue)
 * @param cs Lista ligada de configurações
 * @param binaries_array Array de binários
 * @param number_of_binaries Tamanho do array de binários
 */
void request_enter(CONFIG cs, char** binaries_array, int number_of_binaries);

/**
 * @brief Incrementa instâncias dos binários contidos no array (task sairá da queue)
 * @param cs Lista ligada de configurações
 * @param binaries_array Array de binários
 * @param number_of_binaries Tamanho do array de binários
 */
void request_out(CONFIG cs, char** binaries_array, int number_of_binaries);

/**
 * @brief Calcula tamanho da lista ligada de configurações
 * @param c Lista ligada de configurações
 * @return int Tamanho da lista ligada
 */
int size_array(CONFIG c);

/**
 * @brief Devolve a instância máxima original de um binário
 * @param indice Indice do array
 * @param vetor_instances_original Array que contém as instâncias originais na mesma ordem que ocorrem na lista ligada original 
 * @return int Instância máxima original
 */
int get_original_inst(int indice, int vetor_instances_original[]);

/**
 * @brief Envia para o cliente a parte da mensagem do comando status referente aos dados da CONFIG
 * @param fileDescriptor Cliente para onde se vai enviar a resposta
 * @param c Lista ligada de configurações
 * @param vetor_instances_original Array que contém as instâncias originais na mesma ordem que ocorrem na lista ligada original 
 */
void get_status_from_config(int fileDescriptor, CONFIG c, int vetor_instances_original[]);

/**
 * @brief Copia toda a lista ligada para outra
 * @param c Lista ligada original
 * @return CONFIG Lista ligada cópia
 */
CONFIG copy_config (CONFIG c);

/**
 * @brief Liberta o espaço ocupado pela lista ligada CONFIG
 * @param c Lista ligada de configurações
 */
void free_config (CONFIG c);
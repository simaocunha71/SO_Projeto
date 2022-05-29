#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

typedef struct task {
    int id;
    char* file_input;
    char* file_output;
    char** binaries_to_execute;
    int binaries_num;
    struct task* prox;
} *TASK;

typedef struct queue {
    struct task* inicio , *fim ;
} *Queue;


/**
 * @brief Inicializa a queue
 * @return Queue Queue inicializada
 */
Queue init_queue();

/**
 * @brief Verifica se a queue está vazia.
 * @param q Queue
 * @return int 1 se verdade, 0 caso contrário
 */
int isEmpty (Queue q);

/**
 * @brief Adiciona uma task no fim da queue
 * @param q Queue de tasks
 * @param file_input Ficheiro de input da task
 * @param file_output Ficheiro de output da task
 * @param binaries_to_execute Array de strings da task
 * @param number_of_binaries Tamanho do array de strings da task
 * @param client_pid PID do cliente (é o ID da task)
 */
void add_task (Queue q, char* file_input, char* file_output, char** binaries_to_execute, int number_of_binaries, int client_pid);

/**
 * @brief Remove a primeira task da queue
 * @param q Queue com a task por retirar
 */
void remove_task (Queue q);

/**
 * @brief Liberta a memoria utilizada por um array de strings
 * @param arrayStrings Array de strings a libertar
 * @param size Tamanho do array de strings
 */
void freeArrayList(char** arrayStrings, int size);

/**
 * @brief Copia um array de strings para outro array de strings
 * @param original Array original
 * @param array_size Tamanho do array original
 * @return char** Array cópia
 */
char** arrayStrings_Copy (char** original, int array_size);

/**
 * @brief Envia para o cliente a parte da mensagem do comando status referente aos dados da queue
 * @param fileDescriptor Cliente para onde se vai enviar a resposta
 * @param q Queue
 */
void get_status_from_queue(int fileDescriptor, Queue q);
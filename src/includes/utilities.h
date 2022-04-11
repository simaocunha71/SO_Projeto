#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../utilities.c"

/**
 * @brief Função que lê uma linha e carrega para o buffer line
 * @param fd File descriptor do ficheiro
 * @param line Buffer
 * @param size Tamanho a ler
 */
void readln(int fd, char* line, size_t size);

/**
 * @brief Valida o binário a executar
 * @param binary_to_execute Nome do binário a executar
 * @return int 1 se válido, 0 caso contrário
 */
int validate_binary_to_execute (char* binary_to_execute);

/**
 * @brief Redireciona o std para o filedescriptor de um ficheiro
 * @param filename Nome do ficheiro
 * @param std STDIN, STDOUT ou STDERR
 * @param flag Diz se é ficheiro de input (0) ou de output (> 0)
 */
void redirection (char* filename, int std, int isInput);

#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 * @brief Função que lê uma linha e carrega para o buffer line
 * @param fd File descriptor do ficheiro
 * @param line Buffer
 * @param size Tamanho a ler
 * @return Nº de bytes lidos
 */
size_t readln(int fd, char* line, size_t size);

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

/**
 * @brief Função que converte um numero em string, guardando o resultado em 'string'.
 * Adaptada de https://stackoverflow.com/questions/11819837/converting-integer-to-string-in-c-without-sprintf, do utilizador ExternCowWindows
 * @param n Numero a converter
 * @return String com o numero convertido
 */
char* inttoString(int n);

/**
 * @brief Adiciona uma string a um array dinâmico de strings
 * @param array_of_strings Array dinâmico de strings
 * @param string_to_add String a adicionar
 * @return char** String atualizada
 */
char** add_string_to_array(char** array_of_strings, const char* string_to_add);

/**
 * @brief Conta o número de binários numa string (argumento)
 * @param buffer String a contar
 * @return int Número de binários de argumento
 */
int get_binaries_num(char* buffer);

/**
 * @brief Cria um array de strings com o nome dos binários a utilizar
 * @param buffer Argumento que contem os binários
 * @return char** Array de strings que contém os binários
 */
char** create_binaries_array(char* buffer);
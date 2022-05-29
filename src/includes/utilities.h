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
size_t readln(int fd, char *line, size_t size);

/**
 * @brief Valida o binário a executar
 * @param binary_to_execute Nome do binário a executar
 * @return int 1 se válido, 0 caso contrário
 */
int validate_binary_to_execute (char* binary_to_execute);


/**
 * @brief Converte um inteiro numa string
 * @param n Número a converter
 * @return char* String resultante
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
 * @param number_of_binaries Numero de binarios a criar
 * @return char** Array de strings que contém os binários
 */
char** create_binaries_array(char* buff, int number_of_binaries);

/**
 * @brief Realoca memoria da string a de forma a conter a + b
 * @param a String a
 * @param b String b
 */
void my_strcat(char* a, char* b);
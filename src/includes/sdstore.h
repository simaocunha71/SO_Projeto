#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "utilities.h"
#include "../config.c"

/**
 * @brief Envia a string "status" para o servidor
 * @param str_status String "status"
 * @return int 0 se sucesso, -1 caso contrário
 */
int execute_status(char* str_status);


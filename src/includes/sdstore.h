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
 * @brief Executa vários binários a um ficheiro de input através de pipes anónimos, guardando o resultado num ficheiro de output
 * @param arguments argv
 * @param num_args argc
 */
void execute_bins(const char* arguments[], int num_args);
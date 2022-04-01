#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

/**
 * @brief Valida o binário a executar
 * @param binary_to_execute Nome do binário a executar
 * @return int 1 se válido, 0 caso contrário
 */
int validate_binary_to_execute (char* binary_to_execute){
    return strcmp(binary_to_execute,"obj/bcompress")   ||
           strcmp(binary_to_execute,"obj/bdecompress") ||
           strcmp(binary_to_execute,"obj/decrypt")     ||
           strcmp(binary_to_execute,"obj/encrypt")     ||
           strcmp(binary_to_execute,"obj/gcompress")   ||
           strcmp(binary_to_execute,"obj/gdecompress") ||
           strcmp(binary_to_execute,"obj/nop");
}

/**
 * @brief Redireciona o std para o filedescriptor de um ficheiro
 * @param filename Nome do ficheiro
 * @param std STDIN, STDOUT ou STDERR
 * @param flag Diz se é ficheiro de input (0) ou de output (> 0)
 */
void redirection (char* filename, int std, int isInput){
    int file;
    if(isInput == 0)
        file = open(filename, O_RDONLY, 0640);
    else
        file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    
    dup2(file, std);
    close(file);
}

/**
 * @brief Função que recebe o binário a executar e executa-o num ficheiro de input, guardando o resultado num outro ficheiro
 * @param binary_to_execute Binário a executar (ficheiros objetos que estão na pasta obj)
 * @param file_to_use Ficheiro a usar com o binário
 * @param new_file Nome do novo ficheiro
 */
void execute_command(const char* binary_to_execute, const char* file_to_use, const char* new_file){

    if(validate_binary_to_execute((char*)binary_to_execute)){
        redirection((char*)file_to_use,0, 0);
        redirection((char*)new_file,1, 1);

        char* command_files[] = {(char*)binary_to_execute, (char*)file_to_use, (char*)new_file, NULL};
    
        int e = execvp(command_files[0], command_files);

        if(e == -1)
            perror("Error");
        else 
            printf("Success\n");
    }
    else
        perror("Error\n");
}

// Cliente

int main(int argc, char const *argv[]){

    execute_command(argv[1], argv[2], argv[3]); //./sdstore binario_a_executar ficheiro_a_utilizar ficheiro_novo
    return 0;
}

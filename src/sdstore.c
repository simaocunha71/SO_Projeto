#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

/**
 * @brief Redireciona o std para o filedescriptor de um ficheiro
 * @param filename Nome do ficheiro
 * @param std STDIN, STDOUT ou STDERR
 */
void redirection (const char* filename, int std){
    int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    dup2(std,file);
    close(file);
}

/**
 * @brief Função que coloca no nome dos ficheiros de input e output a string "../"
 * @param file Ficheiro a manusear
 * @return char* Nome do ficheiro atualizado
 */
char* update_name_file(const char* file){
    char* toOpen = malloc(sizeof(char) * (strlen(file)+3));
    strcpy(toOpen,"../");
    strcat(toOpen,file);
    return toOpen;
}


/**
 * @brief Função que recebe o binário a executar e executa-o num ficheiro de input, guardando o resultado num outro ficheiro
 * [TODO: AINDA NAO FUNCIONAL]
 * @param binary_to_execute Binário a executar (ficheiros objetos que estão na pasta obj)
 * @param file_to_use Ficheiro a usar com o binário
 * @param new_file Nome do novo ficheiro
 */
void execute_command(const char* binary_to_execute, const char* file_to_use, const char* new_file){
    int e;
    
    char* toOpen = update_name_file(file_to_use);
    char* toWrite = update_name_file(new_file);
    
    redirection(toWrite,1);
    char* command_files[] = {"obj/bcompress", toOpen, toWrite, NULL};
    
    if (strcmp (binary_to_execute, "obj/bcompress") == 0){
        e = execvp(command_files[0], command_files);

        if(e == -1){
            perror("Error bcompress");
        }
        else printf("Executei bcompress\n");
    }
    }/*
    else if (strcmp (binary_to_execute, "/obj/bdecompress") == 0){
        e = execl("/obj/bdecompress","bdecompress",file_to_use, new_file, NULL);
        if(e == -1){
            perror("Error bdecompress");
        }
        else printf("Executei bdecompress\n");
    }
    else if (strcmp (binary_to_execute, "obj/decrypt") == 0){
        e = execl("/obj/decrypt","decrypt",file_to_use, new_file, NULL);
        if(e == -1){
            perror("Error decrypt");
        }
        else printf("Executei decrypt\n");
    }
    else if (strcmp (binary_to_execute, "obj/encrypt") == 0){
        e = execl("/obj/encrypt","encrypt",file_to_use, new_file, NULL);
        if(e == -1){
            perror("Error encrypt");
        }
        else printf("Executei encrypt\n");
    }
    else if (strcmp (binary_to_execute, "obj/gcompress") == 0){
        e = execl("/obj/gcompress","gcompress",file_to_use, new_file, NULL);
        if(e == -1){
            perror("Error gcompress");
        }
        else printf("Executei gcompress\n");
    }
    else if (strcmp (binary_to_execute, "obj/gdecompress") == 0){
        e = execl("/obj/gdecompress","gdecompress",file_to_use, new_file, NULL);
        if(e == -1){
            perror("Error gdecompress");
        }
        else printf("Executei gdecompress\n");
    }
    else if (strcmp (binary_to_execute, "obj/nop") == 0){
        e = execl("/obj/nop","nop",file_to_use, new_file, NULL);
        if(e == -1){
            perror("Error nop");
        }
        else printf("Executei nop\n");
    }
    else{
        printf("Command not found! Try again!\n");
    }
}
*/

// Cliente

int main(int argc, char const *argv[]){
    //Verifica se está a executar tudo corretamente
    execute_command(argv[1], argv[2], argv[3]); //./sdstore binario_a_executar ficheiro_a_utilizar ficheiro_novo
    return 0;
}

#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Função que recebe o binário a executar e executa-o num ficheiro de input, guardando o resultado num outro ficheiro
 * [TODO: Ver porque não reconhece os paths dos ficheiros de input]
 * ------------------------------------------------------------------
 * simao@simao:~/Desktop/SO_Projeto$ ./sdstore obj/bcompress test_files/file.csv 
   Error bcompress: No such file or directory
   ------------------------------------------------------------------
 * @param binary_to_execute Binário a executar (ficheiros objetos que estão na pasta obj)
 * @param file_to_use Ficheiro a usar com o binário
 * @param new_file Nome do novo ficheiro
 */
void execute_command(const char* binary_to_execute, const char* file_to_use, const char* new_file){
    int e;
    if (strcmp (binary_to_execute, "obj/bcompress") == 0){
        e = execl("/obj/bcompress","bcompress", file_to_use, new_file, NULL);
        if(e == -1){
            perror("Error bcompress");
        }
        else printf("Executei bcompress\n");
    }
    else if (strcmp (binary_to_execute, "obj/bdecompress") == 0){
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


// Cliente

int main(int argc, char const *argv[])
{
    //Verifica se está a executar tudo corretamente
    execute_command(argv[1], argv[2], argv[3]); //./sdstore binario_a_executar ficheiro_a_utilizar ficheiro_novo
    return 0;
}

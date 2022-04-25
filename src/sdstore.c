#include "includes/sdstore.h"

// Cliente
void execute_bins(const char* arguments[], int num_args){
    
    int pipe_bn[2];
    char* input = (char*) arguments[2];
    char* output = (char*) arguments[3];

    CONFIG c = load_configurations("bin/sdstore.conf", "obj/"); //isto provavelmente vai para o servidor e vai ser passado por argumento nesta função

    for (int i = 4; i < num_args; i++){

        if ((strcmp(arguments[i],"nop")) != 0) {

            if(pipe(pipe_bn) < 0)
                perror("Erro a fazer pipe");

            if(fork()==0){
                close(pipe_bn[0]);
                dup2(pipe_bn[1],1);
                close(pipe_bn[1]);
                execute_config(c,(char*) arguments[i],input,output);    
                _exit(0);

            }
            else {
                wait(NULL);
                close(pipe_bn[1]);
                dup2(pipe_bn[0], 0);
                close(pipe_bn[0]);
            }
        }
    }
    
}

//$ ./sdstore proc-file FILEINPUT FILEOUTPUT bcompress nop gcompress encrypt nop

int main(int argc, char const *argv[]){

    if (argc < 5) {
        printf ("Nº de argumentos inválido\n");
        return -1;
    }

/*
    printf("File input: %s\n",file_input);
    printf("File output: %s\n", file_output);

    for (i = 0; i < argc-4; i++)
    {
        printf("Bin nº %d: %s\n",i+1,binaries_to_execute[i]);
    }
    
*/
    if ((strcmp(argv[1], "proc-file") == 0))
        execute_bins(argv, argc);
    else
        printf("DEBUG: Provavelmente falta a flag \"proc-file\"\n");

    return 0;
}

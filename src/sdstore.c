#include "includes/sdstore.h"
#include "includes/utilities.h"
#include "includes/config.h"

// Cliente

void execute_bins(char* input, char* output, char* bins, int size){
    
    int pipe_bn[2];
    char* ipt = (char*) input;

    CONFIG c = load_configurations("bin/sdstore.conf", "obj/");

    for (int i = 0; i < size; i++){

        if ((strcmp(bins[i],"nop")) != 0) {

            if(pipe(pipe_bn)<0){
                 perror("PIPE ERROR!!!!!\n");
                return -1;
            }


            if(fork()==0){
                close(pipe_bn[0]);
                dup2(pipe_bn[1],1);
                close(pipe_bn[1]);
                execute_config(c,bins[i],ipt,output);   //???       
                _exit(0);

                //FALTA: ESCREVER O RESULTADO NO FICHEIRO OUTPUT
            }
            else {
                wait(NULL);
                close(pipe_bn[1]);
            }

            ipt = output;               //?????????
        }
    }
    
}

//$ ./sdstore proc-file FILEINPUT FILEOUTPUT bcompress nop gcompress encrypt nop

int main(int argc, char const *argv[]){

    if (argc < 5) {
        printf ("TOO FEW ARGUMENTS!\n");
        return -1;
    }

    int i,j = 0;
    char* bins[argc-4];

    char* file_input = (char*) argv[2];
    char* file_output = (char*) argv[3];

    for (i = 4; i < argc; i++)
    {
        bins[j] = strdup(argv[i]);
        j++;
    }

/*
    printf("File input: %s\n",file_input);
    printf("File output: %s\n", file_output);

    for (i = 0; i < argc-4; i++)
    {
        printf("Bin nº %d: %s\n",i+1,bins[i]);
    }
    
*/

    execute_bins(file_input,file_output,bins,argc-4);

    return 0;
}

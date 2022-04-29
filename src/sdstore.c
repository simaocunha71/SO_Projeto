#include "includes/sdstore.h"

// Cliente
void execute_bins(char* arguments[], int num_args){
    
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

int main(int argc, char *argv[]){

    if(!strcmp(argv[1], "status")){
        int fifo_status=open("comunication", O_WRONLY);

        if(fifo_status < 0){
            perror("open");
            return 1;
        }

        write(fifo_status,argv[1],strlen(argv[1]));
        close(fifo_status);
        return 0;
    }
    else if (argc < 5) {
        printf ("Nº de argumentos inválido\n");
        return -1;
    }
    char* pipeinfo= malloc(1);
    for(int i =2; i<argc; i++){
        pipeinfo = mystrcat(pipeinfo, argv[i]);
        pipeinfo=mystrcat(pipeinfo, " ");
    }

    int fifo_fd=open("comunication", O_WRONLY);

    if(fifo_fd < 0){
        perror("open");
        return 1;
    }

    write(fifo_fd,pipeinfo,strlen(pipeinfo));
    close(fifo_fd);


    char buffer[1024];
    ssize_t bytes_read;
    int results=open("client0", O_RDONLY);
    while((bytes_read=read(results,buffer,1024))>0){
        write(1,buffer,bytes_read);
    }


/*
    printf("File input: %s\n",file_input);
    printf("File output: %s\n", file_output);

    for (i = 0; i < argc-4; i++)
    {
        printf("Bin nº %d: %s\n",i+1,binaries_to_execute[i]);
    }
    
    if ((strcmp(argv[1], "proc-file") == 0))
        execute_bins(argv, argc);
    else
        printf("DEBUG: Provavelmente falta a flag \"proc-file\"\n");
*/

    return 0;
}

#include "includes/sdstore.h"

// Cliente
void execute_bins(char* arguments[], int num_args){
    
    int pipe_bn[2];
    char* input = (char*) arguments[2];
    char* output = (char*) arguments[3];

    CONFIG c = load_configurations("bin/sdstore.conf", "obj/"); //isto provavelmente vai para o servidor e vai ser passado por argumento nesta função

    for (int i = 4; i < num_args; i++){

        if ((strcmp(arguments[i],"nop")) != 0) {

            if((pipe(pipe_bn)) < 0)
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

    //caso que o argumento do cliente seja so "status"
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
    //pid do cliente para usar como nome do pipe de comunicacao do cliente->servidor
    int pid=getpid();

    //string aonde vai ter toda a informação enviada para o servidor
    char* pipeinfo=malloc(100);
    
    //eu sei que sprintf n devia ser usado mas é só usado para meter o numero do pid no pipeinfo
    sprintf(pipeinfo, "%d", pid);

    //lembrar de separar todos os argumentos por espaço, para quando o servidor ler, conseguir-mos facilmente separar os argumentos
    pipeinfo=mystrcat(pipeinfo, " ");

    //este ciclo for separa todos os argumentos enviados para o servidor
    for(int i =2; i<argc; i++){
        pipeinfo = mystrcat(pipeinfo, argv[i]);
        pipeinfo=mystrcat(pipeinfo, " ");
    }




    //abrimos o pipe aonde o server vai escrever para o ciente
    char* pipo=malloc(100);
    //meter o pid em string
    sprintf(pipo, "%d", pid);

    //criamos o pipe com o pid do cliente
    if(mkfifo(pipo, 0666)<0){
            perror("mkfifo client");
        }
    //vamos abrir o pipe de comunicaçao para escrever la dentro o que queremos enviar
    int fifo_fd;
    if((fifo_fd=open("comunication", O_WRONLY))< 0){
        perror("open");
        return 1;
    }
    //escrever tudo que temos para escrever para o servidor    
    write(fifo_fd,pipeinfo,strlen(pipeinfo));
    //fechamos o pipe
    close(fifo_fd);

    
    //abrimos este pipe primeiro pq senao pode dar conflito mais tarde no servidor


    //e abrimos o descritor de leitura
    int results;
    if((results=open(pipo, O_RDONLY))<0){
        perror("open results");
        return 1;
    }
    //inicializar o buffer para leitura
    char buffer[1024];
    ssize_t bytes_read;


    //lemos e escrevemos no terminal tudo o que o servidor quer nos enviar.
    while((bytes_read = read(results,buffer,1024)) > 0){
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
    close(results);
    unlink(pipo);
    return 0;
}

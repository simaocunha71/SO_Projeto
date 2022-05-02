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

int execute_status(char* str_status){
    int fifo_status = open("status_fifo", O_WRONLY);
    if(fifo_status < 0){
        perror("Erro a abrir o pipe para o comando \"status\"");
        return -1;
    }
    write(fifo_status,str_status,strlen(str_status));
    close(fifo_status);
    return 0;
}

//$ ./sdstore proc-file FILEINPUT FILEOUTPUT bcompress nop gcompress encrypt nop
int main(int argc, char *argv[]){

    if (argc < 5) {
        perror ("Nº de argumentos inválido! Tenta outra vez...");
        return -1;
    }
    
    if(strcmp(argv[1], "status") == 0){
        return execute_status(argv[1]);
    }
    else if(strcmp(argv[1], "proc-file") == 0){

        /***************************************Envio do pedido do cliente**************************************************/
        //pid do cliente para usar como nome do pipe de comunicacao do cliente->servidor
        int pid_client = getpid();
        char* str_pid_client = inttoString(pid_client);

        int size_of_args = 0;
        for(int i = 2; i < argc; i++)
            size_of_args += strlen(argv[i]);

        //String aonde vai ter toda a informação enviada para o servidor: capacidade para o pid + tamanho das strings que compoe a informação
        //Formato desta string com, por exemplo, 2 binários a executar: "PID_Cliente File_Input File_Output Binario1 Binario2"
        char* info_toSend = malloc(sizeof(char) * (sizeof(int) + size_of_args + 1));
        strcat(info_toSend, str_pid_client);
        strcat(info_toSend, " "); 

        //Parse dos nomes dos argumentos
        for(int i = 2; i < argc; i++){
            strcat(info_toSend, argv[i]);
            if(i < argc - 1)
                strcat(info_toSend, " ");
        }

        //abrimos o pipe cliente->servidor
        char* identifier_fifo = "_execute_fifo_request"; //formato do nome do fifo: [INT]_execute_fifo (ex: 3519_execute_fifo_request)
        char* fifo_request = malloc(sizeof(char) * (sizeof(int) + strlen(identifier_fifo)));
        strcat(fifo_request, str_pid_client);
        strcat(fifo_request, identifier_fifo);

        //criamos o pipe com o pid do cliente
        if(mkfifo(fifo_request, 0666) < 0)
            perror("Erro a fazer o pipe do cliente para o servidor");

        //vamos abrir o pipe cliente->servidor
        int fifo_fd;
        if((fifo_fd = open(fifo_request, O_WRONLY))< 0){
            perror("Erro a criar o nome para o fifo de execução de binários");
            return -1;
        }

        //Escrevemos a informação para o servidor: nome_input, nome_output, nomes dos binários a executar 
        write(fifo_fd,info_toSend,strlen(info_toSend));

        //fechamos o pipe cliente->servidor
        close(fifo_fd);

        //////////////////////////////////////////////////////////////////////////////
        //NOTA:
        //abrimos este pipe primeiro pq senao pode dar conflito mais tarde no servidor
        //////////////////////////////////////////////////////////////////////////////

        /***************************************Resposta recebida pelo cliente**********************************************/
        //Abrimos o descritor de leitura
        int results;
        if((results = open(fifo_request, O_RDONLY))<0){
            perror("Erro a abrir o fifo com o pedido do cliente");
            return -1;
        }
        //Inicializar o buffer para leitura
        int size_buffer = 1024;
        char buffer[size_buffer];
        ssize_t bytes_read;


        //Lemos e escrevemos no terminal tudo o que o servidor quer enviar
        while((bytes_read = read(results,buffer,size_buffer)) > 0){
            write(1,buffer,bytes_read);
        }

        close(results);
        unlink(fifo_request);
    }
    else {
        perror("Comando desconhecido! Tenta outra vez...");
        return -1;
    }

    return 0;
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
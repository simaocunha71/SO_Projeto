#include "includes/sdstore.h"

#define FIFO_NAME "fifo"

// Cliente

int execute_status(char* str_status){
    int fifo_status = open(FIFO_NAME, O_WRONLY);
    printf("[Cli]: Abri descritor de fifo\n");
    if(fifo_status < 0){
        perror("Erro a abrir o descritor do pipe para o comando \"status\"");
        return -1;
    }
    write(fifo_status,str_status,strlen(str_status));
    close(fifo_status);
    printf("[Cli]: Fechei descritor de fifo\n");
    return 0;
}

//$ ./sdstore proc-file FILEINPUT FILEOUTPUT bcompress nop gcompress encrypt nop
int main(int argc, char *argv[]){

    if (argc < 2 && strcmp(argv[1], "status") != 0) {
        perror ("Nº de argumentos inválido! Tenta outra vez...");
    }
    
    if (argc < 5 && strcmp(argv[1], "proc-file") != 0) {
        perror ("Nº de argumentos inválido! Tenta outra vez...");
    }

    
    if(strcmp(argv[1], "status") == 0){
        return execute_status(argv[1]); //TODO
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
        char* info_toSend = malloc(sizeof(char) * (sizeof(int) + strlen(argv[1]) + size_of_args + 1));
        strcat(info_toSend, str_pid_client);
        strcat(info_toSend, " "); 
        strcat(info_toSend, "P ");//flag para proc-file

        //Parse dos nomes dos argumentos
        for(int i = 2; i < argc; i++){
            strcat(info_toSend, argv[i]);
            if(i < argc - 1)
                strcat(info_toSend, " ");
        }

        //criamos o pipe com o pid do cliente
        if(mkfifo(str_pid_client, 0666) < 0)
            perror("Erro a fazer o pipe do cliente (com PID) para o servidor");

        printf("[Cli]: Criei fifo com pid\n");

        //vamos abrir o pipe cliente->servidor
        int fifo_send_fd;
        if((fifo_send_fd = open(FIFO_NAME, O_WRONLY)) < 0){
            perror("Erro a criar o descritor para o pipe do comando \"proc-file\" (resposta do servidor)");
            return -1;
        }

        printf("[Cli]: Abri descritor de pid de cliente com pid\n");

        //Escrevemos a informação para o servidor: nome_input, nome_output, nomes dos binários a executar 
        write(fifo_send_fd,info_toSend,strlen(info_toSend));

        //fechamos o pipe cliente->servidor
        close(fifo_send_fd);
        printf("[Cli]: Fechei descritor de pid de cliente com pid\n");

        //////////////////////////////////////////////////////////////////////////////
        //NOTA:
        //abrimos este pipe primeiro pq senao pode dar conflito mais tarde no servidor
        //////////////////////////////////////////////////////////////////////////////

        /***************************************Resposta recebida pelo cliente**********************************************/
        //Abrimos o descritor de leitura
        int fifo_receive_fd;
        if((fifo_receive_fd = open(str_pid_client, O_RDONLY)) < 0){
            perror("Erro a abrir o fifo com o pedido do cliente");
            return -1;
        }
        printf("[Cli]: Abri descritor de fifo para receber resposta\n");
        //Inicializar o buffer para leitura
        int size_buffer = 1024;
        char buffer[size_buffer];
        ssize_t bytes_read;


        //Lemos e escrevemos no terminal tudo o que o servidor quer enviar
        while((bytes_read = read(fifo_receive_fd,buffer,size_buffer)) > 0){
            write(1,buffer,bytes_read);
        }

        close(fifo_receive_fd);
        printf("[Cli]: Fechei descritor de fifo para receber resposta\n");
        //unlink(FIFO_NAME);
        unlink(str_pid_client);
        printf("[Cli]: Apaguei fifo\n");
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
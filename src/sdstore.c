#include "includes/sdstore.h"

#define FIFO_NAME "fifo"

// Cliente

int main(int argc, char *argv[]){

    if (argc < 2 && strcmp(argv[1], "status") == 0) {
        perror ("Nº de argumentos inválido! Tenta outra vez...");
    }
    
    if (argc < 5 && strcmp(argv[1], "proc-file") == 0) {
        perror ("Nº de argumentos inválido! Tenta outra vez...");
    }

    
    if(strcmp(argv[1], "status") == 0){
        /***************************************Envio do pedido do cliente**************************************************/
        //pid do cliente para usar como nome do pipe de comunicacao do cliente->servidor
        int pid_client = getpid();
        char* str_pid_client = inttoString(pid_client);

        char* info_toSend = malloc(sizeof(char)); 
        my_strcat(info_toSend, str_pid_client);
        my_strcat(info_toSend, " "); 
        my_strcat(info_toSend, "S");
        my_strcat(info_toSend, "\n");


        //criamos o pipe com o pid do cliente
        if(mkfifo(str_pid_client, 0666) < 0)
            perror("Erro a fazer o pipe do cliente (com PID) para o servidor");

        //vamos abrir o pipe cliente->servidor
        int fifo_send_fd;
        if((fifo_send_fd = open(FIFO_NAME, O_WRONLY)) < 0){
            perror("Erro a criar o descritor para o pipe do comando \"status\" (resposta do servidor)");
            return -1;
        }

        write(fifo_send_fd,info_toSend,strlen(info_toSend)); 

        //fechamos o pipe cliente->servidor
        close(fifo_send_fd);

        /***************************************Resposta recebida pelo cliente**********************************************/
        
        //Abrimos o descritor de leitura

        int fifo_receive_fd;
        if((fifo_receive_fd = open(str_pid_client, O_RDONLY)) < 0){
            perror("Erro a abrir o fifo com o pedido do cliente");
            return -1;
        }
        
        //Inicializar o buffer para leitura
        int size_buffer = 1024;
        char buffer[size_buffer];
        ssize_t bytes_read;

        while((bytes_read = read(fifo_receive_fd,buffer,size_buffer)) > 0){
            write(1,buffer,bytes_read);
        }


        close(fifo_receive_fd);
        unlink(str_pid_client);
    }
    else if(strcmp(argv[1], "proc-file") == 0){

        /***************************************Envio do pedido do cliente**************************************************/
        //pid do cliente para usar como nome do pipe de comunicacao do cliente->servidor
        int pid_client = getpid();
        char* str_pid_client = inttoString(pid_client);

        int size_of_args = 0;
        for(int i = 2; i < argc; i++)
            size_of_args += strlen(argv[i]);

        char* info_toSend = malloc(sizeof(char)); 
        my_strcat(info_toSend, str_pid_client);
        my_strcat(info_toSend, " "); 
        my_strcat(info_toSend, "P ");//flag para proc-file

        //Parse dos nomes dos argumentos
        for(int i = 2; i < argc; i++){
            my_strcat(info_toSend, argv[i]);
            if(i != argc - 1)
                my_strcat(info_toSend, " ");
        }

        my_strcat(info_toSend, "\n");
        //criamos o pipe com o pid do cliente
        if(mkfifo(str_pid_client, 0666) < 0)
            perror("Erro a fazer o pipe do cliente (com PID) para o servidor");

        //vamos abrir o pipe cliente->servidor
        int fifo_send_fd;
        if((fifo_send_fd = open(FIFO_NAME, O_WRONLY)) < 0){
            perror("Erro a criar o descritor para o pipe do comando \"proc-file\" (resposta do servidor)");
            return -1;
        }

        write(fifo_send_fd,info_toSend,strlen(info_toSend)); 

        //fechamos o pipe cliente->servidor
        close(fifo_send_fd);

        /***************************************Resposta recebida pelo cliente**********************************************/
        //Abrimos o descritor de leitura
        int fifo_receive_fd;
        if((fifo_receive_fd = open(str_pid_client, O_RDONLY)) < 0){
            perror("Erro a abrir o fifo com o pedido do cliente");
            return -1;
        }
        //Inicializar o buffer para leitura
        int size_buffer = 1024;
        char buffer[size_buffer];
        ssize_t bytes_read;

        int flag =1;
        //Lemos e escrevemos no terminal tudo o que o servidor quer enviar
        while(flag && (bytes_read = read(fifo_receive_fd,buffer,size_buffer)) > 0){
            buffer[bytes_read]='\0';
            if(strcmp(buffer,"done!")==0){
                flag = 0;
                close(fifo_receive_fd);
            }
            write(1,buffer,bytes_read);
        }

        close(fifo_receive_fd);
        unlink(str_pid_client);
    }
    else {
        perror("Comando desconhecido! Tenta outra vez...");
        return -1;
    }

    return 0;
}
#include "includes/sdstored.h"

#define FIFO_NAME "fifo"

// Servidor


CONFIG start_server(char* config_filename, char* binarys_folder){
    CONFIG c = load_configurations(config_filename, binarys_folder);
    //printConfigs(c);
    return c;
}

//Debug
void printConfigs(CONFIG c){
    while(c != NULL){
        printf("Executável: %s | Nº instâncias máximas: %d | Path do executável: %s\n",c->binary_name, c->max_instances, c->path_name);
        c=c->next;
    }
}

void sendServerStatus(){
    printf("Opção ainda não implementada!\n");
}

int execute_commands_in_pipeline(CONFIG c, char* input, char* output, char** binaries_array, int number_of_commands){

    int input_fd, output_fd;
    //abre o input pra onde lemos
    if((input_fd= open(input, O_RDONLY))==-1){
        perror("input_fd open");
    }

    //abre o output para onde vamos escrever
    if((output_fd=open(output, O_WRONLY | O_CREAT | O_TRUNC, 0640))==-1){
        perror("output_fd open");
    }

    //printf("%d | %s | %s | %s\n", nc, path, input, output);

    //guardamos o standard output pra debugging
   // int stdout = dup(1);

    //mudamos o input default pra o nosso ficheiro de input
    dup2(input_fd, 0);
    //mudamos o output default pra o nosso ficheiro output
    dup2(output_fd,1);
    //fechamos o input e o output pq os execs vao usar o 0 e 1
    close(input_fd);
    close(output_fd);


    //redirection(input,0,1);
    //redirection(output,1,1);


    //criamos os pipes necessarios pra executar todos os comandos
    int i,status;
    //number_of_commands--;

    //caso so haja um comando
    if(number_of_commands == 1){
        if(fork() == 0){
            execute_config(c,binaries_array[0]);
            _exit(0);
        }
        wait(NULL);
    }
    else{
        int p[number_of_commands-1][2];
        for (i = 0; i < number_of_commands; i++){           //CASO DE INICIO
            if (i == 0){
                pipe(p[i]);
                if (fork()==0){
                   // printf("INICO FIRST PIPE\n");
                    close(p[i][0]);
                   // printf("close\n");
                    dup2(p[i][1],1);
                   // printf("dup2\n");
                    close(p[i][1]);
                   // printf("close2\n");
                    execute_config(c,binaries_array[0]);
                   // printf("has executed\n");
                    _exit(0);
                   // printf("FIM FIRST PIPE\n");
                }       
                else
                    close(p[i][1]);
            }
            else if (i == number_of_commands - 1){          //ULTIMO
                if (fork()==0){
                   // printf("INICO LAST PIPE\n");
                    dup2(p[i-1][0],0);
                    close(p[i-1][0]);
                    execute_config(c,binaries_array[0]);
                    _exit(0);
                   // printf("FIM LAST PIPE\n");
                }
                else 
                    close(p[i-1][0]);
            }
            else {                                              //MEIO
                pipe(p[i]);
                if (fork() == 0){
                   // printf("INICO MIDDLE PIPE\n");
                    close(p[i][0]);
                    dup2(p[i][1],1);
                    close(p[i][1]);
                    dup2(p[i-1][0],0);
                    close(p[i-1][0]);
                    execute_config(c,binaries_array[0]);
                    _exit(0);
                   // printf("LAST MIDDLE PIPE\n");
                }
                else {
                    close(p[i][1]);
                    close(p[i-1][0]);
                }
            }
        }
        
        for (i = 0; i < number_of_commands; i++) 
            wait(&status);
    }
    return 0;
}



int sendtoclient(int nr){
    printf("A implementar!\n");
    return 0;

}

//Apagar fifos pela bash: find . -type p -delete
//./sdstored bin/sdstore.conf obj/
int main(int argc, char const *argv[]){
    char* filename_config = (char*) argv[1];
    char* binarys_folder = (char*) argv[2];


    CONFIG c = start_server(filename_config, binarys_folder);
    //execute_config(c,"bdecompress","test_files/bfile.bz2","test_files/new.csv");


    //abre o pipe com nome pra estabelecer a ligaçao do server
    if((mkfifo(FIFO_NAME, 0666)) < 0){
        perror("Erro a criar o fifo do servidor");
    }
    printf("[SV]: Criei FIFO\n");
    int fifo_fd, fifo_fd_write;

    
    if((fifo_fd = open(FIFO_NAME, O_RDONLY)) < 0){
        perror("Erro a abrir o descritor de comunicação com clientes");
    }
    printf("[SV]: Abri descritor de comunicação com clientes\n");


    //para o server estar sempre a funcionar, temos este descritor SEMPRE aberto (nunca o vamos usar)
    if((fifo_fd_write = open(FIFO_NAME, O_WRONLY)) < 0){
        perror("Erro a abrir o descritor de sobrevivência do servidor");
    }
    printf("[SV]: Abri descritor de sobrevivencia\n");

    //Array dinâmico com todos os binarios a executar
    char** binaries_to_execute = NULL;

    //buffer para ler e escrever
    char buffer_from_fifo[BUFFERSIZE];
    ssize_t bytes_read;

    //ciclo infinito que nunca devia ser quebrado, vai estar sempre a ler do pipe de comunicacao!
    

    while((bytes_read = read(fifo_fd,buffer_from_fifo,BUFFERSIZE)) > 0){
        char* buffer_copy = strdup(buffer_from_fifo); //necessário fazer esta cópia porque o buffer_from_fifo vai ficar destruído depois do parse

        char* client_pid = strdup(strsep(&buffer_copy, " "));
        char* operation_mode = strdup(strsep(&buffer_copy, " "));

        write(1, buffer_from_fifo, strlen(buffer_from_fifo));
        printf("[SV]: Enviei mensagem para cliente com pid\n");

        int client_write;
        if((client_write = open(client_pid, O_WRONLY)) < 0){
            perror("Erro a abrir o descritor do fifo do cliente");
        }
        printf("[SV]: Abri descritor de cliente com pid\n");

        char* pending_message = "pending...\n";
        write(client_write, pending_message, strlen(pending_message));

        ////////////////////TODO//////////////////////////////////
        //fazer aqui o código que verifica se podemos executar o pedido do cliente ou se metemos esse pedido numa queue
        //metemos no pipo o clientpid
        //mystrcat(pipo, clientpid);
        //////////////////////////////////////////////////////////

        
        //Execução do comando "status"
        if(strcmp(operation_mode, "S") == 0){
            char* executing_message = "STATUS executing...\n";
            write(client_write, executing_message, strlen(executing_message));

            /*
            
                Inserir o algoritmo para o comando status
            
            */

            char* success_message = "STATUS done!\n";
            write(client_write, success_message, strlen(success_message));
            close(fifo_fd);
        }
        
        //Execução do comando "proc-file"
        if(strcmp(operation_mode, "P") == 0){

            /////////////////////////////////Funcionalidade extra////////////////////////////
            //esta priority como ainda n ta implementada os comandos de execucao nao irao usar
            //logo so tirar de comentario isto quando for para implementar o priority
            //isso ira mudar um coto o codigo
            //char* priority = strdup(strsep(&buffer, " "));
            /////////////////////////////////////////////////////////////////////////////////
            
            char* inputfile = strdup(strsep(&buffer_copy, " "));
            char* outputfile= strdup(strsep(&buffer_copy, " "));

            int number_of_commands = get_binaries_num(buffer_copy);

            //Parse dos binários a executar
            binaries_to_execute = create_binaries_array(buffer_copy);
            /*
            for(number_of_commands = 0;buffer_copy != NULL; number_of_commands++){
                binaries_to_execute = add_string_to_array(binaries_to_execute, strdup(strsep(&buffer_copy, " "))); //está a crashar aqui
            }
            */

            //for (int i = 0; i < number_of_commands; i++){
            //    printf("||%s\n",binaries_to_execute[i]);
            //}
            
            
            //Escrever ao cliente que vamos executar o pedido dele
            char* executing_message = "executing...\n";
            write(client_write, executing_message, strlen(executing_message));

            //Pipeline dos comandos a executar
            if(fork() == 0){
                if(execute_commands_in_pipeline(c,inputfile,outputfile,binaries_to_execute,number_of_commands) != 0){ 
                    perror("Erro a efetuar a execução da pipeline dos binários");
                }

                close(client_write);
                close(fifo_fd_write);
                printf("[SV]: Fechei descritor de comunicação com clientes\n");

                close(fifo_fd);
                exit(0);
            }
            wait(NULL);
            
            //escrevemos no cliente no cliente que concluímos o pedido deles
            char* success_message = "done!\n";
            write(client_write, success_message, strlen(success_message));

            write(client_write, outputfile, strlen(outputfile));
        }

        
        //fechamos o pipe para aonde enviamos a informação ao cliente
        close(client_write);
        printf("[SV]: Fechei descritor de cliente com pid\n");
    }
    
    //printf("Código de erro: %d\n",errno); //???


   //******************Fecho do servidor***********************
   //char end_buffer[11];
   //while(readln(STDIN_FILENO,end_buffer,strlen(end_buffer)) > 0){
   //    if(strcmp(end_buffer, "end server") == 0){
   //         unlink(FIFO_NAME);
   //         printf("[SV]: Apaguei fifo\n");
   //    }
   //    else printf("[SV]: Continuo a dar...\n");
   //
   //}

   return 0;
   
}


/*
printf("----MAIN----\nCF: %s\nBF: %s\n------------\n", filename_config,binarys_folder);
//execute_config(c,"bcompress","test_files/file.csv","test_files/bfile.bz2");
return 0;
*/
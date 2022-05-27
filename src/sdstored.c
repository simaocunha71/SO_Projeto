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
        perror("Erro a abrir o ficheiro de input");
    }

    //abre o output para onde vamos escrever
    if((output_fd=open(output, O_WRONLY | O_CREAT | O_TRUNC, 0640))==-1){
        perror("Erro a abrir o ficheiro de output");
    }
    //int debug= dup(1);
    //write(debug, input, strlen(input));
    //write(debug, output, strlen(output));
    //printf("\nentrou em exec_in_pipe %d\n", number_of_commands);
    //for(int i = number_of_commands; i>0; i--) write(original_stdout, binaries_array[i], strlen(binaries_array[i]));
    //write(debug, "\n fim\n", strlen("\n fim\n"));
    //mudamos o input default pra o nosso ficheiro de input
    dup2(input_fd, 0);
    //mudamos o output default pra o nosso ficheiro output
    dup2(output_fd,1);
    //fechamos o input e o output pq os execs vao usar o 0 e 1
    close(input_fd);
    close(output_fd);



    //criamos os pipes necessarios pra executar todos os comandos
    int i,status;

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
                    close(p[i][0]);
                    dup2(p[i][1],1);
                    close(p[i][1]);

                    execute_config(c,binaries_array[0]);
  
                    _exit(0);
                }       
                else
                    close(p[i][1]);
            }
            else if (i == number_of_commands - 1){          //ULTIMO
                if (fork()==0){
                    dup2(p[i-1][0],0);
                    close(p[i-1][0]);

                    execute_config(c,binaries_array[i]);

                    _exit(0);
                }
                else 
                    close(p[i-1][0]);
            }
            else {                                              //MEIO
                pipe(p[i]);
                if (fork() == 0){
                    close(p[i][0]);
                    dup2(p[i][1],1);
                    close(p[i][1]);
                    dup2(p[i-1][0],0);
                    close(p[i-1][0]);

                    execute_config(c,binaries_array[i]);

                    _exit(0);
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

void create_status_message(int fileDescriptor, CONFIG c, Queue q, int vetor_instances_original[]){
    get_status_from_queue (fileDescriptor, q);
    get_status_from_config(fileDescriptor, c,vetor_instances_original);
}

void close_server(int signum){
    unlink(FIFO_NAME);
    kill(0,signum);

    //not sure if needed
    kill(getpid(), signum);

    _exit(3);
}


//Apagar fifos pela bash: find . -type p -delete
//./sdstored bin/sdstore.conf obj/
int main(int argc, char const *argv[]){
    signal(SIGTERM, close_server);
    signal(SIGINT, close_server);

    char* filename_config = (char*) argv[1];
    char* binarys_folder = (char*) argv[2];


    CONFIG c = start_server(filename_config, binarys_folder);

    Queue q = init_queue();

    //Copia das instancias originais dos binarios (vao estar por ordem)
    CONFIG copy = copy_config(c);
    int s = size_array(copy);
    int vector_originalInstances[s];
    for(int i = 0; i < s; i++){
        vector_originalInstances[i] = copy->max_instances;
        copy = copy->next;
    }
    free_config(copy);

    


    //abre o pipe com nome pra estabelecer a ligaçao do server
    if((mkfifo(FIFO_NAME, 0666)) < 0){
        perror("Erro a criar o fifo do servidor");
    }
    int fifo_fd, fifo_fd_write;

    if((fifo_fd = open(FIFO_NAME, O_RDONLY)) < 0){
        perror("Erro a abrir o descritor de comunicação com clientes");
    }


    //para o server estar sempre a funcionar, temos este descritor SEMPRE aberto (nunca o vamos usar)
    if((fifo_fd_write = open(FIFO_NAME, O_WRONLY)) < 0){
        perror("Erro a abrir o descritor de sobrevivência do servidor");
    }

    //Array dinâmico com todos os binarios a executar
    char** binaries_to_execute = NULL;

    //buffer para ler e escrever
    char buffer_from_fifo[BUFFERSIZE];
    ssize_t bytes_read;
    int original_stdout = dup(1);

    //ciclo infinito que nunca devia ser quebrado, vai estar sempre a ler do pipe de comunicacao!
    while((bytes_read = read(fifo_fd,buffer_from_fifo,BUFFERSIZE)) > 0){
        write(original_stdout, buffer_from_fifo, strlen(buffer_from_fifo)+1);
        write(original_stdout, "\n", 2);
        write(original_stdout ,"chupamos\n", strlen("chupamos\n"));
        
        //pid proc-file ficheiro1 ficheiro2 comandos


        
        char* buffer_copy = strdup(buffer_from_fifo); //necessário fazer esta cópia porque o buffer_from_fifo vai ficar destruído depois do parse
        if(strcmp(buffer_copy, "1") == 0){
            //char* task_pid = strdup(strsep(&buffer_copy, " "));
            request_out(c,q->inicio->binaries_to_execute,q->inicio->binaries_num); 
            remove_task(q);
                write(original_stdout, "\nchegou1\n", strlen("\nchegou1\n"));
            if(!isEmpty(q)){
                write(original_stdout, "\nchegou2\n", strlen("\nchegou2\n"));

                    write(original_stdout, "mode 1: O que vai pra dentro do request enter:\n", strlen("mode 1: O que vai pra dentro do request enter:\n"));
                    for(int i = q->inicio->binaries_num; i>0; i--){
                        write(original_stdout, q->inicio->binaries_to_execute[i], strlen(q->inicio->binaries_to_execute[i]));
                        write(original_stdout, "\n", strlen("\n"));

                    }
                    write(original_stdout, "\n", strlen("\n"));
                if(canExecuteBinaries(c, q->inicio->binaries_to_execute, q->inicio->binaries_num)){
                    write(original_stdout, "\nchegou3\n", strlen("\nchegou3\n"));
                    request_enter(c,q->inicio->binaries_to_execute,q->inicio->binaries_num);
                    write(original_stdout,"\nchegou4\n", strlen("\nchegou4\n"));

                    if(fork()==0){
                        int task_client;
                        char* qID = malloc(10);
                        sprintf(qID, "%d", q->inicio->id);

                        if((task_client= open(qID, O_WRONLY)) < 0){
                            perror("real_client_pid open error");
                        }
                        write(task_client, "executing...\n", strlen("executing...\n"));
                        if(fork()==0){

                            if(execute_commands_in_pipeline(c,q->inicio->file_input,
                                                                          q->inicio->file_output,
                                                                          q->inicio->binaries_to_execute,
                                                                          q->inicio->binaries_num) != 0){ 
                                            perror("Erro a efetuar a execução da pipeline dos binários");
                            }
                            _exit(0);
                        }
                        wait(NULL);
                        write(task_client, "done!", strlen("done!"));     
                        write(fifo_fd_write, "1", 2);                           
                        _exit(0);
                    }
                }
            
            }
        }
        else{

        char* client_pid = strdup(strsep(&buffer_copy, " "));
        char* operation_mode = strdup(strsep(&buffer_copy, " "));

        if(strcmp(client_pid, "0")==0){
            write(original_stdout, "\nmode 0:entrou\n", strlen("\nmode 0:entrou\n"));
            //0 pid proc-file ficheiro1 ficheiro2 comandos.
            //Como o servidor vai mandar algo do genero 0 PidDoCliente Ficheiros Comandos
            char* real_client_pid = strdup(operation_mode);
            //write(original_stdout, real_client_pid, strlen(real_client_pid));
            //write(original_stdout, "\n", 1);

            char* inputfile = strdup(strsep(&buffer_copy, " "));
            //write(original_stdout, inputfile, strlen(inputfile));
            //write(original_stdout, "\n", 1);

            char* outputfile= strdup(strsep(&buffer_copy, " "));
            //write(original_stdout, outputfile, strlen(outputfile));
            //write(original_stdout, "\n", 1);
            //write(original_stdout, buffer_copy, strlen(buffer_copy));

            int number_of_commands = get_binaries_num(buffer_copy);

            binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);
            request_out(c,binaries_to_execute,number_of_commands); 
            //Parse dos binários a executar
            int real_client;
            if((real_client= open(real_client_pid, O_WRONLY)) < 0){
                        perror("real_client_pid open error");
            }
            write(real_client, "done!", strlen("done!")+1);
            close(real_client);
            if(!isEmpty(q)){
                write(original_stdout, "\nmode 0:entrou na isEmpty\n", strlen("\nmode 0:entrou na isEmpty\n"));
                
            
                if(canExecuteBinaries(c, q->inicio->binaries_to_execute, q->inicio->binaries_num)){
                    write(original_stdout, "\nmode 0:entrou na CanExec\n", strlen("\nmode 0:entrou na CanExec\n"));
                    request_enter(c,q->inicio->binaries_to_execute,q->inicio->binaries_num);
                    write(original_stdout, "\nmode 0:entrou fez request_enter\n", strlen("\nmode 0:entrou fez request_enter\n"));
                    if(fork()==0){
                        int task_client;
                        char* qID = malloc(10);
                        sprintf(qID, "%d", q->inicio->id);
                        if((task_client= open(qID, O_WRONLY)) < 0){
                            perror("real_client_pid open error");
                        }
                        write(task_client, "executing...\n", strlen("executing...\n"));
                        if(fork()==0){
                            if(execute_commands_in_pipeline(c,q->inicio->file_input,
                                                                          q->inicio->file_output,
                                                                          q->inicio->binaries_to_execute,
                                                                          q->inicio->binaries_num) != 0){ 
                                            perror("Erro a efetuar a execução da pipeline dos binários");
                                        }
                            _exit(0);
                        }
                        wait(NULL);
                        write(task_client, "done!", strlen("done!"));     
                        write(fifo_fd_write, "1", 2);       
                        _exit(0);                    

                    }
                }
                write(original_stdout, "\nmode 0: executou com sucesso\n", strlen("\nmode 0: executou com sucesso\n"));
                
            }
            else{
                write(original_stdout, "\nmode 0: rejeitou isEmpty\n", strlen("\nmode 0: rejeitou isEmpty\n"));
            }

            
        }
        else{
            int client_write;
            if((client_write = open(client_pid, O_WRONLY)) < 0){
                perror("Erro a abrir o descritor do fifo do cliente");
            }
                //Execução do comando "status"
            if(strcmp(operation_mode, "S") == 0){
                if(fork() == 0){
                    create_status_message(client_write, c,q, vector_originalInstances);
                    //printf("%s\n", status_message);
                    close(client_write);
                    close(fifo_fd_write);
                    close(fifo_fd);
                    exit(0);
                }
                    close(client_write);
            }
            //Execução do comando "proc-file"
            if(strcmp(operation_mode, "P") == 0){
                char* request = malloc(strlen(buffer_copy)+ strlen(client_pid)+4);
                strcpy(request, "0 ");
                strcat(request, client_pid);
                strcat(request, " ");
                strcat(request,buffer_copy);
                char* inputfile = strdup(strsep(&buffer_copy, " "));
                char* outputfile= strdup(strsep(&buffer_copy, " "));
                int number_of_commands = get_binaries_num(buffer_copy);
                //Parse dos binários a executar
                write(original_stdout, buffer_copy, strlen(buffer_copy));
                printf("\n buffer copy ^, number of comands %d\n", number_of_commands);
                binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);
                if(canExecuteBinaries(c, binaries_to_execute, number_of_commands)==1){
                    request_enter(c,binaries_to_execute,number_of_commands);
                    if(fork()==0){
                        char* executing_message = "executing...\n";
                        write(client_write, executing_message, strlen(executing_message)+1);
                        write(original_stdout, "\na executar\n", strlen("\na executar\n"));
                        //printConfigs(c);
                            if(fork()==0){
                                sleep(10);
                                if(execute_commands_in_pipeline(c, inputfile,outputfile,binaries_to_execute,number_of_commands) != 0){ 
                                    perror("Erro a efetuar a execução da pipeline dos binários");
                                }
                                _exit(0);
                            }
                            wait(NULL);
                        //debugging
                        write(original_stdout, request, strlen(request));
                        /*
                        mandar ao servidor pra fazer request_out
                        */
                        write(fifo_fd_write, request, strlen(request));  
                        //request_out(c,binaries_to_execute,number_of_commands);   
                        _exit(0);                  
                    }
                }
                else{
                        char* message = "pending...\n";
                        write(client_write, message, strlen(message)+1);
                        add_task(q, inputfile, outputfile, binaries_to_execute, number_of_commands, atoi(client_pid));
                }
            }
        }
    }
    }
    
        unlink(FIFO_NAME);
        return 0;
    }
/*
printf("----MAIN----\nCF: %s\nBF: %s\n------------\n", filename_config,binarys_folder);
//execute_config(c,"bcompress","test_files/file.csv","test_files/bfile.bz2");
return 0;
*/
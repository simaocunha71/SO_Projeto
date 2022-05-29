#include "includes/sdstored.h"

#define FIFO_NAME "fifo"

// Servidor

CONFIG start_server(char* config_filename, char* binarys_folder){
    CONFIG c = load_configurations(config_filename, binarys_folder);
    return c;
}

int execute_commands_in_pipeline(CONFIG c, char* input, char* output, char** binaries_array, int number_of_commands){
    int input_fd, output_fd;

    if((input_fd = open(input, O_RDONLY))==-1){
        perror("Erro a abrir o ficheiro de input");
    }

    if((output_fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0640))==-1){
        perror("Erro a abrir o ficheiro de output");
    }

    dup2(input_fd, 0);
    dup2(output_fd,1);
    close(input_fd);
    close(output_fd);

    int i,status;

    //caso so haja um comando a processar
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


void create_status_message(int fileDescriptor, CONFIG c, Queue q, int vetor_instances_original[]){
    get_status_from_queue (fileDescriptor, q);
    get_status_from_config(fileDescriptor, c,vetor_instances_original);
}

void close_server(int signum){
    unlink(FIFO_NAME);
    kill(0,signum);
    kill(getpid(), signum);
    _exit(3);
}


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

    
    if((mkfifo(FIFO_NAME, 0666)) < 0){
        perror("Erro a criar o fifo do servidor");
    }
    int fifo_fd, fifo_fd_write;

    if((fifo_fd = open(FIFO_NAME, O_RDONLY)) < 0){
        perror("Erro a abrir o descritor de comunicação com clientes");
    }

    if((fifo_fd_write = open(FIFO_NAME, O_WRONLY)) < 0){
        perror("Erro a abrir o descritor de sobrevivência do servidor");
    }

    //Array dinâmico com todos os binarios a executar
    char** binaries_to_execute = NULL;

    //buffer para ler e escrever
    char buffer_from_fifo[BUFFERSIZE];
    ssize_t bytes_read;

    while((bytes_read = readln(fifo_fd,buffer_from_fifo,BUFFERSIZE)) > 0){
        char* buffer_copy = strdup(buffer_from_fifo); //necessário fazer esta cópia porque o buffer_from_fifo vai ficar destruído depois do parse
        buffer_copy[bytes_read-1] = '\0';

        char* client_pid = strdup(strsep(&buffer_copy, " "));
        char* operation_mode = strdup(strsep(&buffer_copy, " "));

        if(strcmp(client_pid, "1") == 0){
            char* real_client_pid = strdup(operation_mode);
            free(real_client_pid);

            char* inputfile = strdup(strsep(&buffer_copy, " "));
            char* outputfile= strdup(strsep(&buffer_copy, " "));
            free(inputfile); 
            free(outputfile);
            
            int number_of_commands = get_binaries_num(buffer_copy);

            binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);

            request_out(c,binaries_to_execute,number_of_commands); 

            if(!isEmpty(q)){
                if(canExecuteBinaries(c, q->inicio->binaries_to_execute, q->inicio->binaries_num)){
                    request_enter(c,q->inicio->binaries_to_execute,q->inicio->binaries_num);
                    char* input = strdup(q->inicio->file_input);
                    char* output = strdup(q->inicio->file_output);
                    char** bins = arrayStrings_Copy(q->inicio->binaries_to_execute,q->inicio->binaries_num);
                    int num = q->inicio->binaries_num;
                    char* qID = inttoString(q->inicio->id);
                    char* request = malloc(3);
                    strcpy(request, "1 ");
                    my_strcat(request, qID);
                    my_strcat(request, " ");
                    my_strcat(request, input);
                    my_strcat(request, " ");
                    my_strcat(request, output);
                    my_strcat(request, " ");
                    for(int i = 0; i < num; i++){
                        my_strcat(request, bins[i]);
                        if(i != num- 1)
                            my_strcat(request, " ");
                    }
                        
                    my_strcat(request, "\n");
                    remove_task(q);
                    if(fork() == 0){
                        int task_client;
                        if((task_client= open(qID, O_WRONLY)) < 0){
                            perror("Erro a abrir task_client");
                        }
                        write(task_client, "processing\n", strlen("processing\n"));
                        if(fork() == 0){
                            sleep(10);
                            if(execute_commands_in_pipeline(c,input,output,bins,num) != 0){ 
                                perror("Erro a efetuar a execução da pipeline dos binários");
                            }
                            _exit(0);
                        }
                        wait(NULL);
                        write(task_client, "concluded\n", strlen("concluded\n"));     
                        close(task_client);
                        write(fifo_fd_write, request, strlen(request));                           
                        _exit(0);
                    }
                }
            }
        }
        else{
            if(strcmp(client_pid, "0") == 0){
                char* real_client_pid = strdup(operation_mode);

                char* inputfile = strdup(strsep(&buffer_copy, " "));
                char* outputfile= strdup(strsep(&buffer_copy, " "));
                free(inputfile); 
                free(outputfile);

                int number_of_commands = get_binaries_num(buffer_copy);
                binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);
                request_out(c,binaries_to_execute,number_of_commands); 

                int real_client;
                if((real_client= open(real_client_pid, O_WRONLY)) < 0){
                    perror("Erro a abrir real_client");
                }
                write(real_client, "concluded\n", strlen("concluded\n"));
                close(real_client);
                if(!isEmpty(q)){
                    if(canExecuteBinaries(c, q->inicio->binaries_to_execute, q->inicio->binaries_num)){
                        request_enter(c,q->inicio->binaries_to_execute,q->inicio->binaries_num);
                        char* inputs1 = strdup(q->inicio->file_input);
                        char* outputs1 = strdup(q->inicio->file_output);
                        char** bins1 = arrayStrings_Copy(q->inicio->binaries_to_execute,q->inicio->binaries_num);
                        int num1 = q->inicio->binaries_num;
                        char* qID1 = inttoString(q->inicio->id);
                        char* request = malloc(3);
                        strcpy(request, "1 ");
                        my_strcat(request, qID1);
                        my_strcat(request, " ");
                        my_strcat(request, inputs1);
                        my_strcat(request, " ");
                        my_strcat(request, outputs1);
                        my_strcat(request, " ");

                        for(int i = 0; i < num1; i++){
                            my_strcat(request, bins1[i]);
                            if(i != num1- 1)
                                my_strcat(request, " ");
                        }
                        
                        my_strcat(request, "\n");
                        remove_task(q);

                        if(fork() == 0){
                            int task_client;
                            if((task_client= open(qID1, O_WRONLY)) < 0){
                                perror("Erro a abrir task_client");
                            }
                            write(task_client, "processing\n", strlen("processing\n"));
                            if(fork() == 0){
                                sleep(10);
                                if(execute_commands_in_pipeline(c,inputs1,outputs1,bins1,num1) != 0){ 
                                    perror("Erro a efetuar a execução da pipeline dos binários");
                                }
                                _exit(0);
                            }
                            wait(NULL);

                            write(task_client, "concluded\n", strlen("concluded\n"));     
                            write(fifo_fd_write, request, strlen(request));       
                            _exit(0);                    

                        }
                    }

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
                        close(client_write);
                        close(fifo_fd_write);
                        close(fifo_fd);
                        exit(0);
                    }
                    close(client_write);
                }
                //Execução do comando "proc-file"
                if(strcmp(operation_mode, "P") == 0){
                    char* request = malloc(sizeof(char));
                    my_strcat(request, "0 ");
                    my_strcat(request, client_pid);
                    my_strcat(request, " ");
                    my_strcat(request,buffer_copy);
                    my_strcat(request,"\n");

                    char* inputfile = strdup(strsep(&buffer_copy, " "));
                    char* outputfile= strdup(strsep(&buffer_copy, " "));
                    int number_of_commands = get_binaries_num(buffer_copy);

                    binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);
                    
                    if(canExecuteBinaries(c, binaries_to_execute, number_of_commands)==1){
                        request_enter(c,binaries_to_execute,number_of_commands);

                        if(fork() == 0){
                            if(fork()==0){
                                write(client_write, "processing\n", strlen("processing\n"));
                                sleep(10);
                                if(execute_commands_in_pipeline(c, inputfile,outputfile,binaries_to_execute,number_of_commands) != 0){ 
                                    perror("Erro a efetuar a execução da pipeline dos binários");
                                }
                                _exit(0);
                            }
                            wait(NULL);

                            write(fifo_fd_write, request, strlen(request));  
                            _exit(0);                  
                        }
                    }
                    else{
                        char* message = "pending\n";
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
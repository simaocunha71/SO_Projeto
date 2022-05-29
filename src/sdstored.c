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
    while((bytes_read = readln(fifo_fd,buffer_from_fifo,BUFFERSIZE)) > 0){
        write(original_stdout, buffer_from_fifo, strlen(buffer_from_fifo));
        //write(original_stdout, "\n", 1);
        write(original_stdout ,"chupamos\n", strlen("chupamos\n"));
        
        //pid proc-file ficheiro1 ficheiro2 comandos


        char* buffer_copy = strdup(buffer_from_fifo); //necessário fazer esta cópia porque o buffer_from_fifo vai ficar destruído depois do parse
        buffer_copy[bytes_read-1] = '\0';
        write(original_stdout,buffer_copy, strlen(buffer_copy));

        char* client_pid = strdup(strsep(&buffer_copy, " "));
        write(original_stdout, client_pid, strlen(client_pid));
        char* operation_mode = strdup(strsep(&buffer_copy, " "));
        write(original_stdout, operation_mode, strlen(operation_mode));
        write(original_stdout, "acabou\n", strlen("acabou\n"));

        if(strcmp(client_pid, "1") == 0){
            write(original_stdout, "\n cliente pid e 1\n", strlen("\n cliente pid e 1\n"));
            char* real_client_pid = strdup(operation_mode);
            free(real_client_pid);
            write(original_stdout, "AAA1\n", strlen("AAA1\n"));
            char* inputfile = strdup(strsep(&buffer_copy, " "));
            write(original_stdout, "AAA2\n", strlen("AAA1\n"));

            char* outputfile= strdup(strsep(&buffer_copy, " "));
            write(original_stdout, "AAA3\n", strlen("AAA1\n"));

            free(inputfile); 
            free(outputfile);
            int number_of_commands = get_binaries_num(buffer_copy);
            write(original_stdout, "AAA4\n", strlen("AAA1\n"));

            binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);
            write(original_stdout, "AAA5\n", strlen("AAA1\n"));

            request_out(c,binaries_to_execute,number_of_commands); 
            write(original_stdout, "fez request ou \n", strlen("fez request ou \n"));

            
            

            if(!isEmpty(q)){
                if(canExecuteBinaries(c, q->inicio->binaries_to_execute, q->inicio->binaries_num)){
                    write(original_stdout, "\nchegou1\n", strlen("\nchegou1\n"));
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
                    write(original_stdout, request, strlen(request));

                    
                    if(fork()==0){
                        int task_client;
                        if((task_client= open(qID, O_WRONLY)) < 0){
                            perror("real_client_pid open error");
                        }
                        write(task_client, "executing from mode 1...\n", strlen("executing from mode 1....\n"));
                        if(fork()==0){
                            sleep(10);
                            if(execute_commands_in_pipeline(c,input,output,bins,num) != 0){ 
                                perror("Erro a efetuar a execução da pipeline dos binários");
                            }
                            _exit(0);
                        }
                        wait(NULL);
                        write(task_client, "done!", strlen("done!"));     
                        write(fifo_fd_write, request, strlen(request));                           
                        _exit(0);
                    }
                }
            
            }
        }
        else{
            
            

            if(strcmp(client_pid, "0") == 0){
                write(original_stdout, "\nmode 0:entrou\n", strlen("\nmode 0:entrou\n"));
                //0 pid proc-file ficheiro1 ficheiro2 comandos.
                //Como o servidor vai mandar algo do genero 0 PidDoCliente Ficheiros Comandos
                char* real_client_pid = strdup(operation_mode);
                char* inputfile = strdup(strsep(&buffer_copy, " "));
                char* outputfile= strdup(strsep(&buffer_copy, " "));
                free(inputfile); 
                free(outputfile);
                int number_of_commands = get_binaries_num(buffer_copy);
                binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);
                request_out(c,binaries_to_execute,number_of_commands); 
                //Parse dos binários a executar
                int real_client;
                if((real_client= open(real_client_pid, O_WRONLY)) < 0){
                    perror("real_client_pid open error");
                }
                write(real_client, "done!", strlen("done!"));
                close(real_client);
                if(!isEmpty(q)){
                    write(original_stdout, "\nmode 0:entrou na isEmpty\n", strlen("\nmode 0:entrou na isEmpty\n"));
                    if(canExecuteBinaries(c, q->inicio->binaries_to_execute, q->inicio->binaries_num)){
                        write(original_stdout, "\nmode 0:entrou na CanExec\n", strlen("\nmode 0:entrou na CanExec\n"));
                        request_enter(c,q->inicio->binaries_to_execute,q->inicio->binaries_num);
                        char* inputs1 = strdup(q->inicio->file_input);
                        write(original_stdout, inputs1, strlen(inputs1));
                        char* outputs1 = strdup(q->inicio->file_output);
                        char** bins1 = arrayStrings_Copy(q->inicio->binaries_to_execute,q->inicio->binaries_num);
                        int num1 = q->inicio->binaries_num;
                        char* qID1 = inttoString(q->inicio->id);
                        char* request = malloc(3);
                        strcpy(request, "1 ");
                        my_strcat(request, qID1);
                        my_strcat(request, " ");
                        my_strcat(request, inputs1);
                        write(original_stdout, "breakou1?\n", strlen("breakou1?\n"));
                        my_strcat(request, " ");
                        my_strcat(request, outputs1);
                        my_strcat(request, " ");

                        write(original_stdout, "breakou2?\n", strlen("breakou1?\n"));
                        for(int i = 0; i < num1; i++){
                            my_strcat(request, bins1[i]);
                            if(i != num1- 1)
                                my_strcat(request, " ");
                        }
                        
                        my_strcat(request, "\n");
                        write(original_stdout, "breakou3?\n", strlen("breakou1?\n"));
                        remove_task(q);
                        write(original_stdout, "\nmode 0:entrou fez request_enter\n", strlen("\nmode 0:entrou fez request_enter\n"));
                        write(original_stdout, request, strlen(request));
                        if(fork()==0){
                            int task_client;
                            if((task_client= open(qID1, O_WRONLY)) < 0){
                                perror("real_client_pid open error");
                            }
                            write(task_client, "executing from mode 0....\n", strlen("executing from mode 0....\n"));
                            if(fork()==0){
                                sleep(10);
                                if(execute_commands_in_pipeline(c,inputs1,outputs1,bins1,num1) != 0){ 
                                    perror("Erro a efetuar a execução da pipeline dos binários");
                                }
                                _exit(0);
                            }
                            wait(NULL);

                            write(task_client, "done!", strlen("done!"));     
                            write(fifo_fd_write, request, strlen(request));       
                            _exit(0);                    

                        }
                    }

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
                    write(original_stdout, "entrou no status\n", strlen("entrou no status\n"));
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
                    //char* request = malloc(strlen(buffer_copy)+ strlen(client_pid)+4);
                    char* request = malloc(sizeof(char));
                    my_strcat(request, "0 ");
                    my_strcat(request, client_pid);
                    my_strcat(request, " ");
                     write(original_stdout,"\nBuffer Copy: ",strlen("\nBuffer Copy: "));
                     write(original_stdout,buffer_copy,strlen(buffer_copy));
                     write(original_stdout,"\n",strlen("\n"));
                    my_strcat(request,buffer_copy);

                    my_strcat(request,"\n");
                    char* inputfile = strdup(strsep(&buffer_copy, " "));
                    char* outputfile= strdup(strsep(&buffer_copy, " "));
                    int number_of_commands = get_binaries_num(buffer_copy);
                    //Parse dos binários a executar
                    write(original_stdout, buffer_copy, strlen(buffer_copy));
                    printf("\n buffer copy ^, number of comands %d\n", number_of_commands);
                    binaries_to_execute = create_binaries_array(buffer_copy, number_of_commands);
                    write(original_stdout, request, strlen(request));
                    
                    if(canExecuteBinaries(c, binaries_to_execute, number_of_commands)==1){
                        request_enter(c,binaries_to_execute,number_of_commands);
                        write(original_stdout,"\nBuffer Copy 2nd: ",strlen("\nBuffer Copy 2nd: "));
                        write(original_stdout,buffer_copy,strlen(buffer_copy));
                        write(original_stdout,"\n",strlen("\n"));
                        if(fork()==0){
                            char* executing_message = "executing from mode proc....\n";
                            write(client_write, executing_message, strlen(executing_message)+1);
                            write(original_stdout, "\na executar\n", strlen("\na executar\n"));
                            //printConfigs(c);

                                if(fork()==0){
                                write(original_stdout, "segfault4?\n", strlen("segfault1?\n"));
                                    sleep(10);
                                    if(execute_commands_in_pipeline(c, inputfile,outputfile,binaries_to_execute,number_of_commands) != 0){ 
                                        perror("Erro a efetuar a execução da pipeline dos binários");
                                    }
                            write(original_stdout, "segfault5?\n", strlen("segfault1?\n"));
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
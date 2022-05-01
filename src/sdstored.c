#include "includes/sdstored.h"

// Servidor


CONFIG start_server(char* config_filename, char* binarys_folder){
    CONFIG c = load_configurations(config_filename, binarys_folder);
    printConfigs(c);
    return c;
}


void printConfigs(CONFIG c){
    while(c != NULL){
        printf("Executável: %s | Nº instâncias máximas: %d | Path do executável: %s\n",c->binary_name, c->max_instances, c->path_name);
        c=c->next;
    }
}

void statusfunction(){

}

int execcommands(int nc,char **cmd, char* path, char* input, char* output){
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
    int stdout = dup(1);
    //mudamos o input default pra o nosso ficheiro de input
    dup2(input_fd, 0);
    //mudamos o output default pra o nosso ficheiro output
    dup2(output_fd,1);
    //fechamos o input e o output pq os execs vao usar o 0 e 1
    close(input_fd);
    close(output_fd);

    //criamos os pipes necessarios pra executar todos os comandos
    int p[nc-1][2];
    int i,status;
    nc--;
    //caso so haja um comando
    if(nc == 1){
        if(fork()==0){

        mystrcat(path, cmd[0]);
        execl(path, cmd[0], NULL);
        _exit(0);
        }
        wait(NULL);
    }
    else{

    for (i=0; i<nc; i++){
      if (i==0){
        pipe(p[i]);
        if (fork()==0){
          close(p[i][0]);
          dup2(p[i][1],1);
          close(p[i][1]);
          mystrcat(path, cmd[i]);
          //write(stdout,path,strlen(path));
          execl(path, cmd[i], NULL);
          _exit(0);
        }
        else close(p[i][1]);
      }
      else if (i==nc-1){
        if (fork()==0){
          dup2(p[i-1][0],0);
          close(p[i-1][0]);
          mystrcat(path, cmd[i]);
          execl(path, cmd[i], NULL);
          _exit(0);
        }
        else close(p[i-1][0]);
      }
      else {
        pipe(p[i]);
        if (fork()==0){
          close(p[i][0]);
          dup2(p[i][1],1);
          close(p[i][1]);
          dup2(p[i-1][0],0);
          close(p[i-1][0]);
          mystrcat(path, cmd[i]);
          execl(path, cmd[i], NULL);
          _exit(0);
        }
        else {
          close(p[i][1]);
          close(p[i-1][0]);
        }
      }
    }
    for (i=0; i<nc; i++) wait(&status);
    }
    return 0;
}

int sendtoclient(int nr){
    return 0;

}

//./sdstored bin/sdstore.conf obj/
int main(int argc, char const *argv[]){
    //char* filename_config = (char*) argv[1];
    char* binarys_folder = (char*) argv[2];


    //CONFIG c = start_server(filename_config, binarys_folder);
    //execute_config(c,"bdecompress","test_files/bfile.bz2","test_files/new.csv");


    //pegar no maximo de instrucoes possiveis ao adicionar td da lista ligada
    //por agora fazemos com 2000
    int maxinst= 2000;


    //abre o pipe com nome pra estabelecer a ligaçao do server
    if((mkfifo("comunication", 0666)) <0){
        perror("mkfifo");
    }
    int fifo_fd , fifo_fd_write;

    //descritor de comunicação para ler o que o cliente vai enviar
    if((fifo_fd=open("comunication", O_RDONLY))<0){
        perror("open fifo_fd");
    }

    //para o server estar sempre a funcionar, temos este descritor SEMPRE aberto (nunca o vamos usar)
    if((fifo_fd_write=open("comunication", O_WRONLY))<0){
        perror("open fifo_fd_write");
    }


    //meter os comandos aqui
    char* commands[maxinst];

    //buffer para ler e escrever
    char *buffer1=malloc(1024);
    //bytes_read when using read
    ssize_t bytes_read;

    //ciclo infinito que nunca devia ser quebrado, vai estar sempre a ler do pipe de comunicacao!
    

    while((bytes_read=read(fifo_fd,buffer1,1024))>0){
        char* buffer=strdup(buffer1);
        //string aonde vamos meter cenas
        // char* pipo=malloc(100);

        //clientpid para saber que pipe escrever para!
        char* clientpid = strdup(strsep(&buffer, " "));
        //write(1,buffer, strlen(buffer));
        //write(1,clientpid, strlen(clientpid));

        int client_write;

        //abrimos aqui o pipe para escrever para o cliente
        //criamos o pipe com o clientpid
        /*
        write(1,pipo,strlen(pipo));
        write(1,clientpid,strlen(clientpid));
        */
        
        if((client_write=open(clientpid, O_WRONLY))==-1){
            perror("client_write open");
        }
        //escrevemos para o cliente que o seu pedido está a penden-te
        write(client_write, "pending...\n", strlen("pending...\n"));
        /*
        fazer aqui o código que verifica se podemos executar o pedido do cliente ou se metemos esse pedido numa queue
        */
        //metemos no pipo o clientpid
        //mystrcat(pipo, clientpid);

        
        //se o pedido do cliente for status fazemos o que o status pede
        if(!strcmp(buffer, "status")){
            //esta função ainda não faz nada
            statusfunction();
        }
        else{


            //esta priorirty como ainda n ta implementada os comandos de execucao nao irao usar
            //logo so tirar de comentario isto quando for para implementar o priority
            //isso ira mudar um coto o codigo
            //char* priority = strdup(strsep(&buffer, " "));
            
            //input file mandado pelo cliente
            char* inputfile = strdup(strsep(&buffer, " "));
            //output file mandado pelo cliente
            char* outputfile= strdup(strsep(&buffer, " "));
            int i;
            //comandos
            for(i=0;i<maxinst && buffer; i++){
                //array de strings, cada uma com comandos
                //o i é o numero de comandos
                commands[i]=strdup(strsep(&buffer, " "));
            }
            
            //escrever ao cliente que vamos finalmente executar o pedido deles
            write(client_write, "executing...\n", strlen("executing...\n"));

            //funcao de execução de comandos
            if(fork()==0){
            if(execcommands(i, commands, strdup(binarys_folder), inputfile, outputfile)<0){
                perror("execcommands");
            }
            close(client_write);
            close(fifo_fd);
            close(fifo_fd_write);
            exit(0);
            }
            wait(NULL);
            

        }
        //escrevemos no cliente no cliente que concluímos o pedido deles
        write(client_write, "done!\n", strlen("done!\n"));
        //fechamos o pipe para aonde escrevemos o cliente
        close(client_write);
    }
    printf("\n%d\n",errno);
    //fechamos o comunication pipe quando paramos de ler cenas, que devia ser nunca que nao deveria ser possivel sair do ciclo while por agora
    close(fifo_fd);

/*
    int log_fd=open("log.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int fifo_fd , fifo_fd_write;

    fifo_fd=open("comunication", O_RDONLY);

    //server estar sempre a funfar
    fifo_fd_write=open("comunication", O_WRONLY);

        //read from pipe
            char buffer[1024];
            ssize_t bytes_read;
        while((bytes_read=read(fifo_fd,buffer,1024))>0){
            write(log_fd,buffer,bytes_read);
        }
        close(fifo_fd);

    


    
    printf("----MAIN----\nCF: %s\nBF: %s\n------------\n", filename_config,binarys_folder);

    
    //execute_config(c,"bcompress","test_files/file.csv","test_files/bfile.bz2");
    return 0;
    */

   //nunca vai chegar aqui, mas isto apaga o pipe de comunicação,
   //ou seja, quando implementarmos a cena de fechar o servidor direito, executamos no final este comando
   unlink("comunication");
}

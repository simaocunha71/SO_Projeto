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
    if((input_fd= open(input, O_RDONLY))==-1){
        perror("input_fd open");
    }
    if((output_fd=open(output, O_WRONLY | O_CREAT | O_TRUNC, 0640))==-1){
        perror("output_fd open");
    }
    printf("%d | %s | %s | %s\n", nc, path, input, output);
    int stdout = dup(1);
    dup2(input_fd, 0);
    dup2(output_fd,1);
    close(input_fd);
    close(output_fd);
    int p[nc-1][2];
    int i,status;
    nc--;
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

}

//./sdstored bin/sdstore.conf obj/
int main(int argc, char const *argv[]){
    char* filename_config = (char*) argv[1];
    char* binarys_folder = (char*) argv[2];


    //CONFIG c = start_server(filename_config, binarys_folder);
    //execute_config(c,"bdecompress","test_files/bfile.bz2","test_files/new.csv");


    //pegar no maximo de instrucoes possiveis ao adicionar td da lista ligada
    int maxinst= 2000;


    //abre o pipe com nome pra estabelecer a ligaçao do server
    if(mkfifo("comunication", 0666) <0){
        perror("mkfifo");
    }
    int fifo_fd , fifo_fd_write;

    fifo_fd=open("comunication", O_RDONLY);

    //server estar sempre a funfar
    fifo_fd_write=open("comunication", O_WRONLY);


    //meter os comandos aqui
    char* commands[maxinst];
     //read from pipe
    char *buffer=malloc(1024);
    ssize_t bytes_read;
    int j;
    while((bytes_read=read(fifo_fd,buffer,1024))>0){
        j=0;
        char* pipo=malloc(100);
        sprintf(pipo, "client%d", j);
        if(mkfifo(pipo, 0666)<0){
            perror("mkfifo client");
        }
        
        int client_write;
        if((client_write=open(pipo, O_WRONLY))==-1){
            perror("client_write open");
        }
        write(client_write, "pending...\n", sizeof("pending...\n"));
        if(!strcmp(buffer, "status")){
            statusfunction();
        }
        else{
            printf("%s\n",buffer);
           // char* priority = strdup(strsep(&buffer, " "));
            char* inputfile = strdup(strsep(&buffer, " "));
            char* outputfile= strdup(strsep(&buffer, " "));
            int i;
            for(i=0;i<maxinst && buffer; i++){
                commands[i]=strdup(strsep(&buffer, " "));
            }
            write(client_write, "executing...\n", sizeof("executing...\n"));
            if(execcommands(i, commands, strdup(binarys_folder), inputfile, outputfile)<0){
                perror("execcommands");
            }
            

        }
        write(client_write, "done!\n", sizeof("done!\n"));
        close(client_write);
        j++;
    }
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
   unlink("comunication");
}

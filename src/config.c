#include "includes/config.h"

CONFIG create_config (char* line, char* binarys_folder, CONFIG c){

    //Parse da linha do ficheiro de configuração
    char* name = strdup(strsep(&line," "));
    int num = atoi(strsep(&line,"\n"));

    if (validate_binary_to_execute(name) && num > 0){
        CONFIG new = malloc(sizeof(struct configuration));
        new->binary_name = strdup(name);
        new->max_instances = num;
        new->path_name = get_binary_filename(new->binary_name,binarys_folder);
        new->next = NULL;

        CONFIG aux = c;
        
        while (c!=NULL && c->next != NULL)
            c = c->next;
        
        if (c!=NULL){
            c->next = new;
            return aux;
        }
        else 
            return new;
    }

    return NULL;
}


CONFIG load_configurations (char* config_filename, char* binarys_folder){
    CONFIG new,c;
    new = c = NULL;
    int buf_size = 20;
    size_t bytes_read = 0;
    char buffer[buf_size];

    int fd;
    if((fd = open(config_filename, O_RDONLY, 0660)) == -1)
        perror("Erro a abrir o ficheiro de configuração");

    while ((bytes_read = readln(fd, buffer, buf_size)) > 0){
        new = create_config(buffer, binarys_folder, c);
        c = new;
    }

    close(fd);

    return c;
}

char* get_binary_filename (char* binary_name, char* binary_folder){
    char* res = (char*) malloc(strlen(binary_name) + strlen(binary_folder) + 1);
    strcat(res, binary_folder);
    strcat(res, binary_name);
    return res;
}

void execute_config (CONFIG cs, char* binary_name, char* file_to_use, char* new_file){
    CONFIG c = get_Config(binary_name,cs);

    //printf("GET:\n");
    //printf("> Nome: %s\n", c->binary_name);
    //printf("> Num: %d\n", c->max_instances);
    //printf("> Path: %s\n", c->path_name);
    if (c != NULL){
        redirection(file_to_use,0, 0);
        redirection(new_file,1, 1);
        char* command_files[] = {c->path_name, file_to_use, new_file, NULL}; 
        //printf("-> Path: %s\n", c->path_name);
        //printf("-> Input: %s\n", file_to_use);
        //printf("-> Output: %s\n", new_file);

        execvp(command_files[0], command_files);
        perror("Erro a executar o binário");

    }
}

CONFIG get_Config(char* binary_name, CONFIG cs){
    CONFIG c = malloc (sizeof (struct configuration));
    int found = 0;
    while(cs != NULL && found == 0){
        if(strcmp(binary_name, cs->binary_name) == 0){
            c->binary_name = strdup(cs->binary_name);
            c->max_instances = cs->max_instances;
            c->path_name = cs->path_name;
            c->next = NULL;
            found = 1;
        }
        cs = cs->next;
    }
    if (c != NULL)
        return c;
    else
        return NULL;
}

char* mystrcat(char* a, char* b){
    a = realloc(a, (strlen(a)+strlen(b))+1);
    strcat(a,b);
    return a;
}
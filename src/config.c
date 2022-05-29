#include "includes/config.h"

CONFIG create_config (char* line, char* binarys_folder, CONFIG c){

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

void execute_config (CONFIG cs, char* binary_name){
    CONFIG c = get_Config(binary_name,cs);
    
    if (c != NULL){
        execl(c->path_name,c->binary_name, NULL);
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
    return c;
}

void changeInstances(CONFIG c, char* binary, char* operation){
    if(strcmp(operation, "inc") == 0){
        while(c != NULL){
            if(strcmp(c->binary_name, binary) == 0)
                c->max_instances++;
            c = c->next;
        }
    }
    if(strcmp(operation, "dec") == 0){
        while(c != NULL){
            if(strcmp(c->binary_name, binary) == 0)
                c->max_instances--;
            c = c->next;
        }
    }    

}

int canExecuteBinaries(CONFIG c, char** binaries_array, int number_of_commands){

    CONFIG aux = copy_config(c);
    int size = size_array(c);
    int v[size];
    for(int i = 0; i < size; i++){
        v[i] = aux->max_instances;
        aux = aux->next;
    }
    free_config(aux);
    for(int i = 0; i < number_of_commands; i++){
        if(strcmp("nop", binaries_array[i]) == 0){
            v[0]--;
        }
        if(strcmp("bcompress", binaries_array[i]) == 0){
            v[1]--;
        }
        if(strcmp("bdecompress", binaries_array[i]) == 0){
            v[2]--;
        }
        if(strcmp("gcompress", binaries_array[i]) == 0){
            v[3]--;
        }
        if(strcmp("gdecompress", binaries_array[i]) == 0){
            v[4]--;
        }
        if(strcmp("encrypt", binaries_array[i]) == 0){
            v[5]--;
        }
        if(strcmp("decrypt", binaries_array[i]) == 0){
            v[6]--;
        }
    }

    int flag = 1;
    for(int i = 0; (i < size) && (flag == 1); i++){
        if(v[i] < 0){
            flag = 0;
        }    
    }
    return flag;
}

void request_enter(CONFIG cs, char** binaries_array, int number_of_binaries){
    int i;
    for(i = 0; i < number_of_binaries; i++) {
        changeInstances(cs,binaries_array[i],"dec");
    }
    
}

void request_out(CONFIG cs, char** binaries_array, int number_of_binaries){
    int i;
    for(i = 0; i < number_of_binaries; i++) {
        changeInstances(cs,binaries_array[i],"inc");
    }
}

int size_array(CONFIG c){
    int r = 0;
    while(c != NULL){
        r++;
        c=c->next;
    }
    return r;
}


int get_original_inst(int indice, int vetor_instances_original[]){
    return vetor_instances_original[indice];
}

void get_status_from_config(int fileDescriptor, CONFIG c, int vetor_instances_original[]){
    int indice = 0;
    if (c == NULL)
        write(fileDescriptor, "CONFIG NULL!\n ", strlen("CONFIG NULL\n"));
    while(c != NULL && indice < 7){
        
        write(fileDescriptor, "transf ", strlen("transf "));
        write(fileDescriptor, c->binary_name, strlen(c->binary_name));
        write(fileDescriptor, ": ", strlen(": "));
        write(fileDescriptor, inttoString(abs(get_original_inst(indice, vetor_instances_original) - c->max_instances)), 
                       strlen(inttoString(abs(get_original_inst(indice, vetor_instances_original) - c->max_instances))));
        write(fileDescriptor, "/", strlen("/"));
        write(fileDescriptor, inttoString(get_original_inst(indice, vetor_instances_original)), 
                       strlen(inttoString(get_original_inst(indice, vetor_instances_original))));
        write(fileDescriptor, " (running/max)\n", strlen(" (running/max)\n"));

        indice++;
        c=c->next;
    }
}

CONFIG copy_config (CONFIG c){
    CONFIG r = malloc(sizeof(struct configuration));
    if (c == NULL)
        return NULL;
    r->binary_name = strdup(c->binary_name);
    r->max_instances = c->max_instances;
    r->path_name = strdup(c->path_name);
    r->next = copy_config(c->next);
    return r;
}

void free_config (CONFIG c){
    CONFIG temp;
    while(c != NULL){
        temp = c;
        c=c->next;
        free(temp);
    }
}
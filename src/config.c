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

void execute_config (CONFIG cs, char* binary_name){
    CONFIG c = get_Config(binary_name,cs);
    
    if (c != NULL){
        changeInstances(cs,binary_name,"dec");
        execl(c->path_name,c->binary_name, NULL);
        perror("Erro a executar o binário");

    }
}

//Talvez diminuir ao nº de instancias para fazer algum controlo de binarios nesta funçao
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
    int flag = 1;
    for(int i = 0; (i < number_of_commands) && (flag == 1); i++){
        if(match_binary_with_instances(c, binaries_array[i]) == 0){
            flag = 0;
        }    
    }
    return flag;
}

int match_binary_with_instances(CONFIG cs, char* binary_name){
    CONFIG c = get_Config(binary_name,cs);
    return c->max_instances > 0;
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

void print_num(CONFIG c){
    while (c != NULL)
    {
        printf("%s : %d\n", c->binary_name, c->max_instances);
        c = c->next;
    }
}

/*
int main(){
    CONFIG c = load_configurations("../bin/sdstore.conf","../obj/");
    
    printf("Instancias antes:\n");

    print_num(c);

    char* args = "nop nop bcompress nop bdecompress";
    int num = get_binaries_num(args);
    char** bin = NULL;
    bin = create_binaries_array(args,num);

    request_enter(c,bin,num);

    printf("Instancias depois de entrar:\n");

    print_num(c);

    printf("Pode entrar mais pedidos?: %d\n",canExecuteBinaries(c,bin,num));

    request_out(c,bin,num);

    printf("Instancias depois de sair:\n");

    print_num(c);

    printf("Pode entrar mais pedidos?: %d\n",canExecuteBinaries(c,bin,num));

    return 0;
}
*/
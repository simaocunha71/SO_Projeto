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
        printf("%s -> %d\n", c->binary_name, c->max_instances);
        c = c->next;
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

int* create_array_copy_of_instances(CONFIG c){
    int size = size_array(c);
    int* v = malloc(sizeof(int) * size);
    for(int i = 0; c != NULL && i < size; i++){
        v[i] = c->max_instances;
        c=c->next;
    }
    return v;
}

int get_original_inst(int indice, int vetor_instances_original[]){
    return vetor_instances_original[indice];
}

char* get_status_from_config(CONFIG c, int vetor_instances_original[]){
    char* r = malloc(sizeof(char));
    int indice = 0;
    while(c != NULL){
        my_strcat(r,"transf ");
        my_strcat(r,c->binary_name);
        my_strcat(r,": ");
        my_strcat(r,inttoString(abs(get_original_inst(indice, vetor_instances_original) - c->max_instances))); //instancias a decorrer
        my_strcat(r,"/");
        my_strcat(r,inttoString(c->max_instances));
        my_strcat(r," (running/max)\n");
        //printf("R:: %s\n", r); CORRIGIR ISTO
        indice++;
        c=c->next;
    }
    
    return r;
}

/*
int main(){
    CONFIG c = load_configurations("../bin/sdstore.conf","../obj/");

     Queue q = init_queue();

    
    printf("Instancias antes:\n");

    print_num(c);

    printf("\n");

    char* args = "nop nop bcompress nop bdecompress";
    int num = get_binaries_num(args);
    char** bin = NULL;
    bin = create_binaries_array(args,num);

    request_enter(c,bin,num);

    printf("Instancias depois de entrar:\n");

    print_num(c);

    printf("\n");

    printf("Pode entrar mais pedidos?: %d\n",canExecuteBinaries(c,bin,num));

    printf("\n");

    if (canExecuteBinaries(c, bin, num) == 0){
        add_task(q,"Input debug","Output Debug",bin,num);
    }

    //printQueue(q);

    printf("\n");

    request_out(c,bin,num);

    printf("Instancias depois de sair:\n");

    print_num(c);

    printf("\n");

    printf("Pode entrar mais pedidos?: %d\n",canExecuteBinaries(c,bin,num));

    printf("\n");

    if (canExecuteBinaries(c,bin,num) == 1)
    {
        remove_task(q);
    }

    //printQueue(q);

    printf("\n");
    

    return 0;
}
*/
#include "includes/configuration.h"

CONFIG create_config (char* line){
    CONFIG c;
    char name[20];
    int num;

    sscanf(line, "%s %d", name, &num);
    if (validate_binary_to_execute(name) && num > 0){
        c.binary_name = strdup(name);
        c.max_instances = num;
    }
    else
        c.binary_name = "";
    
    return c;
}

CONFIGURATIONS load_configurations (char* config_filename){
    CONFIGURATIONS cs = malloc(sizeof(struct configurations));
    int i = 0, buf_size = 20;
    char buffer[buf_size];
    
    int fd = open(config_filename, O_RDONLY, 0660);


    while (i < MAX_CONFIGS){
        readln(fd, buffer, buf_size);
        CONFIG c = create_config(buffer);
        if (strcmp(c.binary_name, "") != 0){
            cs->config[i] = c;
            i++;
        }
    }

    close(fd);
    return cs;
}

void execute_config (CONFIG c, const char* file_to_use, const char* new_file){
    char binary_to_execute[strlen("../obj/") + strlen(c.binary_name) + 1];
    strcat(binary_to_execute, "../obj/");
    strcat(binary_to_execute, c.binary_name);

    redirection((char*)file_to_use,0, 0);
    redirection((char*)new_file,1, 1);
    char* command_files[] = {(char*)binary_to_execute, (char*)file_to_use, (char*)new_file, NULL};

    int e = execvp(command_files[0], command_files);
    if(e == -1)
        perror("Error");
    else 
        printf("Success\n");

}

//DEBUG
void printConfigs (CONFIGURATIONS cs){
    int i;
    for (i = 0; i < MAX_CONFIGS; i++){
        printf("Nome: %s | MAX: %d\n", cs->config[i].binary_name, cs->config[i].max_instances);
    }
}

//DEBUG
int main(int argc, char const *argv[]){
    CONFIGURATIONS cs = load_configurations("../bin/sdstore.conf");
    printConfigs(cs);
    //execute_config(cs->config[1], "../test_files/file.csv", "../test_files/newFile.bz2");
    //execute_config(cs->config[2], "../test_files/newFile.bz2", "../test_files/reverted.csv");
    
    return 0;
}

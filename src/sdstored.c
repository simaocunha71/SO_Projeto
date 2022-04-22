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


//./sdstored bin/sdstore.conf obj/
int main(int argc, char const *argv[]){
    char* filename_config = (char*) argv[1];
    char* binarys_folder = (char*) argv[2];

    printf("----MAIN----\nCF: %s\nBF: %s\n------------\n", filename_config,binarys_folder);

    CONFIG c = start_server(filename_config, binarys_folder);
    //execute_config(c,"bcompress","test_files/file.csv","test_files/bfile.bz2");
    execute_config(c,"bdecompress","test_files/bfile.bz2","test_files/new.csv");
    return 0;
}

#include "includes/utilities.h"

size_t readln(int fd, char *line, size_t size){
    size_t i;
    for (i = 0; i < size && (read(fd, line, 1)) > 0; i++) {
        if (*line=='\n') {
            i++;
            break;
        }
        line++;
    }
    return i;
}

int validate_binary_to_execute (char* binary_to_execute){
    return strcmp(binary_to_execute,"bcompress")   == 0 ||
           strcmp(binary_to_execute,"bdecompress") == 0 ||
           strcmp(binary_to_execute,"decrypt")     == 0 ||
           strcmp(binary_to_execute,"encrypt")     == 0 ||
           strcmp(binary_to_execute,"gcompress")   == 0 ||
           strcmp(binary_to_execute,"gdecompress") == 0 ||
           strcmp(binary_to_execute,"nop")         == 0;
}

char* inttoString(int n){
    char* string = malloc(sizeof(char) * 50);
    sprintf(string, "%d", n);
    return string;
}

int get_binaries_num(char* buffer){
    int r = 0;
    for(int i = 0; buffer[i] != '\0'; i++)
        if(buffer[i] == ' ')
            r++;
    return r+1;
}

char** create_binaries_array(char* buff, int number_of_binaries){
    char* buffer = strdup(buff);
    char** results = malloc(number_of_binaries * sizeof(char*));
    int i;
    for(i = 0; i < number_of_binaries - 1 && buffer != NULL; i++){
        results[i] = strdup(strsep(&buffer, " "));
    }
    results[i] = strdup(strsep(&buffer, "\0"));
    return results;
}

void my_strcat(char* a, char* b){
    a = realloc(a, (strlen(a)+strlen(b))+1);
    strcat(a,b);
}
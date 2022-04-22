#include "includes/utilities.h"

size_t readln(int fd, char* line, size_t size){
    ssize_t bytes_read = read(fd, line, size);
    size_t line_length = strcspn(line, "\n") + 1; 
    if(bytes_read < line_length) 
		line_length = bytes_read;
    line[line_length] = 0;
    lseek(fd, line_length - bytes_read, SEEK_CUR);
    return bytes_read;
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

void redirection (char* filename, int std, int isInput){
    int file;
    if(isInput == 0)
        file = open(filename, O_RDONLY, 0640);
    else
        file = open(filename, O_RDWR  | O_CREAT | O_TRUNC, 0640);
    
    dup2(file, std);
    close(file);
}
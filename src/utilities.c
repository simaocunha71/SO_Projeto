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
    if(isInput == 0){
        if((file = open(filename, O_RDONLY, 0640))==-1)
            perror("Erro a abrir o ficheiro de input");
    }
    else{
        if((file = open(filename, O_RDWR  | O_CREAT | O_TRUNC, 0640))==-1)
            perror("Erro a abrir o ficheiro de output");
    }
    
    dup2(file, std);
    close(file);
}

char* inttoString(int n){
   char* string = malloc(sizeof(int));
   if(n == 0) { 
       string[0] = '0'; 
       return "\0";
   };
   int modResult;
   int length = 0;
   int isNegative = 0;
   int copyOfNumber = n;

   if(n < 0) {
     isNegative = 1;
     n = 0 - n;
     length++;
   }

   while(copyOfNumber != 0){ 
       length++;
       copyOfNumber /= 10;
   }

   for(int divide = 0; divide < length; divide++) {
     modResult = n % 10;
     n = n / 10;
     string[length - (divide + 1)] = modResult + '0';
   }
   if(isNegative)
       string[0] = '-';
   string[length] = '\0';

   return string;
}

int get_binaries_num(char* buffer){
    int r = 0;
    for(int i = 0; buffer[i] != '\0'; i++)
        if(buffer[i] == ' ')
            r++;
    return r+1;
}

char** create_binaries_array(char* buff){
    char* buffer = strdup(buff);
    int number_of_binaries = get_binaries_num(buffer);
    char** results = malloc(number_of_binaries * sizeof(char*));
    for(int i = 0; i < number_of_binaries && buffer != NULL; i++){
        results[i] = strdup(strsep(&buffer, " "));
        printf("Results %d-> %s\n",i, results[i]);
    }
    return results;
}
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

char** add_string_to_array(char** array_of_strings, const char* string_to_add){
    int num_strings_already_existing = 0;
    while (array_of_strings[num_strings_already_existing] != NULL){        
        num_strings_already_existing++;
    }
    array_of_strings[num_strings_already_existing] = strdup(string_to_add);
    strcpy(array_of_strings[num_strings_already_existing], string_to_add);
    char** aux = realloc(array_of_strings, (num_strings_already_existing+2) * sizeof(char *));
    if(aux == NULL)
        perror("Erro a fazer realloc na adição de strings a arrays");
    else {
        array_of_strings = aux;
        array_of_strings[num_strings_already_existing+1] = NULL;
    }
    return array_of_strings;
}
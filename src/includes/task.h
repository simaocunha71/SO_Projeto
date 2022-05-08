#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    int id;
    char* file_input;
    char* file_output;
    char** binaries_to_execute;
    int binaries_num;
    struct task* prox;
} *TASK;

typedef struct queue {
    struct task* inicio , *fim ;
} *Queue;


Queue init_queue();

void add_task (Queue q, char* file_input, char* file_output, char** binaries_to_execute, int number_of_binaries);

void remove_task (Queue q);

void freeArrayList(char** arrayStrings, int size);

void printQueue (Queue q);

char** arrayStrings_Copy (char** original, int array_size);
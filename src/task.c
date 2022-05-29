#include "includes/task.h"

Queue init_queue(){
    Queue q = malloc(sizeof(struct queue));
    q->inicio = NULL;
    q->fim = NULL;
    return q;
}

int isEmpty (Queue q){
    if(q) return (q->inicio == NULL);
    return (q == NULL);
}

void add_task (Queue q, char* file_input, char* file_output, char** binaries_to_execute, int number_of_binaries, int client_pid){
    TASK new = malloc (sizeof(struct task));
    if (new != NULL){
        new->id = client_pid;
        new->file_input = strdup(file_input);
        new->file_output = strdup(file_output);
        new->binaries_num = number_of_binaries;
        new->binaries_to_execute = arrayStrings_Copy(binaries_to_execute, number_of_binaries);
        new->prox = NULL;
        if (q->inicio == NULL){
            q->inicio = new;
            q->fim = new;
        }
        else{
            q->fim->prox = new;
            q->fim = q->fim->prox;
        }  
    }
}


void remove_task (Queue q){
    if (q->inicio != NULL){
        struct task * temp;
        temp = (q)->inicio;
        (q)->inicio = temp->prox;
        if (q->fim == q->inicio) {
            q->fim = NULL; 
        }
        free(temp->file_input);
        free(temp->file_output);
        freeArrayList(temp->binaries_to_execute, temp->binaries_num); 
    }
}

void freeArrayList(char** arrayStrings, int size){
    for(int i = 0; i < size; i++){
        free(arrayStrings[i]);
    }
}

char** arrayStrings_Copy (char** original, int array_size){
    char** r = malloc(array_size * sizeof(char*));
    for(int i = 0; i < array_size; i++){
        r[i] = strdup(original[i]);
    }
    return r;
}

void get_status_from_queue(int fileDescriptor, Queue q){
    TASK t = q->inicio;
    if(t == NULL)
        write(fileDescriptor, "Empty queue!\n", strlen("Empty queue!\n"));
    while(t != NULL){
        write(fileDescriptor, "task #", strlen("task #"));
        write(fileDescriptor, inttoString(t->id), strlen(inttoString(t->id)));
        write(fileDescriptor, ": proc-file ", strlen(": proc-file "));
        write(fileDescriptor, t->file_input, strlen(t->file_input));
        write(fileDescriptor, " ", strlen(" "));
        write(fileDescriptor, t->file_output, strlen(t->file_output));
        write(fileDescriptor, " ", strlen(" "));
        for(int i = 0; i < t->binaries_num; i++){
            if(i < t->binaries_num - 1){
                write(fileDescriptor, t->binaries_to_execute[i], strlen(t->binaries_to_execute[i]));
                write(fileDescriptor, " ", strlen(" "));
            }
            else
                write(fileDescriptor, t->binaries_to_execute[i], strlen(t->binaries_to_execute[i]));
        }
        write(fileDescriptor, "\n", strlen("\n"));
        
        t=t->prox;        
    }
}


#include "includes/task.h"

int task_id = 1;

Queue init_queue(){
    Queue q = malloc(sizeof(struct queue));
    q->inicio = NULL;
    q->fim = NULL;
    return q;
}
int isEmpty (Queue q){
    return (q->inicio==NULL);
}

void add_task (Queue q, char* file_input, char* file_output, char** binaries_to_execute, int number_of_binaries){
    TASK new = malloc (sizeof(struct task));
    if (new != NULL){
        new->id = task_id;
        task_id++;
        new->file_input = strdup(file_input);
        new->file_output = strdup(file_output);
        new->binaries_num = number_of_binaries;
        new->binaries_to_execute = arrayStrings_Copy(binaries_to_execute, number_of_binaries);
        new->prox = NULL;

        if (q->fim == NULL){
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
        struct task *temp;
        temp = q->inicio;
        q->inicio = q->inicio->prox;
        free(temp->file_input);
        free(temp->file_output);
        freeArrayList(temp->binaries_to_execute, temp->binaries_num); 
    }
}

void freeArrayList(char** arrayStrings, int size){
    for(int i = 0; i < size; i++){
        free(arrayStrings[i]);
        //printf("COPY: %s\n", r[i]);
    }
}

char** arrayStrings_Copy (char** original, int array_size){
    char** r = malloc(array_size * sizeof(char*));
    for(int i = 0; i < array_size; i++){
        r[i] = strdup(original[i]);
    }
    return r;
}

void printQueue (Queue q){
    TASK t = q->inicio;
    if(t == NULL)
        printf("Task nula!\n");
    while(t != NULL){
        printf("Task #%d\n", t->id);
        printf("> %s\n", t->file_input);
        printf("> %s\n", t->file_output);
        printf("> Array de binarios a executar: {");
        for(int i = 0; i < t->binaries_num; i++){
            printf("%s,", t->binaries_to_execute[i]);
        }
        printf("}\n");
        t=t->prox;
    }


}

/*
int main(){
    Queue q = init_queue();
    char* arrayB[] = {"binario1", "binario2", "binario3"};
    add_task(q,"input", "output", arrayB,3);
    add_task(q,"input2", "output2", arrayB,3);
    printQueue(q);
    remove_task(q);
    printf("REM\n");
    printQueue(q);
}
*/
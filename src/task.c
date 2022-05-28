#include "includes/task.h"

//int task_id = 1;

Queue init_queue(){
    Queue q = malloc(sizeof(struct queue));
    q->inicio = NULL;
    q->fim = NULL;
    printQueue(q);
    return q;
}
int isEmpty (Queue q){
    if(q) return (q->inicio == NULL);
    return (q ==NULL);
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
            if (q->fim == NULL){
                q->inicio = new;
                q->fim = new;
            }
            else{
                q->fim->prox = new;
                q->fim = q->fim->prox;
            }  
        }
    //NAO SE PODE USAR PQ USAMOS O STDOUT PRA ESCREVER NO FICHEIRO, O USO DE PRINTFS ESCREVE NO FICHEIRO
    //printQueue(q);
    
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
    //NAO SE PODE USAR PQ USAMOS O STDOUT PRA ESCREVER NO FICHEIRO, O USO DE PRINTFS ESCREVE NO FICHEIRO
    //printQueue(*q);
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

void get_status_from_queue(int fileDescriptor, Queue q){
    TASK t = q->inicio;
    if(t == NULL)
        write(fileDescriptor, "Queue vazia!\n", strlen("Queue vazia!\n"));
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

/*
int main(){
    Queue q = init_queue();
    char* arrayB[] = {"binario1", "binario2", "binario3"};
    printf("Add1\n");
    add_task(q,"input", "output", arrayB,3, 1);
    printf("Add2\n");
    add_task(q,"input2", "output2", arrayB,3, 2);
    printQueue(q);
    printf("rem1\n");
    remove_task(q);
    remove_task(q);
    printf("REM\n");
    printQueue(q);
    add_task(q,"input3", "output3", arrayB,3, 1);
    printQueue(q);

}
*/

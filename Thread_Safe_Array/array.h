#ifndef ARRAY_H
#define ARRAY_H

#include <pthread.h>

#define ARRAY_SIZE 8
//From the zip file of hostnames, the longest one was 17 characters, so 32 should be more than enough
#define MAX_NAME_LENGTH 32

//Circular array bounded buffer with mutexes and condition variables
typedef struct{
    char hostnames[ARRAY_SIZE][MAX_NAME_LENGTH];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t notEmpty;
    pthread_cond_t notFull;
}array;

int array_init(array *s); // initialize the array
int array_put(array *s, char *hostname); // place element into the array, block when full
int array_get(array *s, char **hostname); // remove element from the array, block when empty
void array_free(array *s); //free the array's resources

#endif
#include "array.h"
//GeeksForGeeks documentation on the strncpy() function included in this library was very helpful in figuring out how to approach implementation
//https://www.geeksforgeeks.org/strncpy-function-in-c/
#include <string.h>
#include <stdlib.h>

//Initializing circular bounded buffer with front, rear and count at 0 so it returns 0 on valid initialization
int array_init(array *s){
    int returnValue;
    if(s==NULL){
        return -1;
    }
    s->front=0;
    s->rear=0;
    s->count=0;

    returnValue=pthread_mutex_init(&s->mutex,NULL);
    //error handling for edge cases where it isn't initialized properly
    if(returnValue!=0){
        return -1;
    }
    //Similar type of error handling for the condition variables
    //I referenced IBM's documentation to understand how to properly implement condition variables
    //https://www.ibm.com/docs/zh/aix/7.1?topic=programming-using-condition-variables
    returnValue=pthread_cond_init(&s->notEmpty,NULL);
    if(returnValue!=0){
        pthread_mutex_destroy(&s->mutex);
        return -1;
    }
    returnValue=pthread_cond_init(&s->notFull,NULL);
    if(returnValue!=0){
        pthread_cond_destroy(&s->notEmpty);
        pthread_mutex_destroy(&s->mutex);
        return -1;
    }
    return 0;
}

//Putting hostname into circular queue while blocking when full
int array_put(array *s,char *hostname){
    int returnValue;
    returnValue = pthread_mutex_lock(&s->mutex);
    if(returnValue!=0){
        return -1;
    }
    
    //Assuming queue is full, waiting for space
    while(s->count==ARRAY_SIZE){
        returnValue = pthread_cond_wait(&s->notFull,&s->mutex);
        if(returnValue!=0){
            pthread_mutex_unlock(&s->mutex);
            return -1;
        }
    }
    
    //Adding hostname to queue from the rear and work around
    strncpy(s->hostnames[s->rear],hostname,MAX_NAME_LENGTH-1);
    s->hostnames[s->rear][MAX_NAME_LENGTH-1]='\0';
    
    //Updating conditions after adding. Wraparound via mod
    s->rear=(s->rear+1)%ARRAY_SIZE;
    s->count++;
    //Signals that there's at least one index left to put hostnames in
    returnValue=pthread_cond_signal(&s->notEmpty);
    if(returnValue!=0){
        pthread_mutex_unlock(&s->mutex);
        return -1;
    }
    //Other side of the signaling for other threads to access the buffer
    returnValue=pthread_mutex_unlock(&s->mutex);
    if(returnValue!=0){
        return -1;
    }
    return 0;
}
//Removing and then returning the hostname from the shared queue while blocking on empty
int array_get(array *s,char **hostname){
    int returnValue;
    
    returnValue=pthread_mutex_lock(&s->mutex);
    if(returnValue!=0){
        return -1;
    }
    
    //Waiting when empty
    while(s->count==0){
        returnValue = pthread_cond_wait(&s->notEmpty,&s->mutex);
        if(returnValue!=0){
            pthread_mutex_unlock(&s->mutex);
            return -1;
        }
    }
    
    //Allocating memory with space for a null terminator
    //and then getting hostname from the buffer into the allocated space
    size_t len=strlen(s->hostnames[s->front]);
    *hostname=(char*)malloc(len+1);
    if(*hostname==NULL){
        pthread_mutex_unlock(&s->mutex);
        return -1;
    }
    strcpy(*hostname,s->hostnames[s->front]);
    
    //Updating front pointer for index and count 
    s->front=(s->front + 1)%ARRAY_SIZE;
    s->count--;
    
    //Signaling that there is space in the buffer while 
    //deallocating memory in the case that an error occurs
    returnValue=pthread_cond_signal(&s->notFull);
    if(returnValue!=0){
        free(*hostname);
        pthread_mutex_unlock(&s->mutex);
        return -1;
    }
    returnValue = pthread_mutex_unlock(&s->mutex);
    if(returnValue!=0){
        free(*hostname);
        return -1;
    }
    return 0;
}

//freeing resources
void array_free(array *s){
    pthread_mutex_destroy(&s->mutex);
    pthread_cond_destroy(&s->notEmpty);
    pthread_cond_destroy(&s->notFull);
}

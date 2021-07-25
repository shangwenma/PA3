#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "util.h"
#include "sharedArray.c"
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
typedef struct {
    char fileName[5][255];
    sem_t *semaphmore;
    char service[255];
    char result[255];
    pthread_mutex_t mutex;

    
}parameter;
void write_file(char *fileName, char value[255], pthread_mutex_t mutex);
char *safe_removeData(pthread_mutex_t mutex);
void _resolver(sem_t *semaphore, char* result,pthread_mutex_t mutex);
void safe_insert(char * value,pthread_mutex_t mutex);
int read_file(char *fileName, sem_t *semaphore, char* result,pthread_mutex_t mutex);
void *requester(void *p);

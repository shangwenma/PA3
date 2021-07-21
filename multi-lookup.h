#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "util.h"
#include "sharedArray.c"
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

void write_file(char *fileName, char value[255]);
char *safe_removeData();
void _resolver();
void safe_insert(char * value);
int read_file(char *fileName);
void *requester(void *p);
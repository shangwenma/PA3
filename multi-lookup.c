#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "util.h"
#include "sharedArray.c"
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>


int max_array_length = 20;
sem_t semaphore;
pthread_mutex_t mutex;

void write_file(char *fileName, char value[255]){
    pthread_mutex_lock(&mutex);
    FILE *file;
    file = fopen(fileName,"a");
    fputs(value,(FILE*) file);
    fclose(file);
    pthread_mutex_unlock(&mutex);
}

char *safe_removeData(){
    pthread_mutex_lock(&mutex);
    char * ret = removeData();
    pthread_mutex_unlock(&mutex);
    return ret;
}

void _resolver(){
    char ip[255];
    char buffer[255];
    strcpy(buffer,safe_removeData());

    sem_post(&semaphore);

    if(dnslookup((char *)buffer,ip, 255) == UTIL_FAILURE){
        strcpy(ip,"");
    }
    // printf("%s,%s \n", buffer,ip);
    sprintf(buffer, "%s%s%s%s",buffer, ",", ip,"\n");

    write_file("result.txt",buffer);
    
}

void safe_insert(char * value){
    pthread_mutex_lock(&mutex);
    insert(value);
    pthread_mutex_unlock(&mutex);
}

int read_file(char *fileName){
    //char *fileName = "input/names1.txt";
    FILE *file;  
    file = fopen(fileName,"r");
    char buffer[255];
    char *returnFile; //initial
    do{ // Null when empty
        returnFile = fgets(buffer, 255, (FILE*)file);
        if (returnFile){
            
            sem_wait(&semaphore);

            strtok(buffer, "\n"); // drop the line changed
            safe_insert(buffer);

            if (!isEmpty()){
                _resolver();
            }
        }       
    }while(returnFile);  
    return 0;
}

void *requester(void *p){

    for (int i = 0; i < 5; i++){
        char filename[255];
        strcpy(filename, (char *)(p + 255*i));

        char info[255];
        // Thread <thread id> serviced ### files
        sprintf(info, "%s%ld%s%s%s%s","Thread ", pthread_self(), " serviced ",filename, "files","\n");

        write_file("serviced.txt", info);
        
        read_file(filename);
    }

    return 0;
}


int main(int argc, char* argv[]){

    const int max_requester_threads = 5;
    const int max_resolver_threads = 10;
    int requesters = atoi(argv[1]);
    int resolvers = atoi(argv[2]);
    char *requester_log = argv[3];
    char *resolver_log = argv[4];
    char *input_filename = argv[5];

    if (requesters > max_requester_threads){
        requesters = max_requester_threads;
    }
    if (resolvers > max_resolver_threads){
        resolvers = max_resolver_threads;
    }
    clock_t start, end;
    start = clock();

    sem_init(&semaphore, 0, max_array_length);

    char * fileName = "input"; // find filename of the input files
    DIR * route = opendir(fileName); // Return an object // Open a directory steam on name 
    struct dirent *read; 

    char filenames[requesters][5][255];


    pthread_t requester_thread[requesters];

    int count = 0;


    while((read =readdir(route))!= NULL ){
        if(strcmp(read->d_name,".") ==0 || strcmp(read->d_name,"..")== 0){
            continue;
        }
        else{
            fileName = read->d_name;
            char *name = (char *) malloc(strlen("input/") + strlen(fileName));
            strcpy(name, "input/");
            strcat(name, fileName);
            
            strcpy(filenames[count%requesters][count / requesters], name);
            count++;
            
        }
    }

    for (int i = 0; i<requesters; i++){
        pthread_create(&requester_thread[i],NULL,requester,(void *)(filenames[i]));
    }
    
    for(int i =0; i < requesters; i++){
        pthread_join(requester_thread[i], NULL);
    }

    end = clock();
    
    printf("%s total time is %lf seconds\n", argv[0], (double)(end-start)/CLOCKS_PER_SEC);
}

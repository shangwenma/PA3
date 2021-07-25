// #include <stdio.h>
// #include <stdlib.h>
// #include <dirent.h>
// #include <string.h>
// #include "util.h"
// #include "sharedArray.c"
// #include <pthread.h>
// #include <semaphore.h>
// #include <sys/time.h>
#include "multi-lookup.h"


void write_file(char *fileName, char value[255], pthread_mutex_t mutex){

    pthread_mutex_lock(&mutex); 
    
    FILE *file; // create file object
    file = fopen(fileName,"a");// open file 
    fputs(value,(FILE*) file); //wirte to value

    fclose(file); //close the file
    pthread_mutex_unlock(&mutex);

}

char *safe_removeData(pthread_mutex_t mutex){
    pthread_mutex_lock(&mutex);
    char * ret = removeData(); 
    pthread_mutex_unlock(&mutex);
    return ret;
}

void _resolver(sem_t *semaphore, char* result,pthread_mutex_t mutex){
    char ip[255];
    char buffer[255];
    strcpy(buffer,safe_removeData(mutex)); // remove data from shared arrary and save to buffer

    sem_post(semaphore);// semaphore +1

    if(dnslookup((char *)buffer,ip, 255) == UTIL_FAILURE){ // use hostname in buffer find its ip and save to ip
        strcpy(ip,""); //hanld exception when it is not able to find its ip from hostname
    }
    // printf("%s,%s \n", buffer,ip);
    sprintf(buffer, "%s%s%s%s",buffer, ",", ip,"\n"); //comebine

    write_file(result,buffer,mutex); //write to result
    
}

void safe_insert(char * value,pthread_mutex_t mutex){
    pthread_mutex_lock(&mutex);
    insert(value);
    pthread_mutex_unlock(&mutex);
}

int read_file(char *fileName, sem_t *semaphore, char* result,pthread_mutex_t mutex){
    //char *fileName = "input/names1.txt";
    FILE *file;  
    file = fopen(fileName,"r"); //write authorize
    char buffer[255];
    char *returnFile; //initial

    do{ // Null when empty
        returnFile = fgets(buffer, 255, (FILE*)file);
        if (returnFile){
            
            sem_wait(semaphore);// semaphore -1
            //printf("buffer: %s\n",buffer);
            strtok(buffer, "\n"); // drop the line changed
            safe_insert(buffer, mutex); // write buffer to shared arrary

            if (!isEmpty()){ // go to resolver when ended
                _resolver(semaphore,result,mutex); 
            }
        }       
    }while(returnFile);  
    return 0;
}

void *_requester(void *p){
    parameter *pointer = (parameter *)p ; //transform type
    
    for (int i = 0; i < 5; i++){

        char filename[255];
        strcpy(filename, (char *)(p + 255*i)); //every block in arrary is 255
        printf("filename:%s\n", filename);
        char info[255];
        // Thread <thread id> serviced ### files
        sprintf(info, "%s%ld%s%s%s%s","Thread ", pthread_self(), " serviced ",filename, "files","\n"); // combine log information save to info

        write_file(pointer->service, info, pointer->mutex);// filename, context, lock when 1 thread is writing
        //printf("1\n");
        read_file(filename, pointer->semaphmore, pointer->result, pointer->mutex);
        //printf("2\n");
            
    }

    return 0;
}


int main(int argc, char* argv[]){
    int max_array_length = 20;
    int max_requester_threads = 5;
    int max_resolver_threads = 10;
    
    int requester = atoi(argv[1]);
    int resolver = atoi(argv[2]);
    char *requester_log = argv[3];
    char *resolver_log = argv[4];
    char *input_filename = argv[5];
    int time_use = 0;
    struct timeval start;
    struct timeval end;
    
    if(requester > max_requester_threads){
        requester = max_requester_threads;
    }
    if(resolver > max_resolver_threads){
        resolver = max_resolver_threads;
    }
    gettimeofday(&start,NULL);
    sem_t semaphore;
    pthread_mutex_t mutex;    
   
    sem_init(&semaphore, 0, max_array_length);//initialization



    DIR * route = opendir(input_filename); // Return an object // Open a directory steam on name 
    struct dirent *read; // file object saved in struct

    char filenames[resolver][5][255]; // where does it save to


    pthread_t requester_thread[requester];// create thread array

    int count = 0;


    while((read =readdir(route))!= NULL ){// when read to the end of directory, it would be null
        if(strcmp(read->d_name,".") ==0 || strcmp(read->d_name,"..")== 0){ //handle the especial case of linux

            continue;
        }
        else{
            //input_filename = read->d_name;
            char *name = (char *) malloc(strlen("input/") + strlen(read->d_name)); //put the directory and filename together //strlen- length of string //malloc- memory allocated
            strcpy(name, "input/"); //strcpy - string copy
            strcat(name, read->d_name); // strcat - string concatnate
            printf("name:%s\n", name);
            strcpy(filenames[count%requester][count / requester], name); //filename copy to the correct destnation of filenames arrary
            free(name);
            count++;
            
            
        }
    }
    parameter *requester_p = calloc(requester,sizeof(parameter));//dynamic allocate space for passing parameter to thread
    for (int i = 0; i<requester; i++){ // start to create thread
        for(int j = 0; j< 5; j++){
            strcpy((requester_p->fileName)[j],filenames[i][j]); //copy filenames 
                
        }
        
        strcpy(requester_p[i].service,requester_log); // assign each paramether
        
        requester_p->semaphmore = &semaphore;
      
        strcpy(requester_p[i].result,resolver_log);
        requester_p->mutex = mutex;

        pthread_create(&requester_thread[i],NULL,_requester,(void *)(requester_p)); // passing the parameter to each thread

    }
    
    for(int i = 0; i < requester; i++){

        pthread_join(requester_thread[i], NULL); //prevent main thread to stop before all child end

    }
    free(requester_p);
    gettimeofday(&end,NULL);
    time_use =(end.tv_sec-start.tv_sec);
    
    printf("%s total time is %d seconds\n", argv[0],time_use);
    return 0;
}

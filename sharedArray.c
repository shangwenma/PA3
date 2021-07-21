#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARRAY_SIZE 20
#define MAX_NAME_LENGTH 1025

char array[ARRAY_SIZE][MAX_NAME_LENGTH];
int front = 0;
int rear = -1;
int itemCount = 0;

char *peek() {
   return array[front];
}

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == ARRAY_SIZE;
}

int size() {
   return itemCount;
}  

void insert(char *data) {

   if(!isFull()) {
	
      if(rear == ARRAY_SIZE-1) {
         rear = -1;            
      }       

      strcpy(array[++rear],data);
      itemCount++;
   }
}

char *removeData() {
	
   if(front == ARRAY_SIZE) {
      front = 0;
   }

   itemCount--;
   return array[front++];  
}
// int main(){
//     insert("123");
//     insert("abc");
//     insert("xxxx");
//     char *a = peek();
//     printf("%s\n",a);
//     removeData();
//     // printf("%s\n",d);
//     char *b = peek();
//     printf("%s\n",b);
//     removeData();
//     char *c = peek();
//     printf("%s\n",c);
//     removeData();
//     char *e = peek();
//     printf("%s\n",e);
//     printf("%d\n",isEmpty());
//     printf("%d\n",size());
// }
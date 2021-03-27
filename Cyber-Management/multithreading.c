/**
 *A thread is a single sequence stream within in a process. Because threads have some of the properties of processes, they are sometimes called lightweight proc  esses.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *myThreadFun(void *vargp){
        system("clear");
	sleep(1);
	printf("Threading Code\n");
	return NULL;
}

int main(){

      pthread_t thread_id;
      printf("Before Thread\n");
      pthread_create(&thread_id, NULL, myThreadFun, NULL);
      pthread_join(thread_id, NULL);
      printf("After Thread\n");
      exit(0);

}

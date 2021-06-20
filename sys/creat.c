#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>  

int creat (const char *name, mode_t mode);

int main(){
   int fd;
   fd = creat("f.txt",0644);
   if (fd== -1){
     printf("Something went wrong!\n");
   }
   printf("Operation Completed!\n");
   return 0;
}

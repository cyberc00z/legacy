// linux provides two sys calls for truncating the length of a file, both of which are defined and required (to varying degress) by various POSIX standards.
//#include <sys/types.h>
// #include <unistd.h>

//first:  int ftruncate(int fd, off_t len);

// second: int truncate(const char *path, off_t len);

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>

int main(){
   int ret;

   ret = truncate("./f.txt",20);
   if (ret == -1){
      perror("truncate");
      return -1;
   }
   return 0;
}




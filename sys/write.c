/*
When a call to write() returns, the kernel has copied the data from the supplied
buffer into a kernel buffer, but there is no guarantee that the data has been
written out to its intended destination. Indeed, write calls return much too fast
for that to be the case. The disparity in performance between processors and hard
disks would make such behavior painfully obvious.
*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//ssize_t write(int fd, const void *buf, size_t count);
int main(){
  int fd;  
  const char *buf;
  ssize_t nr;
  nr = write(fd, buf, strlen(buf));
  if (nr == -1){
     perror("error");
  }
  return 0;
}



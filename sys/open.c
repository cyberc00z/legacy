#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

//int open(const char *name, int flags );
//int open(const char *name, int flags, mode_t mode);

int main(){
    int fd;
    fd = open("../p1.c",O_RDONLY);
    if (fd == -1){
       printf("errr");
    }
    printf("Operation successfull.\n");
    return 0;
}

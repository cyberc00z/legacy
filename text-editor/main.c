#include <stdio.h>
#include <ctype.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

/*******  data  **********/
struct editorConfig {
  int screenrows;
  int screencols;
  struct termios orig_termios;
};
struct editorConfig E;

/******* defines   ********/
#define CTRL_KEY(k) ((k) & 0x1f)


/****** terminal ********/

void die(const char *s){
   write(STDOUT_FILENO, "\x1b[2J", 4);
   write(STDOUT_FILENO, "\x1b[H", 3);
   perror(s);
   exit(1);
}

void disableRawMode(){
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios)==-1)
      die("tcsetattr");
}

void enableRawMode(){
   if (tcgetattr(STDIN_FILENO, &E.orig_termios) ==-1)
     die("tcgetattr");
   atexit(disableRawMode);

   struct termios raw = E.orig_termios;
   raw.c_iflag &= ~(BRKINT |ICRNL| INPCK | ISTRIP | IXON); /*disabling input flags*/
   raw.c_oflag &= ~(OPOST); /*disabling output flags*/
   raw.c_cflag |= (CS8); //misc flag
   raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);  /*disabling local flags*/
   raw.c_cc[VMIN] = 0;
   raw.c_cc[VTIME] = 1;

   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
     die("tcsetattr");
}

char editorReadKey(){
   int nread;
   char c;
   while ((nread = read(STDIN_FILENO, &c,1)) != 1){
     if (nread == -1 && errno != EAGAIN) die("read");
   }
   return c;
}

int getWindowSize(int *rows, int *cols){
   struct winsize ws;
   if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)==-1 || ws.ws_col == 0) {
     return -1;
   }
   else {
     *cols = ws.ws_col;
     *rows = ws.ws_row;
     return 0;
   }
}

/****** output   ******/
void editorDrawRows(){
   int y;
   for (y=0; y<E.screenrows; y++){
      write(STDOUT_FILENO, "~\r\n",3);
   }
}

void editorRefreshScreen(){
   write(STDOUT_FILENO,"\x1b[2J]",4);
   write(STDOUT_FILENO, "\x1b[H", 3);
  
   editorDrawRows();
   write(STDOUT_FILENO, "x\1b[H",3);

}

/****** input  ******/
void editorProcessKeypress(){
   char c = editorReadKey();

   switch(c){
     case CTRL_KEY('q'):
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
   }
}

/**** driver code  ****/
void initEditor(){
   if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main(){
   enableRawMode();
   initEditor();   
   while (1){
      editorRefreshScreen();
      editorProcessKeypress();
      char c = '\0';
      if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
         die("read");
      if (iscntrl(c)){
         printf("%d\r\n",c);
      }else {
        printf("%d ('%c')\r\n",c,c);
      }
      if (c==CTRL_KEY('q')) break;
   }
   return 0;
}

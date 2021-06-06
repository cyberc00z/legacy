#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <ctype.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>

/*******  data  **********/
typedef struct erow {
   int size;
   int rsize;
   char *chars;
   char *render;

} erow;

struct editorConfig {
  int cursorX, cursorY;
  int renderX;
  int rowoff;
  int coloff;
  int screenrows;
  int screencols;
  int numrows;
  erow *row;
  int dirty;
  char *filename;
  char statusmsg[80];
  time_t statusmsg_time;
  struct termios orig_termios;
};
struct editorConfig E;

/****** append buffer  *******/
struct appendBuffer{
   char *b;
   int len;
};

#define ABUF_INIT {NULL,0}

void abAppend(struct appendBuffer *ab, const char *s, int len){
   char *new = realloc(ab->b, ab->len + len);

   if (new == NULL) return;
   memcpy(&new[ab->len],s,len);
   ab->b = new;
   ab->len += len;
}

void abFree(struct appendBuffer *ab){
   free(ab->b);
}

/******* defines   ********/
#define CTRL_KEY(k) ((k) & 0x1f)
#define KILO_TAB_STOP 8
#define KILO_VERSION "0.0.1"
#define KILO_QUIT_TIMES 1

enum editorKey {
   BACKSPACE=127,
   ARROW_LEFT =1000,
   ARROW_RIGHT,     
   ARROW_UP,        
   ARROW_DOWN,
   DEL_KEY,
   HOME_KEY,
   END_KEY,
   PAGE_UP,
   PAGE_DOWN,
};

/******  prototypes   *******/

void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt);

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

int  editorReadKey(){
   int nread;
   char c;
   while ((nread = read(STDIN_FILENO, &c,1)) != 1){
     if (nread == -1 && errno != EAGAIN) die("read");
   }
   if (c == '\x1b'){
      char seq[3];

      if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
      if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

      if (seq[0] == '['){
         if (seq[1] >= '0' && seq[1] <= '9'){
            if (read(STDIN_FILENO, &seq[2], 1)!= 1) return '\x1b';
            if (seq[2] == '~'){
               switch(seq[1]){
                  case '1': return HOME_KEY;
                  case '2': return END_KEY;
                  case '3': return DEL_KEY;
                  case '5': return PAGE_UP;
                  case '6': return PAGE_DOWN;
                  case '7': return HOME_KEY;
                  case '8': return END_KEY;
               }
            }
         }else {
          switch(seq[1]){
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
          }
        }

      }
      else if (seq[0] == '0'){
        switch(seq[1]){
           case 'H' : return HOME_KEY;
           case 'F': return END_KEY;
        }
      }
      return '\x1b';
   }
   else {
      return c;
   }
}

int getCursorPosition(int *rows, int *cols){
   char buf[32];
   unsigned int i=0;
   
   if (write(STDOUT_FILENO, "\x1b[6n",4) != 4) return -1;
                                                           
   while (i < sizeof(buf) - 1){
      if (read(STDIN_FILENO, &buf[i],1) != 1) break;
      if (buf[i] == 'R') break;
      i++;
   }
   buf [i] = '\0';
   if (buf[0] != '\x1b' || buf[1] != '[') return -1;
   if (sscanf(&buf[2], "%d:%d", rows, cols) != 2) return -1;
   return 0;
}

int getWindowSize(int *rows, int *cols){
   struct winsize ws;
   if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)==-1 || ws.ws_col == 0) {
     if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B",12) != 12)
         return -1;
     return getCursorPosition(rows, cols);
   }
   else {
     *cols = ws.ws_col;
     *rows = ws.ws_row;
     return 0;
   }
}

/***** row operations  ******/
int editorRowCxToRx(erow *row , int cursorX){
  int renderX = 0;
  int j;
  for (j=0; j < cursorX; j++){
    if (row->chars[j] == '\t'){
       renderX += (KILO_TAB_STOP -1) - (renderX % KILO_TAB_STOP);
    }
    renderX++;
  }
}

void editorUpdateRow(erow *row){
   int tabs = 0;
   int j;
   for (j=0; j<row->size; j++){
      if (row->chars[j] == '\t') tabs++;
   }
   free(row->render);
   row->render = malloc(row->size + tabs*(KILO_TAB_STOP - 1) + 1);
   
   int idx=  0;
   for (j=0; j<row->size; j++){
      if (row->chars[j] == '\t'){
         row->render[idx++] = ' ';
         while (idx % KILO_TAB_STOP != 0) row->render[idx++] = ' ';
      }
      else {
         row->render[idx++] = row->chars[j];
      }
   }
   row->render[idx] = '\0';
   row->rsize=idx;

}

void editorInsertRow(int at,char *s, size_t len){
   if (at < 0 || at > E.numrows) return;
   
   E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
   memmove(&E.row[at+1],&E.row[at],sizeof(erow) *(E.numrows - at ));
   E.row[at].size = len;
   E.row[at].chars = malloc(len + 1);
   memcpy(E.row[at].chars, s, len);
   E.row[at].chars[len] = '\0';

   E.row[at].rsize = 0;
   E.row[at].render = NULL;
   
   E.numrows++;
   E.dirty++;
}

void editorFreeRow(erow *row){
   free(row->render);
   free(row->chars);
}

void editorDelRow(int at){
   if (at <0 || at >= E.numrows) return;
   editorFreeRow(&E.row[at]);
   memmove(&E.row[at], &E.row[at+1], sizeof(erow) * (E.numrows -at- 1));
   E.numrows--;
   E.dirty++;
}


void editorRowInsertChar(erow *row, int at, int c){
   if (at < 0 || at > row->size) at = row->size;
   row->chars = realloc(row->chars,row->size + 2);
   memmove(&row->chars[at + 1], &row->chars[at], row->size - at+1);
   row->size++;
   row->chars[at] = c;
   editorUpdateRow(row);
   E.dirty++;
}

void editorRowDelChar(erow *row, int at){
   if (at < 0 || at >= row->size) return;
   memmove(&row->chars[at], &row->chars[at+1], row->size - at);
   row->size--;
   editorUpdateRow(row);
   E.dirty++; 
}

void editorRowAppendString(erow *row, char *s, size_t len){
    row->chars = realloc(row->chars, row->size + len +1);
    mempcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}

/****  editor operations  *****/
void editorInsertNewline(){
   if  (E.cursorX == 0){
      editorInsertRow(E.cursorY, "", 0);
   }
   else {
      erow *row = &E.row[E.cursorY];
      editorInsertRow(E.cursorY+1, &row->chars[E.cursorX], row->size - E.cursorX);
      row = &E.row[E.cursorY];
      row->size = E.cursorX;
      row->chars[row->size] = '\0';
      editorUpdateRow(row);
   }
   E.cursorY++;
   E.cursorX = 0;
}

void editorInsertChar(int c){
  if (E.cursorY == E.numrows){
     editorInsertRow(E.numrows,"", 0);
  }
  editorRowInsertChar(&E.row[E.cursorY], E.cursorX, c);
  E.cursorX++;

}

void editorDelChar(){
   if (E.cursorY == E.numrows) return;
   if (E.cursorX == 0 && E.cursorY == 0) return;

   erow *row = &E.row[E.cursorY];
   if (E.cursorY > 0){
      editorRowDelChar(row, E.cursorX - 1);
      E.cursorX--;
   }
   else {
      E.cursorX = E.row[E.cursorY -1].size;
      editorRowAppendString(&E.row[E.cursorY -1], row->chars, row->size);
      editorDelRow(E.cursorY);
      E.cursorY--;
   }
}

/******   file i/o    *******/
char *editorRowsToString(int *buflen){
   int totlen = 0;
   int j;
   for (j=0; j< E.numrows; j++){
      totlen += E.row[j].size + 1;
   }
   *buflen = totlen;

   char *buf = malloc(totlen);
   char *p = buf;
   for (j=0; j< E.numrows;j++){
      memcpy(p, E.row[j].chars, E.row[j].size);
      p += E.row[j].size;
      *p = '\n';
      p++;
   }
   return buf;
}

void editorOpen(char *filename){
    free(E.filename);
    E.filename = strdup(filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) die("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1){
       while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen -1] == '\r' ))
         linelen--;

       editorInsertRow(E.numrows,line, linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
}

void editorSave(){
   if (E.filename == NULL){
      E.filename = editorPrompt("Save as :%s");
      if (E.filename == NULL){
         editorSetStatusMessage("Save Aborted");
         return;
      }
   };
   int len;
   char *buf = editorRowsToString(&len);
   int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
   if (fd != -1){
      if (ftruncate(fd, len) != -1){
         if (write(fd, buf, len)== len){
            close(fd);
            free(buf);
            editorSetStatusMessage("%d bytes written to disk", len);
            return;
         }
      }
      close(fd);
   }
   free(buf);
   editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

/****** output   ******/
void editorScroll(){
   E.renderX = 0;
   if (E.cursorY < E.numrows){
      E.renderX = editorRowCxToRx(&E.row[E.cursorY], E.cursorX);
   }
   if (E.cursorY < E.rowoff){
      E.rowoff = E.cursorY;
   }
   if (E.cursorY >= E.rowoff + E.screenrows){
      E.rowoff = E.cursorY - E.screenrows + 1;
   }
   if (E.renderX < E.coloff){
      E.coloff = E.renderX;
   }
   if (E.renderX >= E.coloff + E.screencols){
      E.coloff = E.renderX - E.screenrows + 1;
   }
}

void editorDrawStatusBar(struct appendBuffer *ab){
    abAppend(ab, "\x1b[7m",4);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines", E.filename ? E.filename: "[No Name]", E.numrows, E.dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d%d", E.cursorY + 1, E.numrows);
    if (len > E.screencols) len = E.screencols;
    abAppend(ab, status, len);
    while (len < E.screencols){
       if (E.screencols - len == rlen){
          abAppend(ab, rstatus, rlen);
          break;
       } else {
          abAppend(ab, " ", 1);
          len++;
       }
    }
    abAppend(ab, "\x1b[m", 3);
    abAppend(ab, "\r\n", 2);
}


void editorDrawMessageBar(struct appendBuffer *ab){
   abAppend(ab, "\x1b[K", 3);
   int msglen = strlen(E.statusmsg);
   if (msglen > E.screencols) msglen = E.screencols;
   if (msglen && time(NULL) - E.statusmsg_time < 5){
      abAppend(ab, E.statusmsg, msglen);
   }
}
void editorDrawRows(struct appendBuffer *ab){
   int y;
   for (y=0; y<E.screenrows; y++){
     int filerow = y+ E.rowoff;
     if (filerow >= E.numrows){
      if (E.numrows == 0 &&  y == E.screenrows / 3){
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome),"lol editor -- version %s", KILO_VERSION);
        if (welcomelen > E.screencols){
           welcomelen = E.screencols;
        }
        int padding = (E.screencols - welcomelen) /2;
        if (padding){
           abAppend(ab,"~", 1);
           padding--;
        }
        while (padding--) abAppend(ab, " ", 1);
        abAppend(ab, welcome, welcomelen);
      }
      else {
        abAppend(ab ,"~", 1);
      }
      } else {
        int len = E.row[filerow].rsize - E.coloff;
        if (len < 0) len = 0;
        if (len > E.screencols) len = E.screencols;
        abAppend(ab, &E.row[filerow].render[E.coloff],len);
      }
      
      abAppend(ab, "\x1b[K",3);
      abAppend(ab, "\r\n", 2);
      
  }   
}


void editorRefreshScreen(){
   editorScroll();
   struct appendBuffer ab = ABUF_INIT;
   

   abAppend(&ab, "\x1b[?25l",6);
   //abAppend(&ab,"\x1b[2J",4);
   abAppend(&ab, "\x1b[H", 3);
  
   editorDrawRows(&ab);
   editorDrawStatusBar(&ab);
   editorDrawMessageBar(&ab);

   char buf[32];
   snprintf(buf, sizeof(buf), "\x1b[%d;%dH",(E.cursorY - E.rowoff) + 1 ,( E.renderX - E.coloff) + 1);
   
   abAppend(&ab,buf,strlen(buf));
   
   abAppend(&ab, "x\1b[?25h",6);

   write(STDOUT_FILENO, ab.b, ab.len);
   abFree(&ab);
}



void editorSetStatusMessage(const char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(NULL);
}

/****** input  ******/
char *editorPrompt(char *prompt){
   size_t bufsize = 128;
   char *buf = malloc(bufsize);

   size_t buflen = 0;
   buf[0] = '\0';

   while (1){
      editorSetStatusMessage(prompt, buf);
      editorRefreshScreen();

      int c = editorReadKey();
      if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
          if (buflen != 0) buf[--buflen] ='\0';
      }
      else if (c=='\x1b'){
         editorSetStatusMessage("");
         free(buf);
         return NULL;  
      }else if (c == '\r'){
         if (buflen != 0){
            editorSetStatusMessage("");
            return buf;
         }
      } else if (!iscntrl(c) && c< 128){
         if (buflen == bufsize -1){
            bufsize *= 2;
            buf = realloc(buf, bufsize);
         }
         buf[buflen++] = c;
         buf[buflen] = '\0';
      }
   }
}

void editorMoveCursor( int key){
  erow *row = (E.cursorY >= E.numrows) ? NULL : &E.row[E.cursorY];
  switch(key){
      case ARROW_LEFT:
         if (E.cursorX != 0){
           E.cursorX--;
         } else if (E.cursorY > 0){
           E.cursorY--;
           E.cursorX = E.row[E.cursorY].size;
         }
         break;
      case ARROW_RIGHT:
         if (row && E.cursorX < row->size){
           E.cursorX++;
         }
         else if (row && E.cursorY == row->size){
           E.cursorY++;
           E.cursorX =0;
         }
         break;
      case ARROW_UP:
         if (E.cursorY != 0){
            E.cursorY--;
         }
         break;
      case ARROW_DOWN:
         if (E.cursorY < E.numrows){
            E.cursorY++;
         }
         break;
   }
   row = (E.cursorY >= E.numrows) ? NULL : &E.row[E.cursorY];
   int rowlen = row ? row->size : 0;
   if (E.cursorX > rowlen){
       E.cursorX = rowlen;
   }
}

void editorProcessKeypress(){
   static int quit_times = KILO_QUIT_TIMES;

   int c = editorReadKey();
   switch(c){
     case '\r':
        editorInsertNewline();
        break;

     case CTRL_KEY('q'):
        if (E.dirty && quit_times > 0){
           editorSetStatusMessage("WARNING!!! File has unsaved changes. Press Ctrl - Q %d more times to quit.", quit_times);
           quit_times--;   
           return;
        }
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;

     case PAGE_UP:
     case PAGE_DOWN:
        {
          if (c == PAGE_UP){
             E.cursorY = E.rowoff;
          }
          else if (c== PAGE_DOWN){
             E.cursorY = E.rowoff + E.screenrows - 1;
             if (E.cursorY > E.numrows) E.cursorY = E.numrows;
          }
          int times = E.screenrows;
          while(times--){
             editorMoveCursor(c== PAGE_UP ? ARROW_UP : ARROW_DOWN);
          }
        
        }
        break;

     case HOME_KEY:
        E.cursorX = 0;
        break;

     case END_KEY:
        if (E.cursorY < E.numrows){
            E.cursorX = E.row[E.cursorY].size;
        }
        break;

     case BACKSPACE:
     case CTRL_KEY('h'):
     case DEL_KEY:
         if (c==DEL_KEY) editorMoveCursor(ARROW_RIGHT);
         editorDelChar();
         break;

      case CTRL_KEY('s'):
         editorSave();
         break;
     case ARROW_UP:
     case ARROW_DOWN:
     case ARROW_LEFT:
     case ARROW_RIGHT:
         editorMoveCursor(c);
         break;
     case CTRL_KEY('l'):
     case '\x1b':
         break;

     default:
         editorInsertChar(c);
         break;
   }
   quit_times = KILO_QUIT_TIMES;
}

/**** driver code  ****/
void initEditor(){
   E.cursorX = 0;
   E.cursorY = 0;
   E.rowoff = 0;
   E.renderX = 0;
   E.coloff = 0;
   E.numrows = 0;
   E.dirty = 0;
   E.row = NULL;
   E.filename = NULL;
   E.statusmsg[0] = '\0';
   E.statusmsg_time = 0;
   if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
   E.screenrows -= 2;
}

int main(int argc, char *argv[]){
   enableRawMode();
   initEditor();
   if (argc >= 2){
      editorOpen(argv[1]);
   }
   editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");
   while (1){
      editorRefreshScreen();
      editorProcessKeypress();
   }
   return 0;
}

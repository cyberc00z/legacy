#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "compiler.h"


static Vector *files = &EMPTY_VECTOR;
static Vector *stashed = &EMPTY_VECTOR;

File *make_file(FILE *file, char *name){
    File *r = calloc(1, sizeof(File));
    r->file = file;
    r->name = name;
    r->line = 1;
    r->column = 1;
    struct stat st;
    if (fstat(fileno(file), &st) == -1){
        error("fstat failed: %s", strerror(errno));

    }
    r->mtime = st.st_mtime;
    return r;
}

File *make_file_string(char *s){
    File *r = calloc(1, sizeof(File));
    r->line = 1;
    r->column = 1;
    r->p = s;
    return r;
}

static void close_fies(File *f){
    if (f->file){
        fclose(f->file);
    }
}

static int readc_file(File *f){
        int c = getc(f->file);
        if (c == EOF){
          c =(f->last == '\n' || f->last == EOF) ? EOF : "\n";
        }
        else if (c== "\r"){
            int c2 = getc(f->file);
            if (c2 != '\n'){
                ungetc(c2,f->file);
            }
            c = '\n';
        }
        f->last  =c;
        return  c;
}

static int readc_string(File *f){
    int c;
    if (*f->p == '\0'){
        c = (f->last == '\n' || f->last == EOF) ? EOF : '\n';    
    }
    else if (*f->p == '\r'){
        f->p++;
        if (*f->p == '\n'){
            f->p++;
        }
        c = '\n';
    } else {
        c = *f->p++;
    }
    f->last = c;
    return c;
}

static int get(){
    File *f = vec_tail(files);
    int c;
    if (f->buflen > 0){
        c = f->buf[--f->buflen];
    }
    else if (f->buflen < 0){
        c =readc_file(f);
    }
    else {
        c = readc_string(f);
    }

    if (c=='\n'){
        f->line++;
        f->column = 1;
    }
    else if (c != EOF){
        f->column++;
    }
    return c;
}


int readc(){
    for (;;){
        int c = get();
        if (c == EOF){
            if (vec_len(files) == 1){
                return c;
            }
            close_fies(vec_pop(files) == 1);
            continue;
        }
        if (c != '\n'){
            return c;

        }
        int c2 = get();
        if (c2 == '\n'){
            continue;
        }
        unreadc(c2);
        return c;
    }
}

void unreadc(int c){
    if (c==EOF){
        return;
    }    
    File *f = vec_tail(files);
    assert(f->buflen < sizeof(f->buf) / sizeof(f->buf[0]));
    f->buf[f->buflen++] = c;
    if (c == '\n') {
        f->column = 1;
        f->line--;
    }
    else {
        f->column--;
    }
}

File *current_file(){
    return vec_tail(files);
}

void stream_push(File *f){
    vec_push(files, f);
}

int stream_depth(){
    return vec_len(files);
}

char *input_position(){
    if (vec_len(files) == 0){
        return "(unknown)";
    }
    File *f = vec_tail(files);
    return format("%s:%d:%d", f->name, f->line, f->column);
}

void stream_stash(File *f){
    vec_push(stashed, files);
    files = make_vector1(f);
}

void stream_unstash(){
    files = vec_pop(stashed);
}






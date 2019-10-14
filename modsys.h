#ifndef MODSYS_H
#define MODSYS_H
#include <stdbool.h>

struct command {
    char** args;
    int argc;
    bool background;
};

struct job {
    char* input;
    int* exitcode_arr;
    char** args;
    int argc;
    int exitcode;
    int commandnum;
    struct job* next;
    pid_t processid;
    bool handle;
    bool pipe;
};

bool inout_re(struct command** object,struct job* current,int index,int commandnum,int* currentjob,char* input,bool ispipe);
void printcompletemes(char* input,int exitcode,struct job* current,bool background,bool pipe,int commandnum,int* exitcode_arr);
void modsys(struct command **object,int commandnum,char* input,int* currentjob,struct job* current,bool first);
void external_modsys(struct command **object,bool print,char* input,struct job* current);
bool pipe_recur(struct command **obejct,struct job* current,int *currentjob,int** pid_array,int commandnum,char* input,int* exitcode,bool first);

#endif


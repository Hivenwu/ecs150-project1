
#include <stdbool.h>

struct command {
    char** args;
    int argc;
    bool background;
};

struct job {
    char* input;
    char** args;
    int argc;
    int exitcode;
    struct job* next;
    pid_t processid;
    bool handle;
};

void addnode(struct command **object,struct job* current,char* userinput,bool first,pid_t PID);
bool checkbackground(struct  command **object,int index);
void printcompletemes(char* input,int exitcode,struct job* current,bool background,bool pipe,int commandnum,int* exitcode_arr);
void waitforbackground(int* currentjob,struct command **object,struct job* current,char* input,bool doublecheck);
void modsys(struct command **object,int commandnum,char* input,int* currentjob,struct job* current,bool first);
void external_modsys(struct command **object,bool print,char* input,struct job* current);
bool pipe_recur(struct command **obejct,struct job* current,int *currentjob,int** pid_array,int pipei,int commandi,int commandnum,char* input,int* exitcode,bool first);



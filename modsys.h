
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
    struct job* next;
    pid_t processid;
    bool handle;
    bool pipe;
};

bool inout_re(struct command** object,struct job* current,int index,int* currentjob,char* input);
void addnode(struct command **object,struct job* current,char* userinput,bool first,pid_t PID,int index,int* exitcode,bool pipe);
bool checkbackground(struct  command **object,int index);
void printcompletemes(char* input,int exitcode,struct job* current,bool background,bool pipe,int commandnum,int* exitcode_arr);
void waitforbackground(int* currentjob,struct command **object,struct job* current,char* input,bool doublecheck);
void modsys(struct command **object,int commandnum,char* input,int* currentjob,struct job* current,bool first);
void external_modsys(struct command **object,bool print,char* input,struct job* current);
bool pipe_recur(struct command **obejct,struct job* current,int *currentjob,int** pid_array,int commandnum,char* input,int* exitcode,bool first);




#include <stdbool.h>

struct command {
    char** args;
    int argc;
    bool background;
};

struct job {
    char** args;
    struct job* next;
    pid_t processid;
    bool handle;
};


void modsys(struct command **object,int commandnum,int** bgpid_command,int* currentjob,struct job* current,bool first);
void external_modsys(struct command **object,bool print);
void pipe_recur(struct command **obejct,int** pid_array,int pipei,int commandi,int commandnum);



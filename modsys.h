#include <stdbool.h>

struct command {
    char* command;
    char* args;
    bool background;
};

enum execu_type {
    COMMAND_DATE,
    COMMAND_LS,
    COMMAND_PWD,
    COMMAND_EXIT,
    COMMAND_CD,
    COMMAND_UNDEF,
};


enum execu_type execu_determine(struct command **object,int commandnum);
void modsys(struct command **object,int commandnum);
void external_modsys(struct command **object,bool print);
int pipe_recur(struct command **obejct,int **fd,int *i,int commandnum);



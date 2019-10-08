
#include <stdbool.h>

struct command {
    char** args;
    int argc;
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
void pipe_recur(struct command **obejct,int ***fd,int** pid_array,int pipei,int commandi,int commandnum);



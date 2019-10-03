struct command {
    char* command;
    char* args;
};

enum execu_type {
    COMMAND_DATE,
    COMMAND_LS,
    COMMAND_PWD,
    COMMAND_EXIT,
    COMMAND_CD,
    COMMAND_UNDEF,
};


enum execu_type execu_determine(struct command **object);
void modsys(struct command **object);



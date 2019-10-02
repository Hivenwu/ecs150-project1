struct command {
    char* input;
    char* arguments;
    char* args;
    char* cmd;
    int index;
};

void assignval(struct command *object);
void modsys(struct command *instance);



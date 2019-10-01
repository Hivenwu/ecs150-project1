#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


struct command {
    char* input;
    char* args;
    char* cmd;
    int index;
};

void parse(struct command *object,size_t buffsize) { //Parse user's command-line input and write it to struct
    int num = 0;
    char* buff;

    buff = (char *)malloc(buffsize * sizeof(char));

    for (int index = 0; index < buffsize; index = index + 1) {
        if ((object->input)[index] != ' ') {
            buff[num] = object->input[index];
            num = num + 1;
        }
    }
    
    strcpy(object->input,buff);

    return;
}

void assignval(struct command *object) {  //Assign struct value
    if (!strcmp(object->input,"date\n")){
        object->cmd = "/bin/date";
        object->args = "-u";
        object->index = 1;
    }
    else if (!strcmp(object->input,"ls\n")){
        object->cmd = "/bin/ls";
        object->args = ".";
        object->index = 2;
    }
    else {
        object->index = 3;
    }

    return;
}

int main(int argc, char *argv[])
{
    char *buff;
    int retval;
    struct command instance;
    size_t buffsize = 40;
    pid_t PID;

    while (1) {
        printf("$sshell: ");
        instance.input = (char *)malloc(buffsize * sizeof(char));
        getline(&(instance.input),&buffsize,stdin);   //Get user command
        parse(&instance,buffsize);
        assignval(&instance);
        char *args[] = {instance.cmd, instance.args, NULL};
        switch(instance.index)
        {
            case 1:
            {
                PID = fork();
                if (PID == 0) {
                    retval = execv(instance.cmd,args);
                }   
                else {
                    wait(&PID);
                    fprintf(stderr, "+ completed '%s': [%d]\n", instance.cmd, retval);
                }   
                break;
            }
            case 2:
            {
                PID = fork();
                if (PID == 0) {
                    retval = execv(instance.cmd,args);
                }
                else {
                    wait(&PID);
                    fprintf(stderr, "+ completed '%s': [%d]\n",instance.cmd, retval);
                }
                break;
            }
            case 3:
            {
                perror("Ops");
            }
        }
    }
    return EXIT_SUCCESS;
}
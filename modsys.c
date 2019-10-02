#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"

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
    else if (!strcmp(object->input,"exit\n")) {
        object->index = 3;
    }
    else if (!strcmp(object->input,"pwd\n")) {
        object->index = 4;
    }
    else {
        object->index = 5;
    }

    return;
}


void modsys(struct command *instance) {

    pid_t PID;
    char *args[] = {instance->cmd, instance->args, NULL};

    switch(instance->index)
        {
            case 1:
            {
                PID = fork();
                if (PID == 0) {
                    execv(instance->cmd,args);
                }   
                else {
                    wait(&PID);
                    fprintf(stderr, "+ completed '%s': [%d]\n", instance->cmd, EXIT_SUCCESS);
                }   
                break;
            }
            case 2:
            {
                PID = fork();
                if (PID == 0) {
                    execv(instance->cmd,args);
                }
                else {
                    wait(&PID);
                    fprintf(stderr, "+ completed '%s': [%d]\n",instance->cmd, EXIT_SUCCESS);
                }
                break;
            }
            case 3:
            {
                printf("Bye...\n");
                exit(0);
            }
            case 4:
            {
                getcwd(instance->input,40);
                fprintf(stderr,"%s\n",instance->input);
                fprintf(stderr, "+ completed '%s': [%d]\n",instance->cmd, EXIT_SUCCESS);
            }
            case 5:
            {
                perror("Ops");
            }
        }
    
    return;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"
#define buffsize 100

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
        object->cmd = "pwd";
        object->index = 4;
    }
    else if (!strcmp(object->input,"cd")) {
        object->cmd = object->input;
        object->args = object->arguments;
        object->index = 5;
    }
    else if (!strcmp(object->input,"cat")) {
        object->cmd = object->input;
        object->args = object->arguments;
        object->index = 6;
    }
    else {
        object->index = 6;
    }

    return;
}


void modsys(struct command *instance) {

    pid_t PID;
    int exitcode;
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
                getcwd(instance->input,buffsize);
                fprintf(stderr,"%s\n",instance->input);
                fprintf(stderr, "+ completed 'pwd': [%d]\n", EXIT_SUCCESS);
                break;
            }
            case 5:
            {   
                exitcode = chdir(instance->arguments);  
                if (exitcode != 0) {
                    fprintf(stderr, "Error: no such directory\n");  
                }
                else {
                    fprintf(stderr,"%s\n",getcwd(instance->arguments,100));
                }
                fprintf(stderr, "+ completed '%s %s': [%d]\n",instance->input,instance->arguments, abs(exitcode));
                break;
            }
            case 6:
            {
                
            }
            case 7:
            {
                
            }
        }
    
    return;
}
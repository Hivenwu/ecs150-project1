#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"
#define buffsize 100


enum execu_type execu_determine(struct command **object) {
    if (!strcmp((*object)[0].command,"date")) {
        return COMMAND_DATE;
    }
    else if (!strcmp((*object)[0].command,"ls")) {  
        return COMMAND_LS;
    }
    else if (!strcmp((*object)[0].command,"cd")) {
        return COMMAND_CD;
    }
    else if (!strcmp((*object)[0].command,"pwd")) {
        return COMMAND_PWD;
    }
    else if (!strcmp((*object)[0].command,"exit")) {
        return COMMAND_EXIT;
    }
    return COMMAND_UNDEF;
}


void modsys(struct command **object) {

    pid_t PID;
    int exitcode;
    char* argv[] = {(*object)[0].command,(*object)[0].args,NULL};

    switch(execu_determine(object))
        {
            case COMMAND_DATE:
            {
                PID = fork();
                
                if (PID == 0) {
                    char* argv2[] = {"-u",NULL};
                    execv("/bin/date",argv2);
                }   
                else {
                    wait(&PID);
                    fprintf(stderr, "+ completed '%s': [%d]\n", (*object)[0].command, EXIT_SUCCESS);
                }
                return;
            }
            case COMMAND_LS:
            {
                PID = fork();
                if (PID == 0) {
                    if (!strcmp((*object)[0].args,"")){
                        char* argv3[] = {".",NULL};
                        execv("/bin/ls",argv3);
                    }
                    else{
                        execv("/bin/ls",argv);
                    }
                    
                }
                else {
                    wait(&PID);
                    if (!strcmp((*object)[0].args,"")){
                        fprintf(stderr, "+ completed '%s': [%d]\n",(*object)[0].command,EXIT_SUCCESS);
                    }
                    else{
                        fprintf(stderr, "+ completed '%s %s': [%d]\n",(*object)[0].command,(*object)[0].args,EXIT_SUCCESS);
                    }
                }
                return;
            }
            case COMMAND_EXIT:
            {
                printf("Bye...\n");
                exit(0);
            }
            case COMMAND_PWD:
            {   
                char* address = (char *)malloc(buffsize * sizeof(char));
                getcwd(address,buffsize);
                fprintf(stderr,"%s\n",address);
                fprintf(stderr, "+ completed 'pwd': [%d]\n", EXIT_SUCCESS);
                return;
            }
            case COMMAND_CD:
            {   
                exitcode = chdir((*object)[0].args);  
                if (exitcode != 0) {
                    fprintf(stderr, "Error: no such directory\n");  
                }
                else {
                    fprintf(stderr,"%s\n",getcwd((*object)[0].args,100));
                }
                fprintf(stderr, "+ completed '%s %s': [%d]\n",(*object)[0].command,(*object)[0].args, abs(exitcode));
                return;
            }
            case COMMAND_UNDEF:
            {
                fprintf(stderr, "Error: command not found\n"); 
                fprintf(stderr, "+ completed '%s %s': [%d]\n",(*object)[0].command,(*object)[0].args, EXIT_FAILURE);
                return;
            }
        }
    
    return;
}
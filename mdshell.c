#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include "modsys.h"
#define buffnum 40

int pipesize(char* input) {
    int commandsize = 1;

    for (int index = 0; index < buffnum; index = index + 1) {
        if (input[index] == '|') {
            commandsize = commandsize + 1;
        }
    }
    
    return commandsize;
}

void parse(struct command **object,char* input,int commandnum) { //Parse user's command-line input and write it to struct

    
    char* args;
    char* command;
    int argsi = 0;
    int commandi = 0;
    int objecti = 0;
    bool arg_detect = false;
    bool command_detect = false;
    bool pipe_detect = false;

    args = (char *)malloc(buffnum * sizeof(char));
    command = (char *)malloc(buffnum * sizeof(char));

    
    *object = malloc(commandnum*sizeof(struct command));
    

    for (int i = 0; i < commandnum; i = i + 1) {
        (*object)[i].args = (char *)malloc(buffnum * sizeof(char));
        (*object)[i].command = (char *)malloc(buffnum * sizeof(char));
    }

    for (int index = 0; index <= buffnum; index = index + 1) {
    

        if (input[index] != ' ') {
            if (input[index] == '\n') {
                strcpy((*object)[objecti].args,args);
                strcpy((*object)[objecti].command,command);
                return;
            }
            else if (!command_detect && input[index] != '|') {
                command_detect = true;
                command[commandi] = input[index];
                commandi = commandi + 1;
            }
            else if (!arg_detect && command_detect) {
                command[commandi] = input[index];
                commandi = commandi + 1;
            }
            else if (input[index] != ' ' && input[index] != '|' && arg_detect && command_detect) {
                args[argsi] = input[index];
                argsi = argsi + 1;
            }
            else if (input[index] == '|') {
                strcpy((*object)[objecti].args,args);
                strcpy((*object)[objecti].command,command);
                arg_detect = false;
                command_detect = false;
                objecti = objecti + 1;
                memset(command, 0, buffnum);
                memset(args, 0, buffnum);
                commandi = 0;
                argsi = 0;
            }
        }
        else if (input[index] == ' ') {
            if (!arg_detect && command_detect) {
                arg_detect = true;
            }
        }
        

    }

    return;
}

/*void printcontent(struct command **object,int commandnum) {
    
    for (int i = 0; i < commandnum; i = i + 1) {
        printf("%s\n",(*object)[i].command);
        printf("%s\n",(*object)[i].args);
        printf("\n");
    }
    return;
} */




int main(int argc, char *argv[])
{
    int commandnum;
    size_t buffsize = 40;
    
    char *input = (char *)malloc(buffsize * sizeof(char));
    
    

    while (1) {
        printf("$sshell: ");
        struct command *list;
        getline(&input,&buffsize,stdin);   //Get user command
        
        commandnum = pipesize(input);
        parse(&list,input,commandnum);
        //The switch statement is used to determine which command-line operation to run.
        modsys(&list);
    }
    return EXIT_SUCCESS;
}
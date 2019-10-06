#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"
#define buffnum 512

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
    bool space_detect = false;

    args = (char *)malloc(buffnum * sizeof(char));
    command = (char *)malloc(buffnum * sizeof(char));
    *object = malloc(commandnum*sizeof(struct command));
    

    for (int i = 0; i < commandnum; i = i + 1) {
        (*object)[i].args = (char *)malloc(buffnum * sizeof(char));
        (*object)[i].command = (char *)malloc(buffnum * sizeof(char));
        (*object)[i].background = (bool *)malloc(buffnum * sizeof(bool));
    }

    

    char* command_token = strtok(input, "|"); 
	
	// Keep printing tokens while one of the 
	// delimiters present in str[]. 
	while (command_token != NULL) {
        int len = strlen(command_token);
        for (int i = 0; i < len; i++) {
            if (command_token[i] == ' ') {
                if (!command_detect) {
                }
                else if (command_detect){
                    arg_detect = true;
                }
            }
            else if (command_token[i] != ' ') {
                if (command_token[i] == '\n') {
                }
                else if (!command_detect) {
                    command_detect = true;
                    command[commandi] = command_token[i];
                    commandi++;
                }
                else if (command_detect && !arg_detect) {
                    command[commandi] = command_token[i];
                    commandi++;
                }
                else if (command_detect && arg_detect) {
                    args[argsi] = command_token[i];
                    argsi++;
                }
            }
        }
        strcpy((*object)[objecti].command,command);
        strcpy((*object)[objecti].args,args);
        (*object)[objecti].background = false;
        memset(command,0,strlen(command));
        memset(args,0,strlen(args));
        command_detect = false;
        arg_detect = false;
        space_detect = false;
        commandi = 0;
        argsi = 0;
        objecti++;
		command_token = strtok(NULL, "|");
	}

    return;
}

void printcontent(struct command **object,int commandnum) {
    
    for (int i = 0; i < commandnum; i = i + 1) {
        printf("%s\n",(*object)[i].command);
        printf("%s\n",(*object)[i].args);
        printf("%d\n",(*object)[i].background);
        printf("\n");
    }
    return;
} 




int main(int argc, char *argv[])
{
    int commandnum;
    size_t buffsize = 512;
    char *input = (char *)malloc(buffsize * sizeof(char));

    while (1) {
        printf("$sshell: ");
        struct command *list;
        getline(&input,&buffsize,stdin);   //Get user command
        
        commandnum = pipesize(input);
        parse(&list,input,commandnum);
        //printcontent(&list,commandnum);
        //The switch statement is used to determine which command-line operation to run.
        modsys(&list,commandnum);
        memset(input,0,buffsize); 
    }

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
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
    int argnum = 1;
    bool commandcopied = false;
    bool arg_detect = false;
    bool command_detect = false;

    args = (char *)malloc(buffnum * sizeof(char));
    command = (char *)malloc(buffnum * sizeof(char));
    *object = malloc(commandnum*sizeof(struct command));
    

    for (int i = 0; i < commandnum; i = i + 1) {
        (*object)[i].background = (bool *)malloc(buffnum * sizeof(bool));
        (*object)[i].args = malloc(512*sizeof(char*));
    }

    char* command_token = strtok(input, "|"); 

	while (command_token != NULL) {
        int len = strlen(command_token);
        for (int i = 0; i < len; i++) {
            if (command_token[i] == ' ') {
                if (!command_detect) {
                }
                else if (command_detect && !arg_detect){
                    arg_detect = true;
                }
                else if (command_detect && arg_detect && args[0] != ' ' && strlen(args)!= 0) {
                    (*object)[objecti].args[argnum] = malloc(512*sizeof(char));
                    strcpy((*object)[objecti].args[argnum],args);
                    memset(args,0,strlen(args));
                    argnum = argnum + 1;
                    argsi = 0;
                } 
            }
            else if (command_token[i] != ' ') {
                if (command_token[i] == '\n') {
                }
                else if (!command_detect) {
                    command_detect = true;
                    command[commandi] = command_token[i];
                    commandi = commandi + 1;
                }
                else if (command_detect && !arg_detect) {
                    if (((command_token[i]) == '>' || (command_token[i]) == '<')) {
                        (*object)[objecti].args[0] = malloc(512*sizeof(char));
                        (*object)[objecti].args[1] = malloc(512*sizeof(char));
                        strcpy((*object)[objecti].args[0],command);
                        (*object)[objecti].args[1][0] = command_token[i];
                        (*object)[objecti].args[1][1] = '\0';
                        memset(args,0,strlen(args));
                        arg_detect = true;
                        commandcopied = true;
                        argnum = argnum + 1;
                        argsi = 0;
                    }
                    else {
                        command[commandi] = command_token[i];
                        commandi = commandi + 1;
                    }
                    
                }
                else if (command_detect && arg_detect) {
                    if (((command_token[i]) == '>' || (command_token[i]) == '<')) {
                        if (strlen(args) != 0) {
                            (*object)[objecti].args[argnum] = malloc(512*sizeof(char));
                            (*object)[objecti].args[argnum+1] = malloc(512*sizeof(char));
                            strcpy((*object)[objecti].args[argnum],args);
                            (*object)[objecti].args[argnum+1][0] = command_token[i];
                            (*object)[objecti].args[argnum+1][1] = '\0';
                            argnum = argnum + 2;
                        }
                        else {
                            (*object)[objecti].args[argnum] = malloc(512*sizeof(char));
                            (*object)[objecti].args[argnum][0] = command_token[i];
                            (*object)[objecti].args[argnum][1] = '\0';
                            argnum = argnum + 1;
                        }
                        memset(args,0,strlen(args));
                        argsi = 0;
                    }
                    else {
                        args[argsi] = command_token[i];
                        argsi = argsi + 1;
                    }
                }
            }
        }
        if (!commandcopied){
            (*object)[objecti].args[0] = malloc(512*sizeof(char));
            strcpy((*object)[objecti].args[0],command);
        }
        

        if (strcmp(args,"")) {
            (*object)[objecti].args[argnum] = malloc(512*sizeof(char));
            strcpy((*object)[objecti].args[argnum],args);
            (*object)[objecti].args[argnum+1] = NULL;
            (*object)[objecti].argc = argnum + 2;
            
        }
        else {
            (*object)[objecti].args[argnum] = NULL;
            (*object)[objecti].argc = argnum + 1;
        }
        (*object)[objecti].background = false;
        memset(command,0,strlen(command));
        memset(args,0,strlen(args));
        objecti = objecti + 1;
        argnum = 1;
        commandi = 0;
        argsi = 0;
        command_detect = false;
        arg_detect = false;
		command_token = strtok(NULL, "|");
    }
    return;
}

bool input_errordetect(struct command** object, char* input, int commandnum) {

    for (int i = 0; i < commandnum; i++) {
        if ((*object)[i].argc >= 18) {
            fprintf(stderr,"Error: too many process arguments\n");
            return true;
        }

        if (((*object)[i].argc == 0 || strlen((*object)[i].args[0]) <= 1) && strcmp(input,"\n")) {
            fprintf(stderr,"Error: missing command\n");
            return true;
        } 
        for (int j = 0; j < (*object)[i].argc - 1; j = j + 1) {
            if (!strcmp((*object)[i].args[j],"&")) {
                fprintf(stderr,"Error: mislocated background sign\n");
                return true;
            }
            if (!strcmp((*object)[i].args[j],"<")) {
                if (i != 0) {
                    fprintf(stderr,"Error: mislocated input redirection\n");
                    return true;
                }
                if ((*object)[i].args[j+1] == NULL) {
                    fprintf(stderr,"Error: no input file\n");
                    return true;
                }
                int filein = open((*object)[i].args[j+1],O_RDONLY);
                if (filein < 0) {
                    fprintf(stderr,"Error: cannot open input file\n");
                    return true;
                }
            }
            else if (!strcmp((*object)[i].args[j],">")) {
                if (i != commandnum - 1) {
                    fprintf(stderr,"Error: mislocated output redirection\n");
                    return true;
                }
                if ((*object)[i].args[j+1] == NULL) {
                    fprintf(stderr,"Error: no output file\n");
                    return true;
                }
                int fileout = open((*object)[i].args[j+1],O_RDWR|O_CREAT|O_APPEND, 0644);
                if (fileout < 0) {
                    fprintf(stderr,"Error: cannot open output file\n");
                    return true;
                }
            }
        }
    }

    return false; 
}

void printcontent(struct command **object,int commandnum) {
    printf("commandnm is %d\n",commandnum);
    for (int i = 0; i < commandnum; i++){
        printf("Command is %s and",(*object)[i].args[0]);
        printf("Argument is ");
        for (int k = 1; k < (*object)[i].argc; k++) {
            printf("%s--",(*object)[i].args[k]);
        }
        printf("\n");
        printf("argument num is %d\n",(*object)[i].argc);
    }
        
    
    return;
} 




int main(int argc, char *argv[])
{
    int commandnum;
    int currentjob = 0;
    size_t buffsize = 512;
    struct job* joblist  = (struct job*)malloc(sizeof(struct job));
    struct job* currentnode;
    joblist->handle = true;
    char *input = (char *)malloc(buffsize * sizeof(char));
    

    while (1) {
        currentnode = joblist;  
        struct command *list;
        printf("sshell$ ");
        getline(&input,&buffsize,stdin);   //Get user command
        char* userinput = (char*)malloc(strlen(input)*sizeof(char));
        strcpy(userinput,input);
        if (!isatty(STDIN_FILENO)) {
            printf("%s",input);
            fflush(stdout);
        }   
        commandnum = pipesize(input);
        parse(&list,input,commandnum);
        //printcontent(&list,commandnum);
        if (!input_errordetect(&list,input,commandnum)) {
            if (currentjob == 0) {
                modsys(&list,commandnum,userinput,&currentjob,currentnode,true);
            }
            else {
                modsys(&list,commandnum,userinput,&currentjob,currentnode,false);
            }
        }
        memset(userinput,0,buffsize);
        memset(input,0,buffsize); 
    }

    return EXIT_SUCCESS;
}
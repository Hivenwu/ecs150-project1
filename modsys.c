#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"
#define buffsize 100


enum execu_type execu_determine(struct command **object,int commandnum) {
    
    if (!strcmp((*object)[0].args[0],"date")) {
        return COMMAND_DATE;
    }
    else if (!strcmp((*object)[0].args[0],"ls")) {  
        return COMMAND_LS;
    }
    else if (!strcmp((*object)[0].args[0],"cd")) {
        return COMMAND_CD;
    }
    else if (!strcmp((*object)[0].args[0],"pwd")) {
        return COMMAND_PWD;
    }
    else if (!strcmp((*object)[0].args[0],"exit")) {
        return COMMAND_EXIT;
    }
    return COMMAND_UNDEF;
}

/* void command_pipe(struct command **obejct, int commandum) {
    int fd[2];
    int status;

    status = pipe(fd);
    for (int i = 0; i < commandnum; i = i + 1) {
    
        if (i == commandnum - 2) {
            foo_pipe((&object)[i],fd[0],true));
        }
        else {
            dup2(fd[1],foo_pipe((&object)[i],fd[0],false));
            dup2(fd[0],foo_pipe((&object)[i+1],fd[1],false));
        }
            
    }

    return;
}*/


void modsys(struct command **object,int commandnum) {
    pid_t PID;
    int status;
    int exitcode;
    int **fd;
    int i = 0;
    int fdcounter = 0;
    int* pid_array = (int *)malloc(commandnum*sizeof(int));



    if (commandnum == 1) {
        if (!strcmp((*object)[0].args[0],"pwd") || !strcmp((*object)[0].args[0],"cd") || !strcmp((*object)[0].args[0],"exit")) {
            external_modsys(object,true);
            return;
        }
        
        char* dir = (char *)malloc(buffsize * sizeof(char));
        strcat(dir,"/bin/");
        strcat(dir,(*object)[0].args[0]);
        PID = fork();
        if (PID == 0) {
            execv(dir,(*object)[0].args);
        }
        else {
            wait(&PID);
            fprintf(stderr, "+ completed '%s': [%d]\n", (*object)[0].args[0], EXIT_SUCCESS);
        }
    }

    else {

        PID = fork();
        if (PID == 0) {
            pipe_recur(object,&pid_array,0,0,commandnum);
            exit(0);
        }
        else{
            waitpid(PID,&status,0);
        }
        
        return;
    }

    return;
}


void pipe_recur(struct command **object,int** pid_array,int pipei,int commandi,int commandnum) {
  
    int status;
    int t = 0;
    pid_t PID;
    int fd[commandnum][2];
    char* dir = (char *)malloc(buffsize * sizeof(char));
    strcat(dir,"/bin/");

    for (int i = 0; i <= commandnum - 1; i = i+1) {
        pipe(fd[i]);
        PID = fork();
        if(PID == 0) {
            char* dir2 = (char *)malloc(buffsize * sizeof(char));
            if (i == 0) {
                close(fd[i][0]);
                dup2(fd[i][1],STDOUT_FILENO);
                close(fd[i][1]);
            }
            else if (i == commandnum - 1) {
                close(fd[i-1][1]);
                dup2(fd[i-1][0],STDIN_FILENO);
                close(fd[i-1][0]);
            }
            else {
                close(fd[i-1][1]);
                dup2(fd[i-1][0],STDIN_FILENO);
                close(fd[i-1][0]);
                close(fd[i][0]);
                dup2(fd[i][1],STDOUT_FILENO);
                close(fd[i][1]);
            }
                
            strcat(dir2,(*object)[i].args[0]);
            execvp(dir2,(*object)[i].args);
            perror("Error3: ");
            exit(EXIT_FAILURE);

        }
        else{
            (*pid_array)[i] = PID;
        }
        while (t <= i - 1) {
            close(fd[t][1]);
            close(fd[t][0]);
            t = t + 1;
        }
    }

    for (int k = 0; k <= commandnum - 2 ; k++) {
        waitpid((*pid_array)[k],&status,WUNTRACED);
    }

    return;
}
    

void external_modsys(struct command **object,bool print) {
    
    if (!strcmp((*object)[0].args[0],"pwd")) {
        char* address = (char *)malloc(buffsize * sizeof(char));
        getcwd(address,buffsize);
        if (print) {
            fprintf(stderr,"%s\n",address);
            fprintf(stderr, "+ completed 'pwd': [%d]\n", EXIT_SUCCESS);
        }
        else {
            fprintf(stdout,"%s\n",address);
        }
        return;
    }
    else if (!strcmp((*object)[0].args[0],"cd")) {
        int exitcode = chdir((*object)[0].args[1]);  
        if (exitcode != 0) {
            fprintf(stderr, "Error: no such directory\n");  
        }
        else {
            fprintf(stderr,"%s\n",getcwd((*object)[0].args[1],100));
        }
        if (print){
            fprintf(stderr, "+ completed '%s %s': [%d]\n",(*object)[0].args[0],(*object)[0].args[1], abs(exitcode));
        }
        return;
    }
    else if (!strcmp((*object)[0].args[0],"exit")) {
        printf("Bye...\n");
        exit(0);
    }
    else {
        fprintf(stderr, "Error: command not found\n"); 
        fprintf(stderr, "+ completed '%s %s': [%d]\n",(*object)[0].args[0],(*object)[0].args[1], EXIT_FAILURE);
        return;
    }
    return;
}
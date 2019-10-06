#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"
#define buffsize 100


enum execu_type execu_determine(struct command **object,int commandnum) {
    
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
    int exitcode;
    int *fd;
    int i = 0;
    int fdcounter = 0;



    if (commandnum == 1) {
        if (!strcmp((*object)[0].command,"pwd") || !strcmp((*object)[0].command,"cd") || !strcmp((*object)[0].command,"exit")) {
            external_modsys(object,true);
            return;
        }
        
        char* argv[] = {(*object)[0].command,(*object)[0].args,NULL};
        char* dir = (char *)malloc(buffsize * sizeof(char));
        strcat(dir,"/bin/");
        strcat(dir,(*object)[0].command);
        PID = fork();
        if (PID == 0) {
            execv(dir,argv);
        }
        else {
            wait(&PID);
            fprintf(stderr, "+ completed '%s': [%d]\n", (*object)[0].command, EXIT_SUCCESS);
        }
    }

    else {
        fd = (int *)malloc(commandnum*sizeof(int));
        while(pipe_recur(object,&fd,&i,commandnum) < commandnum) {
        }
    }

    
}

int pipe_recur(struct command **object,int **fd,int *i,int commandnum) {

        char* dir = (char *)malloc(buffsize * sizeof(char));
        strcat(dir,"/bin/");
        int savein = dup(STDIN_FILENO);
        int saveout = dup(STDOUT_FILENO);
        pid_t mainprocess = fork();


        if (mainprocess == 0) {
            if (*i == commandnum - 1) {
                close(STDIN_FILENO);            /* Close existing stdin */        
                dup((*fd)[0]);
                close((*fd)[0]);                     /* And replace it with the pipe */                         /* Close now unused file descriptor *            /* Replace stdout with the pipe */        
                char* argv[] = {(*object)[*i].command,(*object)[*i].args,NULL};  
                strcat(dir,(*object)[*i].command);
                if (!strcmp((*object)[*i].command,"pwd") || !strcmp((*object)[*i].command,"cd") || !strcmp((*object)[*i].command,"exit")) {
                    char* dir2 = (char *)malloc(buffsize * sizeof(char));
                    strcat(dir2,"./external_syscall");
                    char* argv1[] = {(*object)[*i].command,(*object)[*i].args,false,NULL}; 
                    execv(dir2,argv1);
                    perror("Error0:");
                    exit(1);
                }
                execv(dir,argv);
                perror("Error1:");
                exit(1);
            }
            else if (*i == commandnum - 2) {
                pipe(*fd);
                if (fork() != 0) {/* Parent */
                    if (commandnum != 2) {
                        close(STDIN_FILENO);
                        dup((*fd)[0]);
                        close((*fd)[0]);
                    }
                    close((*fd)[0]);
                    dup2((*fd)[1], STDOUT_FILENO);     /* Replace stdout with the pipe */        
                    close((*fd)[1]); 
                    char* argv2[] = {(*object)[*i].command,(*object)[*i].args,NULL};  
                    strcat(dir,(*object)[*i].command);
                    if (!strcmp((*object)[*i].command,"pwd") || !strcmp((*object)[*i].command,"cd") || !strcmp((*object)[*i].command,"exit")) {
                        char* dir2 = (char *)malloc(buffsize * sizeof(char));
                        strcat(dir2,"./external_syscall");
                        char* argv3[] = {(*object)[*i].command,(*object)[*i].args,false,NULL}; 
                        execv(dir2,argv3);
                        perror("Error2:");
                        exit(1);
                    } 
                    execv(dir,argv2);
                    perror("Error2:");
                    exit(1);
                } 
                else {/* Child */       
                    close(STDIN_FILENO);            /* Close existing stdin */        
                    dup((*fd)[0]);                     /* And replace it with the pipe */        
                    close((*fd)[0]);                   /* Close now unused file descriptor*/             /* Replace stdout with the pipe */        
                    char* argv4[] = {(*object)[*i+1].command,(*object)[*i+1].args,NULL};  
                    strcat(dir,(*object)[*i+1].command);
                    if (!strcmp((*object)[*i+1].command,"pwd") || !strcmp((*object)[*i+1].command,"cd") || !strcmp((*object)[*i+1].command,"exit")) {
                        char* dir2 = (char *)malloc(buffsize * sizeof(char));
                        strcat(dir2,"./external_syscall");
                        char* argv5[] = {(*object)[*i+1].command,(*object)[*i+1].args,false,NULL}; 
                        execv(dir2,argv5);
                        perror("Error2:");
                        exit(1);
                    }
                    execv(dir,argv4);
                    perror("Error3:");
                    exit(1);
                    }
                }
            else {
                pipe(*fd);
                if (fork() != 0) {/* Parent */
                    close(STDIN_FILENO);
                    dup((*fd)[0]); 
                    dup2((*fd)[1], STDOUT_FILENO);     /* Replace stdout with the pipe */        
                    char* argv6[] = {(*object)[*i].command,(*object)[*i].args,NULL};  
                    strcat(dir,(*object)[*i].command);
                    close((*fd)[0]);
                    close((*fd)[1]);
                    if (!strcmp((*object)[*i].command,"pwd") || !strcmp((*object)[*i].command,"cd") || !strcmp((*object)[*i].command,"exit")) {
                        char* dir2 = (char *)malloc(buffsize * sizeof(char));
                        strcat(dir2,"./external_syscall");
                        char* argv7[] = {(*object)[*i].command,(*object)[*i].args,false,NULL}; 
                        execv(dir2,argv7);
                        perror("Error2:");
                        exit(1);
                    }
                    execv(dir,argv6);
                    perror("Error4:");
                    exit(1);
                } 
                else {/* Child */       
                    close(STDIN_FILENO);            /* Close existing stdin */        
                    dup((*fd)[0]);
                    close((*fd)[0]);                     /* And replace it with the pipe */                         /* Close now unused file descriptor *            /* Replace stdout with the pipe */        
                    char* argv8[] = {(*object)[*i+1].command,(*object)[*i+1].args,NULL};  
                    strcat(dir,(*object)[*i+1].command);
                    dup2((*fd)[1],STDOUT_FILENO);
                    if (!strcmp((*object)[*i+1].command,"pwd") || !strcmp((*object)[*i+1].command,"cd") || !strcmp((*object)[*i+1].command,"exit")) {
                        char* dir2 = (char *)malloc(buffsize * sizeof(char));
                        strcat(dir2,"./external_syscall");
                        char* argv9[] = {(*object)[*i+1].command,(*object)[*i+1].args,false,NULL}; 
                        execv(dir2,argv9);
                        perror("Error2:");
                        exit(1);
                    }
                    execv(dir,argv8);
                    perror("Error5:");
                    exit(1);
                }        
            }
        }
        else {
            wait(&mainprocess);
            dup2(savein,STDIN_FILENO);
            dup2(saveout,STDOUT_FILENO);
            *i = *i + 2;
            return *i;
        }
        *i = *i + 2;
        return *i;
}

void external_modsys(struct command **object,bool print) {
    
    if (!strcmp((*object)[0].command,"pwd")) {
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
    else if (!strcmp((*object)[0].command,"cd")) {
        int exitcode = chdir((*object)[0].args);  
        if (exitcode != 0) {
            fprintf(stderr, "Error: no such directory\n");  
        }
        else {
            fprintf(stderr,"%s\n",getcwd((*object)[0].args,100));
        }
        if (print){
            fprintf(stderr, "+ completed '%s %s': [%d]\n",(*object)[0].command,(*object)[0].args, abs(exitcode));
        }
        return;
    }
    else if (!strcmp((*object)[0].command,"exit")) {
        printf("Bye...\n");
        exit(0);
    }
    else {
        fprintf(stderr, "Error: command not found\n"); 
        fprintf(stderr, "+ completed '%s %s': [%d]\n",(*object)[0].command,(*object)[0].args, EXIT_FAILURE);
        return;
    }
    return;
}
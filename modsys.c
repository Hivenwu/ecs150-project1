#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include "modsys.h"
#define buffsize 100


void printcompletemes(char* input,int exitcode,struct job* current,bool background,bool pipe,int commandnum,int* exitcode_arr) {
    if (background) {
        fprintf(stderr,"+ completed ");
        for (int i = 0; i < (*current).argc - 1; i++) {
            if (i == 0) {
               fprintf(stderr,"'%s ",(*current).args[i]); 
            }
            else if (i == (*current).argc - 2){
               fprintf(stderr,"%s&' [%d]\n",(*current).args[i],exitcode); 
            }
            else {
                fprintf(stderr,"%s ",(*current).args[i]);
            }
        }
    }
    else if (pipe) {
        input[strlen(input)-1] = '\0';
        fprintf(stderr,"+ completed '%s' ",input);
        for (int i = 0; i < commandnum;i++) {
            if (i == commandnum - 1) {
                fprintf(stderr,"[%d]\n",exitcode_arr[i]);
            }
            else {
                fprintf(stderr,"[%d]",exitcode_arr[i]);
            }
        }
    }
    else {
        input[strlen(input)-1] = '\0';
        fprintf(stderr,"+ completed '%s' [%d]\n",input,exitcode);
    }
    return;
}

bool checkbackground(struct  command **object) {
    for (int i = 1; i < (*object)[0].argc - 1; i = i + 1) {
        for (int k = 0; k < strlen((*object)[0].args[i]); k++) {
             if ((*object)[0].args[i][k] == '&') {
                (*object)[0].args[i][k] = '\0';
                (*object)[0].args[i][k+1] = 0;
                return true;
             }
        }
    }
    return false;
}

void waitforbackground(int* currentjob,struct command **object,struct job* current,char* input,bool doublecheck) {
    int status;

    if (!doublecheck) {
        if (waitpid(-1,&status,WNOHANG) != 0 && current->handle == false) {
            current->handle = true;
            printcompletemes(input,EXIT_SUCCESS,current,true,false,0,NULL);
        }
        while (current->next != NULL) {
            if (waitpid(-1,&status,WNOHANG) != 0 && current->handle == false) {
                current->handle = true;
                printcompletemes(input,EXIT_SUCCESS,current,true,false,0,NULL);
            }
            current = current->next;
        }
        if (waitpid(-1,&status,WNOHANG) != 0 && current->handle == false) {
            current->handle = true;
            printcompletemes(input,EXIT_SUCCESS,current,true,false,0,NULL);
        }
    }
    else {
        if (waitpid(-1,&status,WNOHANG) != 0 && current->handle == false) {
            current->handle = true;
        }
        while (current->next != NULL) {
            if (waitpid(-1,&status,WNOHANG) != 0 && current->handle == false) {
                current->handle = true;
            }
            current = current->next;
        }
        if (waitpid(-1,&status,WNOHANG) != 0 && current->handle == false) {
            current->handle = true;
        }
    }
        
    
    return;
}

void modsys(struct command **object,int commandnum,char* input,int* currentjob,struct job* current,bool first) {
    pid_t PID;
    int status;
    int exitcode;
    int **fd;
    int i = 0;
    int fdcounter = 0;
    int* pid_array = (int *)malloc(commandnum*sizeof(int));

    if (commandnum == 1) {
        char* dir = (char *)malloc(buffsize * sizeof(char));
        strcat(dir,"/bin/");
        strcat(dir,(*object)[0].args[0]);

        if (!strcmp((*object)[0].args[0],"pwd") || !strcmp((*object)[0].args[0],"cd") || !strcmp((*object)[0].args[0],"exit")) {
            external_modsys(object,true,input,current);
            return;
        }

         if (checkbackground(object)) {
            PID = fork();
            if (PID == 0) {
                if ((*object)[0].argc >= 18) {
                    perror("Error: too many process arguments\n");
                    return;
                }
                execv(dir,(*object)[0].args);
                perror("Error: ");
            }
            else {
                waitpid(-1,&status,WNOHANG);
                exitcode = WEXITSTATUS(status);
                *currentjob = *currentjob + 1;
                if (first == true) {
                    while (current->next != NULL) {
                        current = current -> next;
                    }
                    (*current).processid = PID;
                    (*current).args = (char**)malloc(((*object)[0].argc)*sizeof(char*));
                    (*current).argc = (*object)[0].argc;
                    (*current).handle = false; 
                    (*current).next = NULL;
                    for (int i = 0; i < (*object)[0].argc - 1; i = i + 1) {
                        (*current).args[i] = (char*)malloc(strlen((*object)[0].args[i])*sizeof(char));
                        strcpy((*current).args[i],(*object)[0].args[i]);
                    }
                }
                else {
                    while (current->next != NULL) {
                        current = current -> next;
                    }
                    struct job* nextnode = (struct job*)malloc(sizeof(struct job));
                    (*nextnode).processid = PID;
                    (*nextnode).args = (char**)malloc(((*object)[0].argc)*sizeof(char*));
                    (*nextnode).argc = (*object)[0].argc;
                    (*nextnode).handle = false; 
                    for (int i = 0; i < (*object)[0].argc - 1; i = i + 1) {
                        (*nextnode).args[i] = (char*)malloc(strlen((*object)[0].args[i])*sizeof(char));
                        strcpy((*nextnode).args[i],(*object)[0].args[i]);
                    }
                    nextnode -> next = NULL;
                    current->next = nextnode;
                }

                return;
            }

        }

        else {
            PID = fork();
            if (PID == 0) {
                waitforbackground(currentjob,object,current,input,false);
                if ((*object)[0].argc >= 18) {
                    perror("Error");
                    return;
                }
                else if (!strcmp(input,"\n")) {
                    return; 
                }
                execv(dir,(*object)[0].args);
                perror("Error");
                printcompletemes(input,EXIT_FAILURE,current,false,false,0,NULL);
            }
            else {
                waitpid(PID,&status,0);
                exitcode = WEXITSTATUS(status);
                printcompletemes(input,exitcode,current,false,false,0,NULL);
            }
        }
    }

    else {
        for (int i = 0; i < commandnum; i++) {
            if ((*object)[i].argc == 0) {
                perror("Error: missing command");
                return;
            }
            if ((strlen((*object)[i].args[0]) == 0)) {
                perror("Error: missing command");
                return;
            }     
        }

        int* exitcode = (int*)malloc(commandnum*sizeof(int));
        PID = fork();
        if (PID == 0) {
            
            
            pipe_recur(object,&pid_array,0,0,commandnum,input,exitcode);
            exit(0);
        }
        else{
            waitpid(PID,&status,WUNTRACED);
            printcompletemes(input,EXIT_SUCCESS,NULL,false,true,commandnum,exitcode);
        }
        
        return;
    }

    return;
}


void pipe_recur(struct command **object,int** pid_array,int pipei,int commandi,int commandnum,char* input,int* exitcode) {
  
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
            
            if (i == commandnum - 1) {
                strcat(dir2,(*object)[i].args[0]);
                if ((*object)[i].argc >= 18) {
                    perror("Error: too many process arguments\n");
                    exit(1);
                }
                execvp(dir2,(*object)[i].args);
                exit(errno);
            }
            else {
                strcat(dir2,(*object)[i].args[0]);
                if ((*object)[i].argc >= 18) {
                    perror("Error: too many process arguments\n");
                    exit(1);
                }
                execvp(dir2,(*object)[i].args);
                exit(errno);
            }
            
        }
        else{
            (*pid_array)[i] = PID;
        }
        while (t <= i - 1) {
            close(fd[t][1]);
            close(fd[t][0]);
            t = t + 1;
        }
        if (i == commandnum - 1) {
            waitpid(PID,&status,0);
            exitcode[i] = WEXITSTATUS(status);
        }
    }
    

    for (int k = 0; k <= commandnum - 2 ; k++) {
        waitpid((*pid_array)[k],&status,0);
        exitcode[k] = WEXITSTATUS(status);
    }

    return;
}
    

void external_modsys(struct command **object,bool print,char* input,struct job* current) {
    
    if (!strcmp((*object)[0].args[0],"pwd")) {
        char* address = (char *)malloc(buffsize * sizeof(char));
        getcwd(address,buffsize);
        if (print) {
            fprintf(stdout,"%s\n",address);
            printcompletemes(input,EXIT_SUCCESS,NULL,false,false,0,NULL);
        }
        else {
            fprintf(stdout,"%s\n",address);
        }
        return;
    }
    else if (!strcmp((*object)[0].args[0],"cd")) {
        char* currentcommand = (char *)malloc(strlen((*object)[0].args[1])*sizeof(char));
        int exitcode = chdir((*object)[0].args[1]);  
        if (exitcode != 0) {
            fprintf(stderr, "Error: no such directory\n");  
        }
        if (print){
            printcompletemes(input,abs(exitcode),NULL,false,false,0,NULL);
        }
        return;
    }
    else if (!strcmp((*object)[0].args[0],"exit")) {

        if (current->handle == false) {
            fprintf(stderr,"Error: active jobs still running\n");
            printcompletemes(input,1,NULL,false,false,0,NULL);
            return;
        }
        while (current->next != NULL) {
            if (current->handle == false) {
                fprintf(stderr,"Error: active jobs still running\n");
                printcompletemes(input,1,NULL,false,false,0,NULL);
                return;
            }
            current = current->next;
        }
        fprintf(stderr, "Bye...\n");
        exit(0);
    }
    else {
        fprintf(stderr, "Error: command not found\n"); 
        printcompletemes(input,EXIT_FAILURE,NULL,false,false,0,NULL);
        return;
    }
    return;
}
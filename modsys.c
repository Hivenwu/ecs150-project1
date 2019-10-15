#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "modsys.h"
#include "background.h"
#define buffsize 100
#define cmdsize 512

bool inout_re(struct command** object,struct job* current,int index,int commandnum,int* currentjob,char* input,bool ispipe) {
    int breaknum;
    int redir = false;
    char** args;
    char* dir = (char *)malloc(buffsize * sizeof(char));
    strcat(dir,"/bin/");
    strcat(dir,(*object)[index].args[0]);
    args = malloc(cmdsize*sizeof(char*));
    for (int i = 0; i <= (*object)[index].argc; i++) {
        args[i] = (char*)malloc(cmdsize*sizeof(char));
    }
    for (int j = 0; j < (*object)[index].argc - 1; j = j + 1) {
        if (!strcmp((*object)[index].args[j],"<")) {
            redir = true;
            for (int i = 0; i < (*object)[index].argc; i++) {
                if (!strcmp((*object)[index].args[i],"<")) {
                    breaknum = i;
                    break;
                }
                strcpy(args[i],(*object)[index].args[i]);
            }
            args[breaknum] = NULL;
            int filein = open((*object)[index].args[j+1],O_RDONLY); //Input redirection start
            lseek(filein, 0, SEEK_SET);
            dup2(filein,STDIN_FILENO);
            close(filein); //INPUTREDIRECTION END
            break;
        }
        else if (!strcmp((*object)[index].args[j],">")) {
            redir = true;
            for (int i = 0; i < (*object)[index].argc; i ++) {
                if (!strcmp((*object)[index].args[i],">")) {
                    breaknum = i;
                    break;
                }
                strcpy(args[i],(*object)[index].args[i]);
            }
            args[breaknum] = NULL;
            int fileout = open((*object)[index].args[j+1], O_RDWR|O_CREAT|O_APPEND, 0644);
            lseek(fileout, 0, SEEK_CUR);
            dup2(fileout,STDOUT_FILENO);
            close(fileout);
            break;
        }
    }

    if (redir) {
        execv(dir,args);
        perror("Error");
        printcompletemes(input,EXIT_FAILURE,current,false,false,0,NULL);
        exit(errno);
    }
    else {
        return false;
    }
                
                
    return false;
}

void printcompletemes(char* input,int exitcode,struct job* current,bool background,bool pipe,int commandnum,int* exitcode_arr) {
    if (background) {
        if ((*current).pipe) {
            (*current).input[strlen((*current).input)-1] = '\0';
            fprintf(stderr,"+ completed '%s' ",(*current).input);
            for (int i = 0; i <= (*current).commandnum ;i++) {
                
                if (i == (*current).commandnum) {
                    fprintf(stderr,"[%d]\n",(*current).exitcode_arr[i]);
                }
                else {
                    fprintf(stderr,"[%d]",(*current).exitcode_arr[i]);
                }
            }
        }
        else {
            (*current).input[strlen((*current).input)-1] = '\0';
            fprintf(stderr,"+ completed '%s' [%d]\n",(*current).input,exitcode);
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
        if (!strcmp(input,"\n")) {
            return;
        }
        input[strlen(input)-1] = '\0';
        fprintf(stderr,"+ completed '%s' [%d]\n",input,exitcode);
    }
    return;
}



void modsys(struct command **object,int commandnum,char* input,int* currentjob,struct job* current,bool first) {
    pid_t PID;
    int status;
    int exitcode;
    int* pid_array = (int *)malloc(commandnum*sizeof(int));
  
    if (commandnum == 1) {
        char* dir = (char *)malloc(buffsize * sizeof(char));
        strcat(dir,"/bin/");
        strcat(dir,(*object)[0].args[0]);

        if (!strcmp((*object)[0].args[0],"pwd") || !strcmp((*object)[0].args[0],"cd") || !strcmp((*object)[0].args[0],"exit")) {
            external_modsys(object,true,input,current);
            return;
        }

        if (checkbackground(object,0)) {
            PID = fork();
            if (PID == 0) {
                execv(dir,(*object)[0].args);
                perror("Error: ");
                exit(errno);
            }
            else {
                waitpid(-1,&status,WNOHANG);
                exitcode = WEXITSTATUS(status);
                *currentjob = *currentjob + 1;
                addnode(object,current,input,first,PID,0,NULL,false);
                return;
            }
        }

        else {
            PID = fork();
            if (PID == 0) {
                waitforbackground(currentjob,object,current,input,false);
                inout_re(object,current,0,commandnum,currentjob,input,false);
                if (!strcmp(input,"\n")) {
                    exit(0);
                }   
                execv(dir,(*object)[0].args);
                fprintf(stderr,"Error: command not found\n");
                exit(EXIT_FAILURE);
            }
            else {
                waitpid(PID,&status,0);
                waitforbackground(currentjob,object,current,input,true);
                exitcode = WEXITSTATUS(status);
                printcompletemes(input,exitcode,current,false,false,0,NULL);
            }
        }
    }

    else {
        int* exitcode = (int*)malloc(commandnum*sizeof(int));
        pipe_recur(object,current,currentjob,&pid_array,commandnum,input,exitcode,first);

        return;
    }

    return;
}


bool pipe_recur(struct command **object,struct job* current,int* currentjob,int** pid_array,int commandnum,char* input,int* exitcode,bool first) {
    bool background = false;
    bool exitp = false;
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
                if (inout_re(object,current,i,commandnum,currentjob,input,true)) {
                    exit(1);
                }
                if (!checkbackground(object,i)) {
                    waitforbackground(currentjob,object,current,input,false);
                }
                strcat(dir2,(*object)[i].args[0]);
                execvp(dir2,(*object)[i].args);
                perror("Error");
                exit(errno);
            }
            else {
                if (i == 0) {
                    if (inout_re(object,current,i,commandnum,currentjob,input,true)) {
                        exit(1);
                    }
                }
                strcat(dir2,(*object)[i].args[0]);
                execvp(dir2,(*object)[i].args);
                perror("Error");
                exit(errno);
            }
            
        }
        else{
            (*pid_array)[i] = PID;
            if (exitp) {
                return false;
            }
        }
        while (t <= i - 1) {
            close(fd[t][1]);
            close(fd[t][0]);
            t = t + 1;
        }
    }
    

    for (int k = 0; k <= commandnum - 2 ; k++) {
        waitpid((*pid_array)[k],&status,0);
        exitcode[k] = WEXITSTATUS(status);
    }

    if (checkbackground(object,commandnum-1)) {
        *currentjob = *currentjob + 1;
        addnode(object,current,input,first,PID,commandnum-1,exitcode,true);
        background = true;
    }
    if (!background) {
        waitpid(PID,&status,0);
        exitcode[commandnum-1] = WEXITSTATUS(status);
        printcompletemes(input,EXIT_SUCCESS,NULL,false,true,commandnum,exitcode);
    }        

    return background;
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
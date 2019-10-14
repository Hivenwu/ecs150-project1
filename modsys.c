#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "modsys.h"
#define buffsize 100
#define cmdsize 512


bool inout_re(struct command** object,struct job* current,int index,int* currentjob,char* input) {
    int breaknum;
    int redir = false;
    waitforbackground(currentjob,object,current,input,false);
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
            if (filein < 0){
                perror("Couldn't open the input source file.");
                exit(0);
            }
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

            if (fileout < 0){
                perror("Couldn't open the output destination file.");
                exit(0);
            }
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

void addnode(struct command **object,struct job* current,char* userinput,bool first,pid_t PID,int index,int* exitcode,bool pipe) {
    if (first == true) {
        (*current).input = (char*)malloc(cmdsize*sizeof(char));
        (*current).exitcode_arr = (int*)malloc(cmdsize*sizeof(int));
        for (int i = 0; i < index; i++) {
            (*current).exitcode_arr[i] =  exitcode[i];
        }
        strcpy((*current).input,userinput);
        (*current).processid = PID;
        (*current).pipe = pipe;
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
        (*nextnode).input = (char*)malloc(cmdsize*sizeof(char));
        (*nextnode).exitcode_arr= (int*)malloc(cmdsize*sizeof(int));
        for (int i = 0; i < index; i++) {
            (*nextnode).exitcode_arr[i] =  exitcode[i];
        }
        strcpy((*nextnode).input,userinput);
        (*nextnode).processid = PID;
        (*nextnode).pipe = pipe;
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
}

void printcompletemes(char* input,int exitcode,struct job* current,bool background,bool pipe,int commandnum,int* exitcode_arr) {
    if (background) {
        if ((*current).pipe) {
            (*current).input[strlen((*current).input)-1] = '\0';
            fprintf(stderr,"+ completed '%s' ",(*current).input);
            printf("fuck!");
            for (int i = 0; i < commandnum - 1;i++) {
                
                if (i == commandnum - 2) {
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

bool checkbackground(struct command **object,int index) {
    
    for (int i = 1; i < (*object)[index].argc - 1; i = i + 1) {
        for (int k = 0; k < strlen((*object)[index].args[i]); k++) {
            if ((*object)[index].args[i][k] == '&') {
                (*object)[index].args[i][k] = '\0';
                (*object)[index].args[i][k+1] = 0;
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
    bool background;
    bool redir = false;
    const int size = 512;
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

         if (checkbackground(object,0)) {
            PID = fork();
            if (PID == 0) {
                if ((*object)[0].argc >= 18) {
                    perror("Error: too many process arguments\n");
                    return;
                }
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
                inout_re(object,current,0,currentjob,input);
                if ((*object)[0].argc >= 18) {
                    perror("Error");
                    return;
                }
                if (!strcmp(input,"\n")) {
                    exit(0);
                }   
                execv(dir,(*object)[0].args);
                perror("Error: ");
                exit(errno);
            }
            else {
                waitpid(PID,&status,0);
                exitcode = WEXITSTATUS(status);
                waitforbackground(currentjob,object,current,input,true);
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
        pipe_recur(object,current,currentjob,&pid_array,commandnum,input,exitcode,first);

        return;
    }

    return;
}


bool pipe_recur(struct command **object,struct job* current,int* currentjob,int** pid_array,int commandnum,char* input,int* exitcode,bool first) {
    bool background = false;
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
                inout_re(object,current,i,currentjob,input);
                if ((*object)[i].argc >= 18) {
                    perror("Error: too many process arguments\n");
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
                    inout_re(object,current,i,currentjob,input);
                }
                strcat(dir2,(*object)[i].args[0]);
                if ((*object)[i].argc >= 18) {
                    perror("Error: too many process arguments\n");
                    exit(1);
                }
                execvp(dir2,(*object)[i].args);
                perror("Error");
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
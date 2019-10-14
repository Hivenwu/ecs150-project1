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

void addnode(struct command **object,struct job* current,char* userinput,bool first,pid_t PID,int index,int* exitcode,bool pipe) {
    if (first == true) {
        (*current).input = (char*)malloc(cmdsize*sizeof(char));
        if (pipe){
            (*current).exitcode_arr = (int*)malloc(cmdsize*sizeof(int));
            for (int i = 0; i < index; i++) {
                (*current).exitcode_arr[i] =  exitcode[i];
            }
        }
        else {
            (*current).exitcode_arr = NULL;
        }
        strcpy((*current).input,userinput);
        (*current).commandnum = index;
        (*current).processid = PID;
        (*current).pipe = pipe;
        (*current).handle = false; 
        (*current).next = NULL;
    }
    else {
        while (current->next != NULL) {
            current = current -> next;
        }
        struct job* nextnode = (struct job*)malloc(sizeof(struct job));
        (*nextnode).input = (char*)malloc(cmdsize*sizeof(char));
        if (pipe){
            (*nextnode).exitcode_arr = (int*)malloc(cmdsize*sizeof(int));
            for (int i = 0; i < index; i++) {
                (*nextnode).exitcode_arr[i] =  exitcode[i];
            }
        }
        else {
            (*nextnode).exitcode_arr = NULL;
        }
        strcpy((*nextnode).input,userinput);
        (*nextnode).commandnum = index;
        (*nextnode).processid = PID;
        (*nextnode).pipe = pipe;
        (*nextnode).handle = false; 
        nextnode -> next = NULL;
        current->next = nextnode;
    }
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
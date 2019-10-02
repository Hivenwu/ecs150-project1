#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"

void parse(struct command *object,size_t buffsize) { //Parse user's command-line input and write it to struct
    int numc = 0;
    int numarg = 0;
    int command = 0;
    int arguments = 0;
    char* buffc;
    char* buffarg;

    buffc = (char *)malloc(buffsize * sizeof(char));
    buffarg = (char *)malloc(buffsize * sizeof(char));

    for (int index = 0; index < buffsize; index = index + 1) {
        if ((object->input)[index] != ' ' && arguments == 0) {
            command = 1;
            buffc[numc] = object->input[index];
            numc = numc + 1;
        }
        else if (command && (object->input)[index] == ' ' && (object->input)[index+1] != '\n' && (object->input)[index+1] != ' ') {
            arguments = 1;
        }
        else if ((object->input)[index] != ' ' && arguments != 0 && command != 0) {
            if ((object->input)[index] == '\n') {
                break;
            }
            buffarg[numarg] = object->input[index];
            numarg = numarg + 1;
        }

        
    }
    
    strcpy(object->input,buffc);
    strcpy(object->arguments,buffarg);
    return;
}


int main(int argc, char *argv[])
{
    char *buff;
    struct command instance;
    size_t buffsize = 40;
    instance.input = (char *)malloc(buffsize * sizeof(char));
    instance.arguments = (char *)malloc(buffsize * sizeof(char));

    while (1) {
        printf("$sshell: ");
        getline(&(instance.input),&buffsize,stdin);   //Get user command
        parse(&instance,buffsize);
        assignval(&instance);
        //The switch statement is used to determine which command-line operation to run.
        modsys(&instance);
    }
    return EXIT_SUCCESS;
}
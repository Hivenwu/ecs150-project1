#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "modsys.h"

void parse(struct command *object,size_t buffsize) { //Parse user's command-line input and write it to struct
    int num = 0;
    char* buff;

    buff = (char *)malloc(buffsize * sizeof(char));

    for (int index = 0; index < buffsize; index = index + 1) {
        if ((object->input)[index] != ' ') {
            buff[num] = object->input[index];
            num = num + 1;
        }
    }
    
    strcpy(object->input,buff);

    return;
}


int main(int argc, char *argv[])
{
    char *buff;
    int retval;
    struct command instance;
    size_t buffsize = 40;

    while (1) {
        printf("$sshell: ");
        instance.input = (char *)malloc(buffsize * sizeof(char));
        getline(&(instance.input),&buffsize,stdin);   //Get user command
        parse(&instance,buffsize);
        assignval(&instance);
        //The switch statement is used to determine which command-line operation to run.
        modsys(&instance);
    }
    return EXIT_SUCCESS;
}
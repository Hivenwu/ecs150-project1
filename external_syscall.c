#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define buffsize 512

int main(int argc, char *argv[]) {
    
    if (!strcmp(argv[0],"pwd")) {
        char* address = (char *)malloc(buffsize * sizeof(char));
        getcwd(address,buffsize);
        if (argv[2]) {
            fprintf(stderr,"%s\n",address);
            fprintf(stderr, "+ completed 'pwd': [%d]\n", EXIT_SUCCESS);
        }
        else {
            fprintf(stdout,"%s\n",address);
        }
        return EXIT_SUCCESS;
    }

    else if (!strcmp(argv[0],"cd")) {
        int exitcode = chdir(argv[1]);  
        if (exitcode != 0) {
            fprintf(stderr, "Error: no such directory\n");  
        }
        else {
            fprintf(stderr,"%s\n",getcwd(argv[1],512));
        }
        if (argv[2]){
            fprintf(stderr, "+ completed '%s %s': [%d]\n",argv[0],argv[1], abs(exitcode));
            return EXIT_SUCCESS;
        }
        return EXIT_SUCCESS;
    }
    else if (!strcmp(argv[0],"exit")) {
        printf("Bye...\n");
        exit(0);
    }
    else {
        fprintf(stderr, "Error: command not found\n"); 
        fprintf(stderr, "+ completed '%s %s': [%d]\n",argv[0],argv[1], EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
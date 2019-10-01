#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


struct command {
    char* args;
    char* cmd;
    int retval;
};

void parse(struct command *object,size_t buffsize) {
    int num = 0;
    char* buff;
    char* line;
    buff = (char *)malloc(buffsize * sizeof(char));

    for (int index = 0; index < buffsize; index = index + 1) {
        if ((object->args)[index] != ' ') {
            buff[num] = object->args[index];
            num = num + 1;
        }
    }
    
    strcpy(object->args,buff);

    return;
}

int main(int argc, char *argv[])
{
    char *cmd1 = "/bin/date";
    char *cmd2 = "/bin/ls";
    char *args1[] = {cmd1, "-u", NULL};
    char *args2[] = {cmd2, ".", NULL};
    char *buff;
    char *line;
    int retval;
    struct command instance;
    size_t buffsize = 40;
    pid_t PID;

    while (1) {
        printf("$sshell: ");
        instance.args = (char *)malloc(buffsize * sizeof(char));
        getline(&(instance.args),&buffsize,stdin);   //Get user command
        parse(&instance,buffsize);

        if (!strcmp(instance.args,"date\n")) {   //Accouts for /bin/date -u command
            PID = fork();
            if (PID == 0) {
                retval = execv(cmd1,args1);
            }
            else {
                wait(&PID);
                fprintf(stderr, "+ completed '%s': [%d]\n", cmd1, retval);
            }
        }

        else if (!strcmp(instance.args,"ls\n")) {
            PID = fork();
            if (PID == 0) {
                retval = execv(cmd2,args2);
            }
            else {
                wait(&PID);
                fprintf(stderr, "+ completed '%s': [%d]\n", cmd2, retval);
            }
        }

        else {
            perror("command not found");
        }
    }

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *cmd = "/bin/date";
    char *date = "date";
    char *args[] = {cmd, "-u", NULL};
    char *buff;
    char *line;
    int retval;
    size_t buffsize = 40;
    pid_t PID;

    buff = (char *)malloc(buffsize * sizeof(char));

    while (1) {
        printf("$sshell: ");
        getline(&buff,&buffsize,stdin);   //Get user command

        if (strcmp(buff,date)) {   //Accouts for /bin/date -u command
            PID = fork();
            if (PID == 0) {
                retval = execv(cmd,args);
            }
            else {
                wait(&PID);
                fprintf(stderr, "+ completed '%s': [%d]\n", cmd, retval);
            }
        }
    }

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *cmd = "/bin/date -u";
    char *args[] = {cmd, ".", NULL};
    int retval;

    retval = system(cmd);
    fprintf(stdout, "+ completed '%s': [%d]\n", cmd, retval);

    return EXIT_SUCCESS;
}
#ifndef BACKGROUND_H
#define BACKGROUND_H
#define buffsize 100
#define cmdsize 512
#include <stdbool.h>

void addnode(struct command **object,struct job* current,char* userinput,bool first,pid_t PID,int index,int* exitcode,bool pipe);
bool checkbackground(struct command **object,int index);
void waitforbackground(int* currentjob,struct command **object,struct job* current,char* input,bool doublecheck);

#endif
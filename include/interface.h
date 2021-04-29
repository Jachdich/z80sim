#ifndef __INTERFACE_H
#define __INTERFACE_H
#include "../include/main.h"
void stepPrompt(CPU *cpu);
void executeDebugCommand(char *total, CPU *cpu);
int readFile(char *fname, CPU *cpu);
void usage(char *name);
char *parseArgs(int argc, char ** argv, CPU *cpu);
#endif
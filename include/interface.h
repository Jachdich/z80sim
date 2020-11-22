#ifndef __INTERFACE_H
#define __INTERFACE_H
void stepPrompt();
void executeDebugCommand(std::string total);
int readFile(char * fname);
void usage(char * name);
char * parseArgs(int argc, char ** argv);
#endif
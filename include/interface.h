#ifndef __INTERFACE_H
#define __INTERFACE_H
void stepPrompt();
void executeDebugCommand(std::string total);
int readFile(std::string fname);
void usage(std::string name);
std::string parseArgs(int argc, char ** argv);
#endif
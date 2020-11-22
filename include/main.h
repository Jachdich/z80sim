#ifndef __MAIN_H
#define __MAIN_H
#include "regs.h"

extern reg_t mem[65536];

extern reg_t lastInstr;

extern bool halt;
extern bool debug;
extern bool step;

extern std::string eachTimeCommand;
#endif
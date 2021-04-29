#ifndef __MAIN_H
#define __MAIN_H
#include "regs.h"
#include <stdint.h>

#define bool uint8_t
#define true 1
#define false 0

typedef struct {
    //actual CPU implementation
    uint8_t mem[65536];
    uint8_t regs[9];
    uint8_t regs_[9];

    uint8_t reg_i;
    uint8_t reg_r;

    uint16_t idxregs[2];

    uint16_t pc;
    uint16_t sp;

    uint16_t pc_;
    uint16_t sp_;

    //debug & implementation
    uint8_t lastInstr;
    char *eachTimeCommand;
    bool halt;
    bool debug;
    bool step;
} CPU;
#endif
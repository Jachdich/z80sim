#ifndef __REGS_H
#define __REGS_H
#include "main.h"
#include <stdint.h>

#define CARRY_FLAG 0
#define ADD_SUB_FLAG 1
#define PARITY_OVF_FLAG 2
#define HALF_CARRY 4
#define ZERO_FLAG 6
#define SIGN_FLAG 7

#define A 0b111
#define B 0b000
#define C 0b001
#define D 0b010
#define E 0b011
#define H 0b100
#define L 0b101
#define F 0b1000

#define IX 0
#define IY 1

void sf(uint8_t flag, CPU *cpu);
void rf(uint8_t flag, CPU *cpu);
bool gf(uint8_t flag, CPU *cpu);

void printRegs(CPU *cpu);
uint16_t getReg16(uint8_t num, CPU *cpu);
void setReg16(uint8_t num, uint16_t value, CPU *cpu);
bool getCondition(uint8_t num, CPU *cpu);

#endif
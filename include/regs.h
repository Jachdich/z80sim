#ifndef __REGS_H
#define __REGS_H

typedef unsigned char reg_t;
typedef unsigned short reg16_t;
typedef signed char reg_signed_t;

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

extern reg_t REGS[9];
extern reg_t REGS_[9];

extern reg_t reg_i;
extern reg_t reg_r;

extern reg16_t IDXREGS[2];

extern reg16_t pc;
extern reg16_t sp;

extern reg16_t pc_;
extern reg16_t sp_;

void sf(reg_t flag);
void rf(reg_t flag);
void print_regs();
reg16_t getReg16(reg_t num);
void setReg16(reg_t num, reg16_t value);
bool getCondition(reg_t num);
bool gf(reg_t flag);

#endif
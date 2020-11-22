#include <stdio.h>
#include "regs.h"

reg_t REGS[9];
reg_t REGS_[9];

reg_t reg_i;
reg_t reg_r;

reg16_t IDXREGS[2];

reg16_t pc;
reg16_t sp;

reg16_t pc_;
reg16_t sp_;

void print_regs() {
    printf("A:   0x%02x", REGS[A]); printf("   A':   0x%02x\n", REGS_[A]);
    printf("B:   0x%02x", REGS[B]); printf("   B':   0x%02x\n", REGS_[B]);
    printf("C:   0x%02x", REGS[C]); printf("   C':   0x%02x\n", REGS_[C]);
    printf("D:   0x%02x", REGS[D]); printf("   D':   0x%02x\n", REGS_[D]);
    printf("E:   0x%02x", REGS[E]); printf("   E':   0x%02x\n", REGS_[E]);
    printf("F:   0x%02x", REGS[F]); printf("   F':   0x%02x\n", REGS_[F]);
    printf("H:   0x%02x", REGS[H]); printf("   H':   0x%02x\n", REGS_[H]);
    printf("L:   0x%02x", REGS[L]); printf("   L':   0x%02x\n", REGS_[L]);
    printf("PC:  0x%04x", pc);      printf(" PC':  0x%04x\n", pc_);
    printf("SP:  0x%04x", sp);      printf(" SP':  0x%04x\n", sp_);
    printf("IX:  0x%04x", IDXREGS[IX]); printf(" IY:   0x%04x\n", IDXREGS[IY]);
}

void sf(reg_t flag) {
    REGS[F] |= 0x1 << flag;
}

void rf(reg_t flag) {
    REGS[F] &= !(0x1 << flag);
}

bool gf(reg_t flag) {
    return (REGS[F] >> flag) & 1;
}

reg16_t getReg16(reg_t num) {
    switch (num) {
        case 0b00:
            return REGS[B] << 8 | REGS[C];
        case 0b01:
            return REGS[D] << 8 | REGS[E];
        case 0b10:
            return REGS[H] << 8 | REGS[L];
        case 0b11:
            return sp;
        default:
            printf("ERROR: This is a bug. Please report it. Iinvalid reg pair passed to regs::getReg16() (value was %d)\n", num);
            return -1;
    }
}

bool getCondition(reg_t num) {
    switch (num) {
        case 0b000:
            return !gf(ZERO_FLAG);
        case 0b001:
            return gf(ZERO_FLAG);
        case 0b010:
            return !gf(CARRY_FLAG);
        case 0b011:
            return gf(CARRY_FLAG);
        case 0b100:
            return !gf(PARITY_OVF_FLAG);
        case 0b101:
            return gf(PARITY_OVF_FLAG);
        case 0b110:
            return !gf(SIGN_FLAG);
        case 0b111:
            return gf(SIGN_FLAG);
        default:
            printf("ERROR: Please report this bug. Invalid argument to regs::getCondition(reg_t condition)\n");
            return false;
    }
}

void setReg16(reg_t num, reg16_t value) {
    switch (num) {
        case 0b00:
            REGS[B] = value >> 8;
            REGS[C] = value & 0xFF;
            break;
        case 0b01:
            REGS[D] = value >> 8;
            REGS[E] = value & 0xFF;
            break;
        case 0b10:
            REGS[H] = value >> 8;
            REGS[L] = value & 0xFF;
            break;
        case 0b11:
            sp = value;
            break;
        default:
            printf("ERROR: This is a bug. Please report it to the developers. Extra info: invalid reg pair passed to main::setReg16() (value was %d)\n", num);
            return;
    }
}
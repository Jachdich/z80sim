#include <stdio.h>
#include "regs.h"

void printRegs(CPU *cpu) {
    printf("A:   0x%02x", cpu->regs[A]); printf("   A':   0x%02x\n", cpu->regs_[A]);
    printf("B:   0x%02x", cpu->regs[B]); printf("   B':   0x%02x\n", cpu->regs_[B]);
    printf("C:   0x%02x", cpu->regs[C]); printf("   C':   0x%02x\n", cpu->regs_[C]);
    printf("D:   0x%02x", cpu->regs[D]); printf("   D':   0x%02x\n", cpu->regs_[D]);
    printf("E:   0x%02x", cpu->regs[E]); printf("   E':   0x%02x\n", cpu->regs_[E]);
    printf("F:   0x%02x", cpu->regs[F]); printf("   F':   0x%02x\n", cpu->regs_[F]);
    printf("H:   0x%02x", cpu->regs[H]); printf("   H':   0x%02x\n", cpu->regs_[H]);
    printf("L:   0x%02x", cpu->regs[L]); printf("   L':   0x%02x\n", cpu->regs_[L]);
    printf("PC:  0x%04x", cpu->pc);      printf(" PC':  0x%04x\n", cpu->pc_);
    printf("SP:  0x%04x", cpu->sp);      printf(" SP':  0x%04x\n", cpu->sp_);
    printf("IX:  0x%04x", cpu->idxregs[IX]); printf(" IY:   0x%04x\n", cpu->idxregs[IY]);
}

void sf(uint8_t flag, CPU *cpu) {
    cpu->regs[F] |= 0x1 << flag;
}

void rf(uint8_t flag, CPU *cpu) {
    cpu->regs[F] &= ~(0x1 << flag);
}

bool gf(uint8_t flag, CPU *cpu) {
    return (cpu->regs[F] >> flag) & 1;
}

uint16_t getReg16(uint8_t num, CPU *cpu) {
    switch (num) {
        case 0b00:
            return cpu->regs[B] << 8 | cpu->regs[C];
        case 0b01:
            return cpu->regs[D] << 8 | cpu->regs[E];
        case 0b10:
            return cpu->regs[H] << 8 | cpu->regs[L];
        case 0b11:
            return cpu->sp;
        default:
            printf("ERROR: Invalid reg pair passed to getReg16(uint8_t num, CPU *cpu) (value was %d)\n", num);
            return -1;
    }
}

bool getCondition(uint8_t num, CPU *cpu) {
    switch (num) {
        case 0b000:
            return !gf(ZERO_FLAG, cpu);
        case 0b001:
            return gf(ZERO_FLAG, cpu);
        case 0b010:
            return !gf(CARRY_FLAG, cpu);
        case 0b011:
            return gf(CARRY_FLAG, cpu);
        case 0b100:
            return !gf(PARITY_OVF_FLAG, cpu);
        case 0b101:
            return gf(PARITY_OVF_FLAG, cpu);
        case 0b110:
            return !gf(SIGN_FLAG, cpu);
        case 0b111:
            return gf(SIGN_FLAG, cpu);
        default:
            printf("ERROR: Invalid argument to getCondition(uint8_t condition) (value was %d)\n", num);
            return false;
    }
}

void setReg16(uint8_t num, uint16_t value, CPU *cpu) {
    switch (num) {
        case 0b00:
            cpu->regs[B] = value >> 8;
            cpu->regs[C] = value & 0xFF;
            break;
        case 0b01:
            cpu->regs[D] = value >> 8;
            cpu->regs[E] = value & 0xFF;
            break;
        case 0b10:
            cpu->regs[H] = value >> 8;
            cpu->regs[L] = value & 0xFF;
            break;
        case 0b11:
            cpu->sp = value;
            break;
        default:
            printf("ERROR: Invalid reg pair passed to setReg16() (value was %d)\n", num);
            return;
    }
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/regs.h"
#include "../include/interface.h"

void execute(uint8_t instr, CPU *cpu) {
    if (instr == 0x00) {
        
    } else if (instr == 0xDD && cpu->mem[cpu->pc + 1] == 0xFD) { //pop IY CHECK
        uint8_t lowByte = cpu->mem[cpu->sp++];
        cpu->idxregs[IY] = (cpu->mem[cpu->sp++] << 8) | lowByte;
        
    } else if (instr == 0xDD && cpu->mem[cpu->pc + 1] == 0xE1) { //pop IX CHECK
        uint8_t lowByte = cpu->mem[cpu->sp++];
        cpu->idxregs[IX] = (cpu->mem[cpu->sp++] << 8) | lowByte;
 
    } else if (instr == 0xFD || instr == 0xDD) {
        uint8_t ninstr = cpu->mem[++cpu->pc];
        uint8_t idxreg = (instr == 0xFD) ? 1 : 0;
        if (ninstr == 0x36) {
            int8_t d = (int8_t)cpu->mem[++cpu->pc];
            cpu->mem[cpu->idxregs[idxreg] + d] = cpu->mem[++cpu->pc];
            
        } else if (ninstr == 0xE9) {
            cpu->pc = cpu->idxregs[idxreg];
        
        } else if (ninstr == 0xF9) { //LD SP, IX or IY
            cpu->sp = cpu->idxregs[idxreg];
        
        } else if (ninstr == 0xE5) { //push IX or IY
            cpu->mem[--cpu->sp] = cpu->idxregs[idxreg] >> 8;
            cpu->mem[--cpu->sp] = cpu->idxregs[idxreg] & 0xFF;
            
        } else if (ninstr == 0x2A) { //LD IX or IY, (nn)
            uint8_t lowNN = cpu->mem[++cpu->pc];
            uint16_t nn = cpu->mem[++cpu->pc] << 8 | lowNN;
            uint8_t lowByte = cpu->mem[nn];
            uint8_t highByte = cpu->mem[nn + 1];
            cpu->idxregs[idxreg] = (highByte << 8) | lowByte;
            
        } else if (ninstr == 0x22) { //ld (nn), IX or IY
            uint8_t nn = cpu->mem[cpu->pc + 2] << 8 | cpu->mem[cpu->pc + 1];
            cpu->pc += 2;
            cpu->mem[nn] = cpu->idxregs[idxreg] & 0xFF;
            cpu->mem[nn + 1] = cpu->idxregs[idxreg] >> 8;
            
            
        } else if (ninstr == 0x21) { //LD IX or IY, nn
            uint8_t lowByte = cpu->mem[++cpu->pc];
            cpu->idxregs[idxreg] = cpu->mem[++cpu->pc] << 8 | lowByte;
            
        } else if ((ninstr & 0b11000111) == 0b01000110) {
            int8_t d = (int8_t)cpu->mem[++cpu->pc];
            cpu->regs[(ninstr & 0b00111000) >> 3] = cpu->mem[cpu->idxregs[idxreg] + d];
            
        } else if ((ninstr & 0b11111000) == 0b01110000) {
            int8_t d = (int8_t)cpu->mem[++cpu->pc];
            cpu->mem[cpu->idxregs[idxreg] + d] = cpu->regs[ninstr & 0b00000111];
        }
        
    } else if (instr == 0x76) {
        cpu->halt = true;
        return;
        
    } else if (instr == 0xED) {
        uint8_t ninstr = cpu->mem[++cpu->pc];
        //std::cout << "NEW INSTR : 0x" << std::hex << (int)ninstr << "\n";
        if (ninstr == 0xA0) { //LDI
            cpu->mem[getReg16(0b01, cpu)] = cpu->mem[getReg16(0b10, cpu)];
            setReg16(0b01, getReg16(0b01, cpu) + 1, cpu);
            setReg16(0b10, getReg16(0b10, cpu) + 1, cpu);
            setReg16(0b00, getReg16(0b00, cpu) - 1, cpu);
            rf(HALF_CARRY, cpu);
            if (cpu->regs[B] == 0 && cpu->regs[C] == 0) rf(PARITY_OVF_FLAG, cpu); else sf(PARITY_OVF_FLAG, cpu);
            rf(ADD_SUB_FLAG, cpu);
            
        } else if (ninstr == 0xA8) { //LDD
            cpu->mem[getReg16(0b01, cpu)] = cpu->mem[getReg16(0b10, cpu)];
            setReg16(0b01, getReg16(0b01, cpu) - 1, cpu);
            setReg16(0b10, getReg16(0b10, cpu) - 1, cpu);
            setReg16(0b00, getReg16(0b00, cpu) - 1, cpu);
            rf(HALF_CARRY, cpu);
            if (cpu->regs[B] == 0 && cpu->regs[C] == 0) rf(PARITY_OVF_FLAG, cpu); else sf(PARITY_OVF_FLAG, cpu);
            rf(ADD_SUB_FLAG, cpu);
            
        } else if (ninstr == 0xB8) { //LDDR
            cpu->mem[getReg16(0b01, cpu)] = cpu->mem[getReg16(0b10, cpu)];
            setReg16(0b01, getReg16(0b01, cpu) - 1, cpu);
            setReg16(0b10, getReg16(0b10, cpu) - 1, cpu);
            setReg16(0b00, getReg16(0b00, cpu) - 1, cpu);
            rf(HALF_CARRY, cpu);
            rf(PARITY_OVF_FLAG, cpu);
            rf(ADD_SUB_FLAG, cpu);
            if ((cpu->regs[B] != 0) | (cpu->regs[C] != 0)) cpu->pc -= 2;
            
        } else if (ninstr == 0xB0) {
            cpu->mem[getReg16(0b01, cpu)] = cpu->mem[getReg16(0b10, cpu)];
            setReg16(0b01, getReg16(0b01, cpu) + 1, cpu);
            setReg16(0b10, getReg16(0b10, cpu) + 1, cpu);
            setReg16(0b00, getReg16(0b00, cpu) - 1, cpu);
            rf(HALF_CARRY, cpu);
            rf(PARITY_OVF_FLAG, cpu);
            rf(ADD_SUB_FLAG, cpu);
            if ((cpu->regs[B] != 0) | (cpu->regs[C] != 0)) cpu->pc -= 2;
            
        } else if (ninstr == 0x57) {
            cpu->regs[A] = cpu->reg_i;
            if ((int8_t) cpu->reg_i < 0) sf(SIGN_FLAG, cpu); else rf(SIGN_FLAG, cpu);
            if (cpu->reg_i == 0) sf(ZERO_FLAG, cpu); else rf(ZERO_FLAG, cpu);
            rf(HALF_CARRY, cpu);
            rf(ADD_SUB_FLAG, cpu);
        } else if (ninstr == 0x5F) {
            cpu->regs[A] = cpu->reg_r;
            if ((int8_t) cpu->reg_r < 0) sf(SIGN_FLAG, cpu); else rf(SIGN_FLAG, cpu);
            if (cpu->reg_r == 0) sf(ZERO_FLAG, cpu); else rf(ZERO_FLAG, cpu);
            rf(HALF_CARRY, cpu);
            rf(ADD_SUB_FLAG, cpu);
        } else if (ninstr == 0x47) {
            cpu->reg_i = cpu->regs[A];
        } else if (ninstr == 0x4F) {
            cpu->reg_r = cpu->regs[A];
        } else if ((ninstr & 0b11001111) == 0b01001011) { //LD dd, (nn)
            uint8_t lowNN = cpu->mem[++cpu->pc];
            uint16_t nn = cpu->mem[++cpu->pc] << 8 | lowNN;
            uint8_t lowByte = cpu->mem[nn];
            uint8_t highByte = cpu->mem[nn + 1];
            setReg16((ninstr & 0b00110000) >> 4, highByte << 8 | lowByte, cpu);
            
        } else if ((ninstr & 0b11001111) == 0b01000011) { //LD (nn), dd
            uint8_t lowNN = cpu->mem[++cpu->pc];
            uint16_t nn = cpu->mem[++cpu->pc] << 8 | lowNN;
            uint16_t reg = getReg16((ninstr & 0b00110000) >> 4, cpu);
            cpu->mem[nn] = reg & 0xFF;
            cpu->mem[nn + 1] = reg >> 8;
        }

    } else if (instr == 0x22) { //LD (**), hl
        uint8_t lowNN = cpu->mem[++cpu->pc];
        uint16_t nn = cpu->mem[++cpu->pc] << 8 | lowNN;
        uint16_t reg = getReg16(0b10, cpu);
        cpu->mem[nn] = reg & 0xFF;
        cpu->mem[nn + 1] = reg >> 8;
        
    } else if (instr == 0xC3) { //JP nn
        uint8_t lowByte = cpu->mem[++cpu->pc];
        cpu->pc = (cpu->mem[cpu->pc + 1] << 8) | lowByte;
        
    } else if (instr == 0xEB) { //EX DE, HL
        uint8_t hx = cpu->regs[H];
        uint8_t lx = cpu->regs[L];
        cpu->regs[H] = cpu->regs[D];
        cpu->regs[L] = cpu->regs[E];
        cpu->regs[D] = hx;
        cpu->regs[E] = lx;
        
    } else if (instr == 0xE3) { //EX (SP), HL
        uint8_t lx = cpu->regs[L];
        cpu->regs[L] = cpu->mem[cpu->sp];
        cpu->mem[cpu->sp] = lx;
        
        uint8_t hx = cpu->regs[H];
        cpu->regs[H] = cpu->mem[cpu->sp + 1];
        cpu->mem[cpu->sp + 1] = hx;
        
    } else if (instr == 0xD9) { //EXX
        uint8_t b = cpu->regs[B];
        uint8_t c = cpu->regs[C];
        uint8_t d = cpu->regs[D];
        uint8_t e = cpu->regs[E];
        uint8_t h = cpu->regs[H];
        uint8_t l = cpu->regs[L];
        cpu->regs[B] = cpu->regs_[B];
        cpu->regs[C] = cpu->regs_[C];
        cpu->regs[D] = cpu->regs_[D];
        cpu->regs[E] = cpu->regs_[E];
        cpu->regs[H] = cpu->regs_[H];
        cpu->regs[L] = cpu->regs_[L];
        cpu->regs_[B] = b;
        cpu->regs_[C] = c;
        cpu->regs_[D] = d;
        cpu->regs_[E] = e;
        cpu->regs_[H] = h;
        cpu->regs_[L] = l;
        
    } else if (instr == 0x08) { //EX AF, AF'
        uint8_t ax = cpu->regs[A];
        uint8_t fx = cpu->regs[F];
        cpu->regs[A] = cpu->regs_[A];
        cpu->regs[F] = cpu->regs_[F];
        cpu->regs_[A] = ax;
        cpu->regs_[F] = fx;
        
    } else if (instr == 0x0A) {
        cpu->regs[A] = cpu->mem[cpu->regs[B] << 8 | cpu->regs[C]];
        
    } else if (instr == 0x1A) {
        cpu->regs[A] = cpu->mem[cpu->regs[D] << 8 | cpu->regs[E]];
        
    } else if (instr == 0x3A) {
        uint8_t n = cpu->mem[++cpu->pc];
        cpu->regs[A] = cpu->mem[(cpu->mem[++cpu->pc] << 8) | n];
        
        
    } else if (instr == 0x02) {
        cpu->mem[cpu->regs[B] << 8 | cpu->regs[C]] = cpu->regs[A];
        
    } else if (instr == 0x12) {
        cpu->mem[cpu->regs[D] << 8 | cpu->regs[E]] = cpu->regs[A];
        
    } else if (instr == 0x32) {
        uint8_t n = cpu->mem[++cpu->pc];
        cpu->mem[(cpu->mem[++cpu->pc] << 8) | n] = cpu->regs[A];
        
    } else if (instr == 0x36) {
        cpu->mem[cpu->regs[H] << 8 | cpu->regs[L]] = cpu->mem[++cpu->pc];
        
    } else if (instr == 0x2A) { //LD HL, (nn)
        uint8_t addr = cpu->mem[cpu->pc + 1] | cpu->mem[cpu->pc + 2] << 8;
        cpu->pc += 2;
        cpu->regs[L] = cpu->mem[addr];
        cpu->regs[H] = cpu->mem[addr + 1];
        
    } else if (instr == 0xF9) { //LD SP, HL
        cpu->sp = (cpu->regs[H] << 8) | cpu->regs[L];
        
    } else if (instr == 0x22) { //LD (nn), HL
        uint8_t addr = cpu->mem[cpu->pc + 1] | cpu->mem[cpu->pc + 2] << 8;
        cpu->pc += 2;
        cpu->mem[addr] = cpu->regs[L];
        cpu->mem[addr + 1] = cpu->regs[H];
        
    } else if (instr == 0xC3) { //JP nn
        uint8_t lowByte = cpu->mem[++cpu->pc];
        uint8_t highByte = cpu->mem[++cpu->pc];
        cpu->pc = (highByte << 8) | lowByte;
        
    } else if (instr == 0x18) { //JR e //CHECK
        int8_t e = cpu->mem[++cpu->pc];
        cpu->pc += e;
        
    } else if (instr == 0x38) { //JR C, e
        if (gf(CARRY_FLAG, cpu)) {
            int8_t e = cpu->mem[++cpu->pc];
            cpu->pc += e;
        }
        
    } else if (instr == 0x30) { //JR NC, e
        if (!gf(CARRY_FLAG, cpu)) {
            int8_t e = cpu->mem[++cpu->pc];
            cpu->pc += e;
        }
        
    } else if (instr == 0x28) { //JR Z, e
        if (gf(ZERO_FLAG, cpu)) {
            int8_t e = cpu->mem[++cpu->pc];
            cpu->pc += e;
        }
        
    } else if (instr == 0x20) { //JR NZ, e
        if (!gf(ZERO_FLAG, cpu)) {
            int8_t e = cpu->mem[++cpu->pc];
            cpu->pc += e;
        }
        
    } else if (instr == 0xE9) { //JP HL
        cpu->pc = (cpu->regs[H] << 8) | cpu->regs[L];
        
    } else if (instr == 0x10) { //DJNZ
        cpu->regs[B]--;
        if (cpu->regs[B] != 0) {
            int8_t e = cpu->mem[++cpu->pc];
            cpu->pc += e;
        } else {
            ++cpu->pc;
        }
        
    } else if ((instr & 0b11111000) == 0b10000000) { //ADD A, r
        //TODO half carry flag
        //TODO Check parity/overflow flag logic
        bool signBit = cpu->regs[A] >> 7;
        if (cpu->regs[instr & 0b00000111] + cpu->regs[A] > 0xFF) sf(CARRY_FLAG, cpu); else rf(CARRY_FLAG, cpu);
        cpu->regs[A] = cpu->regs[instr & 0b00000111] + cpu->regs[A];
        if (cpu->regs[A] == 0) sf(ZERO_FLAG, cpu); else rf(ZERO_FLAG, cpu);
        if (((int8_t)cpu->regs[A]) < 0) sf(SIGN_FLAG, cpu); else rf(SIGN_FLAG, cpu);
        if (signBit != ((bool)cpu->regs[A] >> 7)) sf(PARITY_OVF_FLAG, cpu); else rf(PARITY_OVF_FLAG, cpu);
        
        rf(ADD_SUB_FLAG, cpu);
        
    } else if ((instr & 0b11000111) == 0b11000010) { //JP cc, nn
        if (getCondition((instr & 0b00111000) >> 3, cpu)) {
            uint8_t lowByte = cpu->mem[++cpu->pc];
            uint8_t highByte = cpu->mem[++cpu->pc];
            cpu->pc = (highByte << 8) | lowByte;
        }
        
    } else if ((instr & 0b11001111) == 0b11000101) { //push qq
        uint8_t highByte;
        uint8_t lowByte;
        switch ((instr & 0b00110000) >> 4) {
            case 0b00:
                highByte = cpu->regs[B];
                lowByte = cpu->regs[C];
                break;
            case 0b01:
                highByte = cpu->regs[D];
                lowByte = cpu->regs[E];
                break;
            case 0b10:
                highByte = cpu->regs[H];
                lowByte = cpu->regs[L];
                break;
            case 0b11:
                highByte = cpu->regs[A];
                lowByte = cpu->regs[F];
                break;
        }
                
        cpu->mem[--cpu->sp] = highByte;
        cpu->mem[--cpu->sp] = lowByte;
        
    } else if ((instr & 0b11001111) == 0b11000001) { //pop qq
        uint8_t lowByte = cpu->mem[cpu->sp++];
        uint8_t highByte = cpu->mem[cpu->sp++];
        switch ((instr & 0b00110000) >> 4) {
            case 0b00:
                cpu->regs[B] = highByte;
                cpu->regs[C] = lowByte;
                break;
            case 0b01:
                cpu->regs[D] = highByte;
                cpu->regs[E] = lowByte;
                break;
            case 0b10:
                cpu->regs[H] = highByte;
                cpu->regs[L] = lowByte;
                break;
            case 0b11:
                cpu->regs[A] = highByte;
                cpu->regs[F] = lowByte;
                break;
        }
        
    } else if ((instr & 0b11111000) == 0b01110000) {
        cpu->mem[cpu->regs[H] << 8 | cpu->regs[L]] = cpu->regs[instr & 0b00000111];
        
    } else if ((instr & 0b11001111) == 0b00000001) { //LD dd, nn
        uint8_t lowOrderByte = cpu->mem[++cpu->pc];
        setReg16((instr & 0b00110000) >> 4, cpu->mem[++cpu->pc] << 8 | lowOrderByte, cpu);
        
    } else if ((instr & 0b11000000) == 0b01000000) {
        cpu->regs[(instr & 0b00111000) >> 3] = cpu->regs[instr & 0b00000111];
        
    } else if ((instr & 0b11000111) == 0b00000110) {
        cpu->regs[(instr & 0b00111000) >> 3] = cpu->mem[++cpu->pc];
        
    } else if ((instr & 0b11000111) == 0b01000110) {
        cpu->regs[(instr & 0b00111000) >> 3] = cpu->mem[cpu->regs[H] << 8 | cpu->regs[L]];
        
    } else {
        printf("WARNING: Unrecognised instruction %02X\n", instr);
    }
    cpu->pc++;
}

int run(CPU *cpu) {
    if (cpu->debug) {
        while (!cpu->halt) {
            if (cpu->step) {
                stepPrompt(cpu);
            }
            
            if (strcmp(cpu->eachTimeCommand, "") == 0) {
                executeDebugCommand(cpu->eachTimeCommand, cpu);
            }
            
            cpu->lastInstr = cpu->mem[cpu->pc];
            
            execute(cpu->mem[cpu->pc], cpu);
        }
        
    } else {
        while (!cpu->halt) {
            execute(cpu->mem[cpu->pc], cpu);
        }
    }
    return 0;
}

#include "../include/stringlib.h"

int main(int argc, char ** argv) {
    CPU cpu = {.eachTimeCommand = NULL};
    char *fname = parseArgs(argc, argv, &cpu);
    if (fname == NULL) {
        return 1;
    }
    if (readFile(fname, &cpu) != 0) {
        return 1;
    }
    free(fname);
    return run(&cpu);
}
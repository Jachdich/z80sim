#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <limits>
#include <vector>
#include <algorithm>
#include "regs.h"
#include "interface.h"

using namespace std;

reg_t mem[65536];

reg_t lastInstr = -1;

bool halt = false;
bool debug = false;
bool step  = false;

string eachTimeCommand = "";

void execute(reg_t instr) {
    if (instr == 0x00) {
        
    } else if (instr == 0xDD && mem[pc + 1] == 0xFD) { //pop IY CHECK
        reg_t lowByte = mem[sp++];
        IDXREGS[IY] = (mem[sp++] << 8) | lowByte;
        
    } else if (instr == 0xDD && mem[pc + 1] == 0xE1) { //pop IX CHECK
        reg_t lowByte = mem[sp++];
        IDXREGS[IX] = (mem[sp++] << 8) | lowByte;
 
    } else if (instr == 0xFD || instr == 0xDD) {
        reg_t ninstr = mem[++pc];
        reg_t idxreg = (instr == 0xFD) ? 1 : 0;
        if (ninstr == 0x36) {
            reg_signed_t d = (reg_signed_t)mem[++pc];
            mem[IDXREGS[idxreg] + d] = mem[++pc];
            
        } else if (ninstr == 0xE9) {
            pc = IDXREGS[idxreg];
        
        } else if (ninstr == 0xF9) { //LD SP, IX or IY
            sp = IDXREGS[idxreg];
        
        } else if (ninstr == 0xE5) { //push IX or IY
            mem[--sp] = IDXREGS[idxreg] >> 8;
            mem[--sp] = IDXREGS[idxreg] & 0xFF;
            
        } else if (ninstr == 0x2A) { //LD IX or IY, (nn)
            reg_t lowNN = mem[++pc];
            reg16_t nn = mem[++pc] << 8 | lowNN;
            reg_t lowByte = mem[nn];
            reg_t highByte = mem[nn + 1];
            IDXREGS[idxreg] = (highByte << 8) | lowByte;
            
        } else if (ninstr == 0x22) { //ld (nn), IX or IY
            reg_t nn = mem[pc + 2] << 8 | mem[pc + 1];
            pc += 2;
            mem[nn] = IDXREGS[idxreg] & 0xFF;
            mem[nn + 1] = IDXREGS[idxreg] >> 8;
            
            
        } else if (ninstr == 0x21) { //LD IX or IY, nn
            reg_t lowByte = mem[++pc];
            IDXREGS[idxreg] = mem[++pc] << 8 | lowByte;
            
        } else if ((ninstr & 0b11000111) == 0b01000110) {
            reg_signed_t d = (reg_signed_t)mem[++pc];
            REGS[(ninstr & 0b00111000) >> 3] = mem[IDXREGS[idxreg] + d];
            
        } else if ((ninstr & 0b11111000) == 0b01110000) {
            reg_signed_t d = (reg_signed_t)mem[++pc];
            mem[IDXREGS[idxreg] + d] = REGS[ninstr & 0b00000111];
        }
        
    } else if (instr == 0x76) {
        halt = true;
        return;
        
    } else if (instr == 0xED) {
        reg_t ninstr = mem[++pc];
        //std::cout << "NEW INSTR : 0x" << std::hex << (int)ninstr << "\n";
        if (ninstr == 0xA0) { //LDI
            mem[getReg16(0b01)] = mem[getReg16(0b10)];
            setReg16(0b01, getReg16(0b01) + 1);
            setReg16(0b10, getReg16(0b10) + 1);
            setReg16(0b00, getReg16(0b00) - 1);
            rf(HALF_CARRY);
            if (REGS[B] == 0 && REGS[C] == 0) rf(PARITY_OVF_FLAG); else sf(PARITY_OVF_FLAG);
            rf(ADD_SUB_FLAG);
            
        } else if (ninstr == 0xA8) { //LDD
            mem[getReg16(0b01)] = mem[getReg16(0b10)];
            setReg16(0b01, getReg16(0b01) - 1);
            setReg16(0b10, getReg16(0b10) - 1);
            setReg16(0b00, getReg16(0b00) - 1);
            rf(HALF_CARRY);
            if (REGS[B] == 0 && REGS[C] == 0) rf(PARITY_OVF_FLAG); else sf(PARITY_OVF_FLAG);
            rf(ADD_SUB_FLAG);
            
        } else if (ninstr == 0xB8) { //LDDR
            mem[getReg16(0b01)] = mem[getReg16(0b10)];
            setReg16(0b01, getReg16(0b01) - 1);
            setReg16(0b10, getReg16(0b10) - 1);
            setReg16(0b00, getReg16(0b00) - 1);
            rf(HALF_CARRY);
            rf(PARITY_OVF_FLAG);
            rf(ADD_SUB_FLAG);
            if ((REGS[B] != 0) | (REGS[C] != 0)) pc -= 2;
            
        } else if (ninstr == 0xB0) {
            mem[getReg16(0b01)] = mem[getReg16(0b10)];
            setReg16(0b01, getReg16(0b01) + 1);
            setReg16(0b10, getReg16(0b10) + 1);
            setReg16(0b00, getReg16(0b00) - 1);
            rf(HALF_CARRY);
            rf(PARITY_OVF_FLAG);
            rf(ADD_SUB_FLAG);
            if ((REGS[B] != 0) | (REGS[C] != 0)) pc -= 2;
            
        } else if (ninstr == 0x57) {
            REGS[A] = reg_i;
            if ((reg_signed_t) reg_i < 0) sf(SIGN_FLAG); else rf(SIGN_FLAG);
            if (reg_i == 0) sf(ZERO_FLAG); else rf(ZERO_FLAG);
            rf(HALF_CARRY);
            rf(ADD_SUB_FLAG);
        } else if (ninstr == 0x5F) {
            REGS[A] = reg_r;
            if ((reg_signed_t) reg_r < 0) sf(SIGN_FLAG); else rf(SIGN_FLAG);
            if (reg_r == 0) sf(ZERO_FLAG); else rf(ZERO_FLAG);
            rf(HALF_CARRY);
            rf(ADD_SUB_FLAG);
        } else if (ninstr == 0x47) {
            reg_i = REGS[A];
        } else if (ninstr == 0x4F) {
            reg_r = REGS[A];
        } else if ((ninstr & 0b11001111) == 0b01001011) { //LD dd, (nn)
            reg_t lowNN = mem[++pc];
            reg16_t nn = mem[++pc] << 8 | lowNN;
            reg_t lowByte = mem[nn];
            reg_t highByte = mem[nn + 1];
            setReg16((ninstr & 0b00110000) >> 4, highByte << 8 | lowByte);
            
        } else if ((ninstr & 0b11001111) == 0b01000011) { //LD (nn), dd
            reg_t lowNN = mem[++pc];
            reg16_t nn = mem[++pc] << 8 | lowNN;
            reg16_t reg = getReg16((ninstr & 0b00110000) >> 4);
            mem[nn] = reg & 0xFF;
            mem[nn + 1] = reg >> 8;
        }

    } else if (instr == 0x22) { //LD (**), hl
        reg_t lowNN = mem[++pc];
        reg16_t nn = mem[++pc] << 8 | lowNN;
        reg16_t reg = getReg16(0b10);
        mem[nn] = reg & 0xFF;
        mem[nn + 1] = reg >> 8;
        
    } else if (instr == 0xC3) { //JP nn
        reg_t lowByte = mem[++pc];
        pc = (mem[pc + 1] << 8) | lowByte;
        
    } else if (instr == 0xEB) { //EX DE, HL
        reg_t hx = REGS[H];
        reg_t lx = REGS[L];
        REGS[H] = REGS[D];
        REGS[L] = REGS[E];
        REGS[D] = hx;
        REGS[E] = lx;
        
    } else if (instr == 0xE3) { //EX (SP), HL
        reg_t lx = REGS[L];
        REGS[L] = mem[sp];
        mem[sp] = lx;
        
        reg_t hx = REGS[H];
        REGS[H] = mem[sp + 1];
        mem[sp + 1] = hx;
        
    } else if (instr == 0xD9) { //EXX
        reg_t b = REGS[B];
        reg_t c = REGS[C];
        reg_t d = REGS[D];
        reg_t e = REGS[E];
        reg_t h = REGS[H];
        reg_t l = REGS[L];
        REGS[B] = REGS_[B];
        REGS[C] = REGS_[C];
        REGS[D] = REGS_[D];
        REGS[E] = REGS_[E];
        REGS[H] = REGS_[H];
        REGS[L] = REGS_[L];
        REGS_[B] = b;
        REGS_[C] = c;
        REGS_[D] = d;
        REGS_[E] = e;
        REGS_[H] = h;
        REGS_[L] = l;
        
    } else if (instr == 0x08) { //EX AF, AF'
        reg_t ax = REGS[A];
        reg_t fx = REGS[F];
        REGS[A] = REGS_[A];
        REGS[F] = REGS_[F];
        REGS_[A] = ax;
        REGS_[F] = fx;
        
    } else if (instr == 0x0A) {
        REGS[A] = mem[REGS[B] << 8 | REGS[C]];
        
    } else if (instr == 0x1A) {
        REGS[A] = mem[REGS[D] << 8 | REGS[E]];
        
    } else if (instr == 0x3A) {
        reg_t n = mem[++pc];
        REGS[A] = mem[(mem[++pc] << 8) | n];
        
        
    } else if (instr == 0x02) {
        mem[REGS[B] << 8 | REGS[C]] = REGS[A];
        
    } else if (instr == 0x12) {
        mem[REGS[D] << 8 | REGS[E]] = REGS[A];
        
    } else if (instr == 0x32) {
        reg_t n = mem[++pc];
        mem[(mem[++pc] << 8) | n] = REGS[A];
        
    } else if (instr == 0x36) {
        mem[REGS[H] << 8 | REGS[L]] = mem[++pc];
        
    } else if (instr == 0x2A) { //LD HL, (nn)
        reg_t addr = mem[pc + 1] | mem[pc + 2] << 8;
        pc += 2;
        REGS[L] = mem[addr];
        REGS[H] = mem[addr + 1];
        
    } else if (instr == 0xF9) { //LD SP, HL
        sp = (REGS[H] << 8) | REGS[L];
        
    } else if (instr == 0x22) { //LD (nn), HL
        reg_t addr = mem[pc + 1] | mem[pc + 2] << 8;
        pc += 2;
        mem[addr] = REGS[L];
        mem[addr + 1] = REGS[H];
        
    } else if (instr == 0xC3) { //JP nn
        reg_t lowByte = mem[++pc];
        reg_t highByte = mem[++pc];
        pc = (highByte << 8) | lowByte;
        
    } else if (instr == 0x18) { //JR e //CHECK
        reg_signed_t e = mem[++pc];
        pc += e;
        
    } else if (instr == 0x38) { //JR C, e
        if (gf(CARRY_FLAG)) {
            reg_signed_t e = mem[++pc];
            pc += e;
        }
        
    } else if (instr == 0x30) { //JR NC, e
        if (!gf(CARRY_FLAG)) {
            reg_signed_t e = mem[++pc];
            pc += e;
        }
        
    } else if (instr == 0x28) { //JR Z, e
        if (gf(ZERO_FLAG)) {
            reg_signed_t e = mem[++pc];
            pc += e;
        }
        
    } else if (instr == 0x20) { //JR NZ, e
        if (!gf(ZERO_FLAG)) {
            reg_signed_t e = mem[++pc];
            pc += e;
        }
        
    } else if (instr == 0xE9) { //JP HL
        pc = (REGS[H] << 8) | REGS[L];
        
    } else if (instr == 0x10) { //DJNZ
        REGS[B]--;
        if (REGS[B] != 0) {
            reg_signed_t e = mem[++pc];
            pc += e;
        } else {
            ++pc;
        }
        
    } else if ((instr & 0b11111000) == 0b10000000) { //ADD A, r
        //TODO half carry flag
        //TODO Check parity/overflow flag logic
        bool signBit = REGS[A] >> 7;
        if (REGS[instr & 0b00000111] + REGS[A] > 0xFF) sf(CARRY_FLAG); else rf(CARRY_FLAG);
        REGS[A] = REGS[instr & 0b00000111] + REGS[A];
        if (REGS[A] == 0) sf(ZERO_FLAG); else rf(ZERO_FLAG);
        if (((reg_signed_t)REGS[A]) < 0) sf(SIGN_FLAG); else rf(SIGN_FLAG);
        if (signBit != ((bool)REGS[A] >> 7)) sf(PARITY_OVF_FLAG); else rf(PARITY_OVF_FLAG);
        
        rf(ADD_SUB_FLAG);
        
    } else if ((instr & 0b11000111) == 0b11000010) { //JP cc, nn
        if (getCondition((instr & 0b00111000) >> 3)) {
            reg_t lowByte = mem[++pc];
            reg_t highByte = mem[++pc];
            pc = (highByte << 8) | lowByte;
        }
        
    } else if ((instr & 0b11001111) == 0b11000101) { //push qq
        reg_t highByte;
        reg_t lowByte;
        switch ((instr & 0b00110000) >> 4) {
            case 0b00:
                highByte = REGS[B];
                lowByte = REGS[C];
                break;
            case 0b01:
                highByte = REGS[D];
                lowByte = REGS[E];
                break;
            case 0b10:
                highByte = REGS[H];
                lowByte = REGS[L];
                break;
            case 0b11:
                highByte = REGS[A];
                lowByte = REGS[F];
                break;
        }
                
        mem[--sp] = highByte;
        mem[--sp] = lowByte;
        
    } else if ((instr & 0b11001111) == 0b11000001) { //pop qq
        reg_t lowByte = mem[sp++];
        reg_t highByte = mem[sp++];
        switch ((instr & 0b00110000) >> 4) {
            case 0b00:
                REGS[B] = highByte;
                REGS[C] = lowByte;
                break;
            case 0b01:
                REGS[D] = highByte;
                REGS[E] = lowByte;
                break;
            case 0b10:
                REGS[H] = highByte;
                REGS[L] = lowByte;
                break;
            case 0b11:
                REGS[A] = highByte;
                REGS[F] = lowByte;
                break;
        }
        
    } else if ((instr & 0b11111000) == 0b01110000) {
        mem[REGS[H] << 8 | REGS[L]] = REGS[instr & 0b00000111];
        
    } else if ((instr & 0b11001111) == 0b00000001) { //LD dd, nn
        reg_t lowOrderByte = mem[++pc];
        setReg16((instr & 0b00110000) >> 4, mem[++pc] << 8 | lowOrderByte);
        
    } else if ((instr & 0b11000000) == 0b01000000) {
        REGS[(instr & 0b00111000) >> 3] = REGS[instr & 0b00000111];
        
    } else if ((instr & 0b11000111) == 0b00000110) {
        REGS[(instr & 0b00111000) >> 3] = mem[++pc];
        
    } else if ((instr & 0b11000111) == 0b01000110) {
        REGS[(instr & 0b00111000) >> 3] = mem[REGS[H] << 8 | REGS[L]];
        
    } else {
        printf("WARNING: Unrecognised instruction %02X\n", instr);
    }
    pc++;
}

int run() {
    if (debug) {
        while (!halt) {
            if (step) {
                stepPrompt();
            }
            
            if (eachTimeCommand != "") {
                executeDebugCommand(eachTimeCommand);
            }
            
            lastInstr = mem[pc];
            
            execute(mem[pc]);
        }
        
    } else {
        while (!halt) {
            execute(mem[pc]);
        }
    }
    return 0;
}

int main(int argc, char ** argv) {
    std::string fname = parseArgs(argc, argv);
    if (fname == "") {
        return 1;
    }
    if (readFile(fname) != 0) {
        return 1;
    }
    return run();
}
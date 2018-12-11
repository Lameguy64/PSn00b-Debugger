/* 
 * File:   disassembler.h
 * Author: Lameguy64
 *
 * Created on August 7, 2018, 8:45 AM
 */

#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

unsigned int mips_GetNextPc(unsigned int *regs, int stepover);
void mips_Decode(unsigned int opcode, unsigned int addr, char* output, int arrows = 0);
unsigned int mips_GetJumpAddr(unsigned int addr, unsigned int opcode);

#endif /* DISASSEMBLER_H */


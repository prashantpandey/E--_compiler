/*
 * =====================================================================================
 *
 *       Filename:  CodeGen.C
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/25/2014 04:05:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ,
 *   Organization:
 *
 * =====================================================================================
 */

#include "CodeGen.h"

//  Add  book keeping for the special puspose registers
//  Stack pointer: R999
//  Base Point: R998
//  Return Value: R997/F997
//  Global Vars Base Addr: R996/F996
//
//  General Purpose Registers:	returned by the global memory allocation function
//	two values: one  each for the Integer/Float registers
//
//  Register free list for tracking the registers for the temporary vars
//	: allocate from a global pool of registers
//	: add the register to used free list when not in use
//
//
//	functions required:
//	    : string fetchNextAvailReg()
//	    : void purgeReg(string regName)
//
//
//  Code Gen:
//

const char* instrName[] = {
    "ADD", "SUB", "DIV", "MUL", "MOD", "FADD", "FSUB", "FDIV", "FMUL",
    "AND", "OR", "XOR", "NEG", "FNEG",
    "UGT", "UGE", "GT", "GE", "EQ", "NE", "FGT", "FGE", "FEQ", "FNE",
    "PRTS", "PRTI", "PRTF",
    "JMP", "JMPC", "JMPI", "JMPCI",
    "MOVL", "MOVS", "MOVI", "MOVF", "MOVIF", "MOVFI",
    "STI", "STF", "LDI", "LDF",
    "IN", "INI", "INF"
};

string Instruction::name(InstructionSet inst) {
    return string(instrName[inst]);
}

Instruction* Instruction::decrSP()  {
    Instruction *instr = new Instruction(Instruction::InstructionSet::SUB, SP_REG, "1", SP_REG);
    return instr;
}

string Instruction::toString() {

    ostringstream os;
    if(label_ != "")
	os << label_ << ": ";
    os << name(instr_) << " " << param1_ << " " << param2_ << " " << param3_ << " " << comment_;
    return os.str();

}


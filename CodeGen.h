/*
 * =====================================================================================
 *
 *       Filename:  CodeGen.h
 *
 *    Description:  File containing classes required for final code generation.
 *
 *        Version:  1.0
 *        Created:  04/25/2014 03:19:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  , 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CODEGEN_H
#define CODEGEN_H

class Instruction;

class FinalCode {
   
   public:
	FinalCode(string progName) {
	    progName_ = progName;
	    instructions_ = new vector<Instruction*>;
	}
    
	~FinalCode() {};

	void insertInstruction(Instruction *instr) {
	    instructions_->push_back(instr);
	}

    private:
	string progName_;
	vector<Instruction*> *instructions_;
};

class Instruction {

    public:
	enum InstructionSet {
	    ADD, SUB, DIV, MUL, MOD, FADD, FSUB, FDIV, FMUL, 
	    AND, OR, XOR, NEG, FNEG, 
	   UGT, UGE, GT, GE, EQ, NE, FGT, FGE, FEQ, FNE,
	   PRTS, PRTI, PRTF,
	    JMP, JMPC, JMPI, JMPCI,
	    MOVL, MOVS, MOVI, MOVF, MOVIF, MOVFI,
	    STI, STF, LDI, LDF,
	    IN, INI, INF
	};

	enum ParamType {
	    REGISTER,
	    MEMORY_LOC,
	    VALUE
	};

	Instruction(InstructionSet instr, string param1, string param2 = "", string param3 = "", string label = "") {
	    instr_ = instr;
	    param1_ = param1;
	    param2_ = param2;
	    param3_ = param3;
	    label_ = label;
	};

	~ Instruction() {};
    
	InstructionSet getInstr() { return instr_; };
	string getParam1() { return param1_; };
	string getParam2() { return param2_; };
	string getParam3() { return param3_; };
	string getLabel() { return label_; };
     
    private:
	InstructionSet instr_;
	string param1_;
	string param2_;
	string param3_;
	string label_;
};

#endif

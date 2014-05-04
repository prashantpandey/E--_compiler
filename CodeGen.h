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

#include <sstream>
#include <string>
#include <vector>
#include "RegMgr.h"

using namespace std;
class Instruction;
class CodeModule;

class ProgCode {
public:
    ProgCode(string progName) {
	progName_ = progName;
	modules_ = new vector<CodeModule*>;
    }
    
    ~ProgCode() {};

    void insertModule(CodeModule* codeMod) {
	modules_->push_back(codeMod);	
    }

private:
    string progName_;
    vector<CodeModule*> *modules_;
};

class CodeModule {
public:
    CodeModule(string moduleName) {
        moduleName_ = moduleName;
        instructions_ = new vector<Instruction*>;
    }

    ~CodeModule() {
	delete instructions_;
    };

    void insertInstructionSet(Instruction *instr) {
	if (instr != NULL)
	    instructions_->push_back(instr);
    }

    void insertInstructionSet(vector<Instruction *> *instrVector) {
	if (instrVector != NULL)
	    instructions_->insert(instructions_->end(), instrVector->begin(), instrVector->end());
    }

private:
    string moduleName_;
    vector<Instruction*> *instructions_;
};


class Quadruple {
    public: 

	Quadruple(string opr, string opr1, string opr2 = "", string res = "") {
	    opr_ = opr; 
	    opr1_ = opr1;
	    opr2_ = opr2;
	    res_ = res;
	};

	~ Quadruple() {};

	void setOpr(string opr){
	    opr_ = opr;
	};

	void setOpr1(string opr1){
	    opr1_ = opr1;
	};

	void setOpr2(string opr2){
	    opr2_ = opr2;
	};

	void setRes(string res){
	    res_ = res;
	};

	string getOpr() {
	    return opr_;
	};

	string getOpr1() {
	    return opr1_;
	};

	string getOpr2() {
	    return opr2_;
	};

	string getRes() {
	    return res_;
	};

	bool isEqual(Quadruple *quad){

	    if(quad->opr_.compare(opr_) == 0) {
		if(quad->opr1_.compare(opr1_) == 0) {
		    if(quad->opr2_.compare(opr2_) ==0) {
			return true;
		    }
		}
	    }
	    return false;
	}
    private:
	string opr_;
	string opr1_;
	string opr2_;
	string res_;
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

	InstructionSet getInstr() {
	    return instr_;
	};

	string getParam1() {
	    return param1_;
	};
	string getParam2() {
	    return param2_;
	};
	string getParam3() {
	    return param3_;
	};
	string getLabel() {
	    return label_;
	};

	string name(InstructionSet inst);
	string toString();
	static Instruction* decrSP();

    private:
	InstructionSet instr_;
	string param1_;
	string param2_;
	string param3_;
	string label_;
};

#endif

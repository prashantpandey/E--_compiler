/*
 *
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
#include "IncludeHeaders.h"

using namespace std;

class Instruction;
class OpCodeInstMap;
class CodeModule;
class VariableEntry;
class OpNode;

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
	    IN, INI, INF, BLANK,
	    DEFAULT
	};

	enum ParamType {
	    REGISTER,
	    MEMORY_LOC,
	    VALUE
	};

	Instruction(InstructionSet instr, string param1, string param2 = "", string param3 = "", string label = "", string comment = "") {
	    instr_ = instr;
	    param1_ = param1;
	    param2_ = param2;
	    param3_ = param3;
	    label_ = label;
	    comment_ = comment;
	};

	Instruction(string label) {
	    instr_ = InstructionSet::BLANK;
	    label_ = label;

	}

	~ Instruction() {};

	InstructionSet getInstr() const{
	    return instr_;
	};

	string getParam1() const{
	    return param1_;
	};
	string getParam2() const{
	    return param2_;
	};
	string getParam3() const{
	    return param3_;
	};
	string getLabel() const{
	    return label_;
	};

	void setLabel(string label) {
	    label_ = label;
	};
	
	void setComment(string comment) {
	    comment_ = comment;
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
	string comment_;
};

class OpCodeInstMap {
public:
    static OpCodeInstMap* opCodeInstMap_[30];
    
    OpCodeInstMap(OpNode::OpCode oprCode, initializer_list<Instruction::InstructionSet> list) {
	int i = 0;
	oprCode_ = oprCode;
	for(Instruction::InstructionSet inst : list) {
	    instr_[i++] = inst;
	}
    }

    static OpCodeInstMap* getOpCode(OpNode::OpCode oprCode) {
	// TODO: return  OpCodeInstMap
	return opCodeInstMap_[(int)oprCode];
    };

    static Instruction::InstructionSet fetchInstr(OpNode::OpCode oprCode, int instNum) {
	return	getOpCode(oprCode)->instr_[instNum];
    };

private:
    OpNode::OpCode oprCode_;
    Instruction::InstructionSet instr_[10];
};

class ProgCode {
public:
    ProgCode(string progName) {
	progName_ = progName;
	modules_ = new vector<CodeModule*>;
    }
    
    ~ProgCode() {};

    vector <CodeModule*>* getModule(){
	return modules_;
    }

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

    vector <Instruction*>* getInstructions(){
	return instructions_;
    }

    void insertInstructionSet(Instruction *instr) {
	if (instr != NULL)
	    instructions_->push_back(instr);
    }

    void insertInstructionSet(vector<Instruction *> *instrVector) {
	mergeVec(instructions_, instrVector);
    }

    Instruction* firstInst() const {
	if (instructions_->size())
	    return instructions_->front();
	return NULL;
    }

private:
    string moduleName_;
    vector<Instruction*> *instructions_ = NULL;
};


class Quadruple {
    public: 
	Quadruple(OpNode::OpCode opc, VariableEntry *opr1, VariableEntry *opr2 = NULL, VariableEntry *res = NULL) {
	    opc_ = opc; 
	    opr1_ = opr1;
	    opr2_ = opr2;
	    res_ = res;
	};

	~ Quadruple() {};

	void setOpc(OpNode::OpCode opc){
	    opc_ = opc;
	};

	void setOpr1(VariableEntry *opr1){
	    opr1_ = opr1;
	};

	void setOpr2(VariableEntry *opr2){
	    opr2_ = opr2;
	};

	void setRes(VariableEntry *res){
	    res_ = res;
	};

	OpNode::OpCode getOpc() {
	    return opc_;
	};

	VariableEntry* getOpr1() {
	    return opr1_;
	};

	VariableEntry* getOpr2() {
	    return opr2_;
	};

	VariableEntry* getRes() {
	    return res_;
	};

    static vector<Instruction*>* iCodeToAsmGen(vector<Quadruple*> *quad);
    static bool checkRegOrTemp(VariableEntry *ve, string &regName);
    static int tempCnt_;
    static string fetchTempVar();
    static void resetTempCnt();
    bool isEqual(Quadruple *quad);
    
    private:
	OpNode::OpCode opc_;
	VariableEntry *opr1_;
	VariableEntry *opr2_;
	VariableEntry *res_;
};

#endif

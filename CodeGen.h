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
#include <set>
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

    InstructionSet getInstr() const {
        return instr_;
    };

    string getParam1() const {
        return param1_;
    };
    string getParam2() const {
        return param2_;
    };
    string getParam3() const {
        return param3_;
    };
    string getLabel() const {
        return label_;
    };

    void setLabel(string label) {
        label_ = label;
    };

    void setComment(string comment) {
        comment_ = comment;
    };

    string name(InstructionSet inst);
    string toString(bool newLine = true);
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
    static OpCodeInstMap* opCodeInstMap_[31];

    OpCodeInstMap(OpNode::OpCode oprCode, initializer_list<Instruction::InstructionSet> list) {
        int i = 0;
        oprCode_ = oprCode;
        for(Instruction::InstructionSet inst : list) {
            instr_[i++] = inst;
        }
        instrSize_ = i;
    }

    static OpCodeInstMap* getOpCode(OpNode::OpCode oprCode) {
        // TODO: return  OpCodeInstMap
        return opCodeInstMap_[(int)oprCode];
    };

    static Instruction::InstructionSet fetchInstr(OpNode::OpCode oprCode, int instNum) {
        OpCodeInstMap *p = getOpCode(oprCode);
        if (instNum < p->instrSize_)
            return  p->instr_[instNum];
        else
            return p->instr_[p->instrSize_ - 1];
    };

private:
    OpNode::OpCode oprCode_;
    Instruction::InstructionSet instr_[10];
    int instrSize_;
};

class ProgCode {
public:
    ProgCode(string progName) {
        progName_ = progName;
        modules_ = new vector<CodeModule*>;
    }

    ~ProgCode() {};

    vector <CodeModule*>* getModule() {
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

    vector <Instruction*>* getInstructions() {
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

/*
class IntrCodeElem {
    public:
	enum class ElemType {
	    VAR_TYPE, TEMP_VAR_TYPE, VAL_TYPE,
	    INV_NODE_TYPE, REF_EXPR_TYPE, QUAD_TYPE,
	    LABEL_TYPE, REG_TYPE, PARAM_TYPE
	};

	IntrCodeElem(const ProgramElem *elem, ElemType type) {
	    progElem_ = elem;
	    type_ = type;
	};

	const ProgramElem* getElem() const { return progElem_; };
	ElemType getType() { return type_; };

    private:
	ElemType type_;
	const ProgramElem* progElem_;
};

*/

class IntrCodeParams : public ProgramElem {
public:
    IntrCodeParams(vector<IntrCodeElem*>* params) {
        params_ = params;
    };

    vector<IntrCodeElem*>* getParams() {
        return params_;
    };

private:
    vector<IntrCodeElem*>* params_;
};

class IntrLabel : public ProgramElem {
public:
    IntrLabel(string label) {
        label_ = label;
    };

    string getLabel() {
        return label_;
    };

private:
    string label_;
};

class Quadruple : public ProgramElem {
public:
    Quadruple(OpNode::OpCode opc, IntrCodeElem *opr1, IntrCodeElem *opr2 = NULL, IntrCodeElem *res = NULL, string label = "") {
        opc_ = opc;
        opr1_ = opr1;
        opr2_ = opr2;
        res_ = res;
        label_ = label;
    };

    ~ Quadruple() {};

    void setOpc(OpNode::OpCode opc) {
        opc_ = opc;
    };

    void setOpr1(IntrCodeElem *opr1) {
        opr1_ = opr1;
    };

    void setOpr2(IntrCodeElem *opr2) {
        opr2_ = opr2;
    };

    void setRes(IntrCodeElem *res) {
        res_ = res;
    };

    void setLabel(string label) {
        label_ = label;
    }

    OpNode::OpCode getOpc() {
        return opc_;
    };

    IntrCodeElem* getOpr1() {
        return opr1_;
    };

    IntrCodeElem* getOpr2() {
        return opr2_;
    };

    IntrCodeElem* getRes() {
        return res_;
    };

    string getLabel() {
        return label_;
    }

    string toString(bool newline = true);

    static vector<Instruction*>* iCodeToAsmGen(vector<Quadruple*> *quad, bool showComment = true, bool purgeRegisters = true);
    static int tempCnt_;
    static string fetchTempVar();
    static void resetTempCnt();
    bool isEqual(Quadruple *quad);

private:
    OpNode::OpCode opc_;
    IntrCodeElem *opr1_;
    IntrCodeElem *opr2_;
    IntrCodeElem *res_;
    string label_;
};

#endif

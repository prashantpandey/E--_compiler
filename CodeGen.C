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

#include "IncludeHeaders.h"

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

int Quadruple::tempCnt_ = 0;

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
    if(instr_ != InstructionSet::BLANK) {
        os << name(instr_) << " " << param1_ << " " << param2_ << " " << param3_ ;
        if(comment_ != "")
            os << " // " << comment_;
        os << endl;
    }
    return os.str();

}

const char* opCodeName[] = {
    "UMINUS", "PLUS", "MINUS", "MULT", "DIV", "MOD",
    "EQ", "NE", "GT", "LT", "GE", "LE",
    "AND", "OR", "NOT",
    "BITNOT", "BITAND", "BITOR", "BITXOR", "SHL", "SHR",
    "ASSIGN", "PRINT", "INVALID",
    "JMP", "JMPC", "CALL", "RET",
    "DEFAULT"
};

string Quadruple::toString() {

    ostringstream os;
    string param1 = "", param2 = "", param3 = "";
    string opc = opCodeName[(int)opc_];

    if(opr1_ != NULL) {

        param1 = IntrCodeElem::toString(opr1_);

        if(opr2_ != NULL) {
            param2 = IntrCodeElem::toString(opr2_);
        }

        if(res_ != NULL) {
            param3 = IntrCodeElem::toString(res_);
        }
    }

    //TODO:: Change how labels can be fetched

    if(label_ != "")
        os << label_ << ": ";
    os << opc << " " << param1 << " " << param2 << " " << param3 ;
    os << endl;
    return os.str();
}


string IntrCodeElem::toString(IntrCodeElem *ice) {

    ProgramElem *pe = ice->getElem();
    ElemType et = ice->getType();
    ostringstream os;

    switch(et) {

    case ElemType::VAR_TYPE:
        return ((VariableEntry*)pe)->name();

    case ElemType::TEMP_VAR_TYPE:
        return ((VariableEntry*)pe)->name();

    case ElemType::VAL_TYPE:
        return (((ValueNode*)pe)->value())->toString();

    case ElemType::INV_NODE_TYPE:
        return (((InvocationNode*)pe)->symTabEntry())->name();

    case ElemType::REF_EXPR_TYPE:
        return ((VariableEntry*)pe)->name();

    case ElemType::QUAD_TYPE:
        return ((Quadruple*)pe)->toString();

    case ElemType::LABEL_TYPE:
        return ((IntrLabel*)pe)->getLabel();

    case ElemType::PARAM_TYPE:
        vector<IntrCodeElem*> *params = ((IntrCodeParams*)pe)->getParams();
        for(vector<IntrCodeElem*>::iterator it = params->begin(); it != params->end(); ++it) {
            os << IntrCodeElem::toString((*it));
        }
        return os.str();
    }

    return string("");
}




string Quadruple::fetchTempVar() {
    ostringstream os;
    os << "T" << tempCnt_++;
    return os.str();
}

void Quadruple::resetTempCnt() {
    tempCnt_ = 0;
}

bool Quadruple::isEqual(Quadruple *quad) {
    /*
        if(quad->opc_ == opc_) {
            if(quad->opr1_->name().compare(opr1_->name()) == 0) {
                if(quad->opr2_->name().compare(opr2_->name()) ==0) {
                    return true;
                }
            }
        }
    */
    return false;
}

OpCodeInstMap* OpCodeInstMap::opCodeInstMap_[] = {
    new OpCodeInstMap(OpNode::OpCode::UMINUS, {Instruction::InstructionSet::NEG, Instruction::InstructionSet::FNEG}),
    new OpCodeInstMap(OpNode::OpCode::PLUS, {Instruction::InstructionSet::ADD, Instruction::InstructionSet::FADD}),
    new OpCodeInstMap(OpNode::OpCode::MINUS, {Instruction::InstructionSet::SUB, Instruction::InstructionSet::FSUB}),
    new OpCodeInstMap(OpNode::OpCode::MULT, {Instruction::InstructionSet::MUL, Instruction::InstructionSet::FMUL}),
    new OpCodeInstMap(OpNode::OpCode::DIV, {Instruction::InstructionSet::DIV, Instruction::InstructionSet::FDIV}),
    new OpCodeInstMap(OpNode::OpCode::MOD, {Instruction::InstructionSet::MOD}),
    new OpCodeInstMap(OpNode::OpCode::EQ, {Instruction::InstructionSet::EQ, Instruction::InstructionSet::FEQ}),
    new OpCodeInstMap(OpNode::OpCode::NE, {Instruction::InstructionSet::NE, Instruction::InstructionSet::FNE}),
    new OpCodeInstMap(OpNode::OpCode::GT, {Instruction::InstructionSet::GT, Instruction::InstructionSet::FGT}),
    new OpCodeInstMap(OpNode::OpCode::LT, {Instruction::InstructionSet::GT, Instruction::InstructionSet::FGT}),
    new OpCodeInstMap(OpNode::OpCode::GE, {Instruction::InstructionSet::GE, Instruction::InstructionSet::FGE}),
    new OpCodeInstMap(OpNode::OpCode::LE, {Instruction::InstructionSet::GE, Instruction::InstructionSet::FGE}),
    new OpCodeInstMap(OpNode::OpCode::AND, {Instruction::InstructionSet::AND}),
    new OpCodeInstMap(OpNode::OpCode::OR, {Instruction::InstructionSet::OR}),
    new OpCodeInstMap(OpNode::OpCode::NOT, {Instruction::InstructionSet::XOR}),
    new OpCodeInstMap(OpNode::OpCode::BITNOT, {Instruction::InstructionSet::XOR}),
    new OpCodeInstMap(OpNode::OpCode::BITAND, {Instruction::InstructionSet::AND}),
    new OpCodeInstMap(OpNode::OpCode::BITOR, {Instruction::InstructionSet::OR}),
    new OpCodeInstMap(OpNode::OpCode::BITXOR, {Instruction::InstructionSet::XOR}),
    new OpCodeInstMap(OpNode::OpCode::SHL, {}),
    new OpCodeInstMap(OpNode::OpCode::SHR, {}),
    new OpCodeInstMap(OpNode::OpCode::ASSIGN, {Instruction::InstructionSet::MOVI, Instruction::InstructionSet::MOVF,  Instruction::InstructionSet::MOVS}),
    new OpCodeInstMap(OpNode::OpCode::PRINT, {Instruction::InstructionSet::PRTI, Instruction::InstructionSet::PRTF}),
    new OpCodeInstMap(OpNode::OpCode::INVALID, {})
};

static string instructionParam(IntrCodeElem *e, vector<Instruction*> *inst_vec) {
    switch(e->getType()) {
    case IntrCodeElem::ElemType::VAR_TYPE:
    case IntrCodeElem::ElemType::TEMP_VAR_TYPE:
    case IntrCodeElem::ElemType::REF_EXPR_TYPE:
    {
        VariableEntry *ve = (VariableEntry*)(e->getElem());
        return regMgr->getVEReg(ve, inst_vec);
    }
    case IntrCodeElem::ElemType::VAL_TYPE:
    {
        ValueNode *vn = (ValueNode*)(e->getElem());
        return vn->value()->toString();
    }
    break;
    case IntrCodeElem::ElemType::INV_NODE_TYPE:
    case IntrCodeElem::ElemType::PARAM_TYPE:
        throw -1;
        //Control Should not reach here, so some error
    case IntrCodeElem::ElemType::QUAD_TYPE:
    case IntrCodeElem::ElemType::LABEL_TYPE:
        //TODO
        break;
    }
    return "";
}
static vector<Instruction*>* getInstructionSet(OpNode::OpCode opc, IntrCodeElem *e1, IntrCodeElem *e2, IntrCodeElem *e3) {
    int instNum = 0;
    vector<Instruction*> *inst_vec = new vector<Instruction*>();
    Type *inst_type = e3->getElem()->type();
    instNum = Type::isFloat(inst_type->tag()) ? 1:0;
    switch(opc) {
    case OpNode::OpCode::LT:
    case OpNode::OpCode::LE:
        opc = OpNode::OpCode::GE;
        swap(e1,e2);
    case OpNode::OpCode::GT:
    case OpNode::OpCode::GE:
        inst_type = e1->getElem()->type();
        instNum = Type::isFloat(inst_type->tag()) ? 1:0;
        break;
    case OpNode::OpCode::ASSIGN:
        if( Type::isString(inst_type->tag()))
            instNum = 2;
        break;
    case OpNode::OpCode::SHL:
        break;
    case OpNode::OpCode::SHR:
        break;
    default:
        break;
    }
    Instruction::InstructionSet instCode = OpCodeInstMap::fetchInstr(opc, instNum);
    string param1 = instructionParam(e1, inst_vec);
    string param2 = "", param3 = "";
    if (e2) {
        param2 = instructionParam(e2, inst_vec);
        param3 = instructionParam(e3, inst_vec);
    }
    else
        param2 = instructionParam(e3, inst_vec);

    Instruction *inst = new Instruction(instCode, param1, param2, param3);
    inst_vec->push_back(inst);
    return inst_vec;
}

vector<Instruction*>* Quadruple::iCodeToAsmGen(vector<Quadruple*> *quad) {
    // TODO:: IMplement Expression Optimization

    IntrCodeElem *ve1, *ve2, *ve3;
    string regName1 = "", regName2 = "", regName3 = "";
    vector<Instruction*>* inst_set = new vector<Instruction*>();
    vector<Instruction*> *instructionSet;
    OpNode::OpCode opc;
    for(vector<Quadruple*>::iterator it = quad->begin(); it != quad->end(); ++it) {
        opc = (*it)->getOpc();
        ve1 = (*it)->getOpr1();
        ve2 = (*it)->getOpr2();
        ve3 = (*it)->getRes();
        if(checkRegOrTemp(ve1, regName1))
            delete(ve1);
        if(checkRegOrTemp(ve2, regName2))
            delete(ve2);
        if(checkRegOrTemp(ve3, regName3))
            delete(ve3);

        //TODO:: Map the opcode to instruction set
        instructionSet = getInstructionSet(opc, ve1, ve2, ve3);
        mergeVec(inst_set, instructionSet);
        delete(instructionSet);
    }
    return inst_set;
}

bool Quadruple::checkRegOrTemp(IntrCodeElem *ve, string &regName) {
    if(ve != NULL) {
        if(((VariableEntry*)ve->getElem())->isTemp()) { // temperary true
            regName = regMgr->fetchNextAvailReg(!Type::isInt(ve->getElem()->type()->tag()), ((VariableEntry*)(ve->getElem())), 0);
            ((VariableEntry*)ve->getElem())->setReg(regName);
        }
        else {
            regName = ((VariableEntry*)ve->getElem())->getReg();
        }
        // delete(ve);
    }
    return ((VariableEntry*)ve->getElem())->isTemp();
}

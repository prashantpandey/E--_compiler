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

string Instruction::toString(bool newLine) {

    ostringstream os;
    if(label_ != "")
        os << label_ << ": ";
    if(instr_ != InstructionSet::BLANK) {

        os << name(instr_);
        if (param1_ != "")
            os << " " << param1_;
        if (param2_ != "")
            os << " " << param2_;
        if (param3_ != "")
            os << " " << param3_;
        if(comment_ != "")
            os << " // " << comment_;
        if (newLine)
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
    "JMP", "JMPC", "CALL", "RET", "MOVIF", "IN",
    "DEFAULT"
};

string Quadruple::toString(bool newLine) {

    ostringstream os;
    string param1 = "", param2 = "", param3 = "";
    string opc = opCodeName[(int)opc_];

    param1 = IntrCodeElem::toString(opr1_);
    param2 = IntrCodeElem::toString(opr2_);
    param3 = IntrCodeElem::toString(res_);

    if(label_ != "")
        os << label_ << ": ";
    os << opc << " " << param1 << " " << param2 << " " << param3 ;
    if (newLine)
        os << endl;
    return os.str();
}

bool IntrCodeElem::equals(IntrCodeElem *p) {
    if (p->type_ != this->type_)
        return false;
    if (this->getElem() == p->getElem())
        return true;
    switch (this->type_)
    {
    case ElemType::VAL_TYPE:
        return (((ValueNode*)progElem_)->value()->toString() == ((ValueNode*)p->progElem_)->value()->toString());
    case ElemType::LABEL_TYPE:
    case ElemType::QUAD_TYPE:
    case ElemType::PARAM_TYPE:
        //TODO
    default:
        return false;
    }
}

string IntrCodeElem::toString(IntrCodeElem *ice) {

    if (ice == NULL)
        return "";
    ProgramElem *pe = ice->getElem();
    ElemType et = ice->getType();
    ostringstream os;

    switch(et) {

    case ElemType::VAR_TYPE:
    case ElemType::TEMP_VAR_TYPE:
        return ((VariableEntry*)pe)->name();

    case ElemType::VAL_TYPE:
        return (((ValueNode*)pe)->value())->toString();

    case ElemType::INV_NODE_TYPE:
        return (((InvocationNode*)pe)->symTabEntry())->name();

    case ElemType::REF_EXPR_TYPE:
        return ((VariableEntry*)pe)->name();

    case ElemType::QUAD_TYPE:
        return ((Quadruple*)pe)->toString(false);

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

static bool isAssociative(OpNode::OpCode opc) {
    switch (opc) {
    case OpNode::OpCode::PLUS:
    case OpNode::OpCode::MULT:
        return true;
    default:
        return false;
    }
}

static bool isAirthmeticOpr(OpNode::OpCode opc) {
    switch (opc) {
    case OpNode::OpCode::PLUS:
    case OpNode::OpCode::MINUS:
    case OpNode::OpCode::MULT:
    case OpNode::OpCode::DIV:
        return true;
    default:
        return false;
    }
}

static inline bool isAssigment(OpNode::OpCode opc) {
    return (opc == OpNode::OpCode::ASSIGN);
}
static void optimiseQuadruples(vector<Quadruple*> *quads) {
    vector<Quadruple*>::iterator it1, it2;
    int quadSize = quads->size();
    for(int i = 0; i < quadSize - 1; i++) {
        Quadruple *quad = quads->at(i);
        //cout << "~~~~~~~~~~~~~~~~" << quad->toString();
        bool isDef = false, isAirthmetic = false;
        set<ProgramElem*> *replaceSet = new set<ProgramElem*>();
        if (!(isAirthmetic = isAirthmeticOpr(quad->getOpc())) && !(isDef = isAssigment(quad->getOpc())))
            continue;
        bool associative = isAssociative(quad->getOpc());
        IntrCodeElem *mOpr1 = quad->getOpr1();
        IntrCodeElem *mOpr2 = quad->getOpr2();
        IntrCodeElem *mRes = quad->getRes();
        for(int j = i + 1; j < quadSize; j++) {
            Quadruple *quad2 = quads->at(j);
            IntrCodeElem *cOpr1 = quad2->getOpr1();
            IntrCodeElem *cOpr2 = quad2->getOpr2();
            IntrCodeElem *cRes = quad2->getRes();
            //cout << "\n*****" << quad2->toString();
            if (isDef) {
                if (cOpr1 && cOpr1->equals(mRes))
                    break;
                if (cOpr2 && cOpr2->equals(mRes))
                    break;
                if (cRes && cRes->equals(mRes)) {
                    quads->erase(quads->begin() + i);
                    i--;
                    quadSize = quads->size();
                    break;
                }
                continue;
            }
            if (cOpr1 && replaceSet->count(cOpr1->getElem()))
                quad2->setOpr1(mRes);
            if (cOpr2 && replaceSet->count(cOpr2->getElem()))
                quad2->setOpr2(mRes);
            if (cRes && (cRes->equals(mOpr1) || cRes->equals(mOpr2)))
                break; // Variables reassigned
            if (!isAirthmeticOpr(quad2->getOpc()))
                continue;
            if (quad2->getOpc() == quad->getOpc() && ((mOpr1->equals(cOpr1) && mOpr2->equals(cOpr2))
                    || (associative && mOpr1->equals(cOpr2) && mOpr2->equals(cOpr1)))) {
                //	cout <<"~~~~~~";
                replaceSet->insert(cRes->getElem());
                //	cout << "\n&&&&&&&&&&&&&&&&&&& " << cRes->getElem();
                quads->erase(quads->begin() + j);
                j--;
                quadSize = quads->size();
            }
        }
        delete(replaceSet);
    }
    quadSize = quads->size();
    for(int i = 0; i < quadSize - 1; i++) {
        int replaceIndex = -1;
        Quadruple *quad = quads->at(i);
        IntrCodeElem *mRes = quad->getRes();
        if (!mRes || mRes->getType() != IntrCodeElem::ElemType::TEMP_VAR_TYPE)
            continue;
        int j;
        for(j = i + 1; j < quadSize; j++) {
            Quadruple *quad2 = quads->at(j);
            IntrCodeElem *cOpr1 = quad2->getOpr1();
            IntrCodeElem *cOpr2 = quad2->getOpr2();
            if (isAssigment(quad2->getOpc()) && cOpr1->equals(mRes)) {
                if (replaceIndex > -1) {
                    break;
                } else {
                    replaceIndex = j;
                    continue;
                }
            }
            if (cOpr1 && cOpr1->equals(mRes))
                break;
            if (cOpr2 && cOpr2->equals(mRes))
                break;
        }

        if (j == quadSize && replaceIndex > -1) {
            Quadruple *quad2 = quads->at(replaceIndex);
            quad->setRes(quad2->getRes());
            quads->erase(quads->begin() + replaceIndex);
            quadSize--;
        }
    }



    /*
       cout << "\nAfter Optimization\n";

       for(vector<Quadruple*>::iterator it = quads->begin(); it != quads->end(); ++it) {
       cout << (*it)->toString();
       }
       cout << "\nOptimization END\n";
     */
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
    new OpCodeInstMap(OpNode::OpCode::INVALID, {}),
    new OpCodeInstMap(OpNode::OpCode::JMP, {Instruction::InstructionSet::JMP}),
    new OpCodeInstMap(OpNode::OpCode::JMPC, {Instruction::InstructionSet::JMPC}),
    new OpCodeInstMap(OpNode::OpCode::CALL, {}),
    new OpCodeInstMap(OpNode::OpCode::RET, {}),
    new OpCodeInstMap(OpNode::OpCode::MOVIF, {Instruction::InstructionSet::MOVIF}),
    new OpCodeInstMap(OpNode::OpCode::IN, {Instruction::InstructionSet::INI, Instruction::InstructionSet::INF}),
    new OpCodeInstMap(OpNode::OpCode::DEFAULT, {})
};

static string instructionParam(IntrCodeElem *e, vector<Instruction*> *inst_vec) {
    if(!e)
        return "";
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
    {
        vector<Quadruple*> *temp = new vector<Quadruple*>();
        temp->push_back((Quadruple*)(e->getElem()));
        return Quadruple::iCodeToAsmGen(temp, false, false)->at(0)->toString(false);
    }
    case IntrCodeElem::ElemType::LABEL_TYPE:
        return ((IntrLabel*)(e->getElem()))->getLabel();
    }
    return "";
}

static vector<Instruction*>* getInstructionSet(OpNode::OpCode opc, IntrCodeElem *e1, IntrCodeElem *e2, IntrCodeElem *e3, string label, string comment) {
    int instNum = 0;
    vector<Instruction*> *inst_vec = new vector<Instruction*>();
    if (e3) {
        Type *inst_type = e3->getElem()->type();
        instNum = Type::isFloat(inst_type->tag()) ? 1:0;
    }
    string param1 ="" ,param2 = "", param3 = "";
    switch(opc) {
    case OpNode::OpCode::LT:
    case OpNode::OpCode::LE:
        opc = OpNode::OpCode::GE;
        swap(e1,e2);
    case OpNode::OpCode::GT:
    case OpNode::OpCode::GE:
    {
        Type *inst_type = e1->getElem()->type();
        instNum = Type::isFloat(inst_type->tag()) ? 1:0;
    }
    break;
    case OpNode::OpCode::ASSIGN:
    {
        Type *inst_type = e3->getElem()->type();
        if(Type::isString(inst_type->tag()))
            instNum = 2;
    }
    break;
    case OpNode::OpCode::SHL:
        break;
    case OpNode::OpCode::SHR:
        break;
    default:
        break;
    }
    Instruction::InstructionSet instCode = OpCodeInstMap::fetchInstr(opc, instNum);
    param1 = instructionParam(e1, inst_vec);
    param2 = instructionParam(e2, inst_vec);
    param3 = instructionParam(e3, inst_vec);

    Instruction *inst = new Instruction(instCode, param1, param2, param3, label, comment);
    inst_vec->push_back(inst);
    return inst_vec;
}

static void insertIntoSet(IntrCodeElem* e,  set<VariableEntry*> *entrySet) {
    if (e == NULL)
        return;
    switch(e->getType()) {
    case IntrCodeElem::ElemType::VAR_TYPE:
    case IntrCodeElem::ElemType::TEMP_VAR_TYPE:
    case IntrCodeElem::ElemType::REF_EXPR_TYPE:
        entrySet->insert((VariableEntry*)(e->getElem()));
        break;
    default:
        break;
    }

}
//How to handle global?
vector<Instruction*>* Quadruple::iCodeToAsmGen(vector<Quadruple*> *quad, bool showComment, bool purgeRegisters) {
    optimiseQuadruples(quad);
    IntrCodeElem *ve1, *ve2, *ve3;
    string label = "";
    set<VariableEntry*> *entrySet = new set<VariableEntry*>();
    vector<Instruction*>* inst_set = new vector<Instruction*>();
    vector<Instruction*> *instructionSet;
    OpNode::OpCode opc;
    for(vector<Quadruple*>::iterator it = quad->begin(); it != quad->end(); ++it) {

        opc = (*it)->getOpc();
        ve1 = (*it)->getOpr1();
        ve2 = (*it)->getOpr2();
        ve3 = (*it)->getRes();
        label = (*it)->getLabel();

        if (opc == OpNode::OpCode::CALL) {
            std::set<VariableEntry*>::iterator it;
            string reg = instructionParam(ve3, inst_set);
            for (it=entrySet->begin(); it!=entrySet->end(); ++it) {
                VariableEntry *ve = (*it);
                if (ve->getReg() == "")
                    continue;
                if (ve->varKind() == VariableEntry::VarKind::GLOBAL_VAR && !ve->isMem())
                    continue;
                bool isFloat = Type::isFloat(ve->type()->tag());
                inst_set->push_back(new Instruction(isFloat ? Instruction::InstructionSet::STF : Instruction::InstructionSet::STI,
                                                    ve->getReg(), SP_REG, "" , "", "Flushing Registers: " + ve->name()));
                inst_set->push_back(Instruction::decrSP());
            }
            InvocationNode *in = (InvocationNode*)(ve1->getElem());

            vector<IntrCodeElem*> *params = ((IntrCodeParams*)(ve2->getElem()))->getParams();
            for(vector<IntrCodeElem*>::reverse_iterator it = params->rbegin(); it != params->rend(); ++it) {
                string val = instructionParam((*it), inst_set);
                insertIntoSet((*it), entrySet);
                bool isFloat = Type::isFloat((*it)->getElem()->type()->tag());
                inst_set->push_back(new Instruction(isFloat ? Instruction::InstructionSet::STF : Instruction::InstructionSet::STI,
                                                    val, SP_REG, "", "", "Pushing parameter: " + IntrCodeElem::toString((*it))));
                inst_set->push_back(Instruction::decrSP());
            }

            mergeVec(inst_set, in->codeGen());
            VariableEntry *retVe = (VariableEntry*)(ve3->getElem());
            bool isFloat = Type::isFloat(retVe->type()->tag());
            inst_set->push_back(new Instruction(isFloat ? Instruction::InstructionSet::MOVF : Instruction::InstructionSet::MOVI
                                                , retVe->getReg(), isFloat ? RETF_REG : RETI_REG, "", "", "Getting return Value"));
            std::set<VariableEntry*>::reverse_iterator rit;
            for (rit=entrySet->rbegin(); rit!=entrySet->rend(); ++rit) {
                VariableEntry *ve = (*rit);
                if (ve->getReg() == "")
                    continue;
                if (ve->varKind() == VariableEntry::VarKind::GLOBAL_VAR && !ve->isMem())
                    continue;
                bool isFloat = Type::isFloat(ve->type()->tag());
                inst_set->push_back(new Instruction(isFloat ? Instruction::InstructionSet::LDF : Instruction::InstructionSet::LDI,
                                                    SP_REG, ve->getReg(), "", "", "Loading Back Registers:" + ve->name()));
                inst_set->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, "1", SP_REG));
            }
            continue;
        }

        insertIntoSet(ve1, entrySet);
        insertIntoSet(ve2, entrySet);
        insertIntoSet(ve3, entrySet);
        switch (opc) {
        case OpNode::OpCode::RET:
        {
            string reg = instructionParam(ve1, inst_set);
            VariableEntry *ve = (VariableEntry*)(ve1->getElem());
            bool isFloat = Type::isFloat(ve->type()->tag());
            inst_set->push_back(new Instruction(isFloat ? Instruction::InstructionSet::MOVF:Instruction::InstructionSet::MOVI, reg, isFloat ? RETF_REG : RETI_REG, "", label));
            continue;
        }
        case OpNode::OpCode::DEFAULT:
            inst_set->push_back(new Instruction(instructionParam(ve1, inst_set)));
            continue;
        default:
            break;
        }
        instructionSet = getInstructionSet(opc, ve1, ve2, ve3, label, showComment ? (*it)->toString(false) : "");
        mergeVec(inst_set, instructionSet);
        delete(instructionSet);
    }
    if (purgeRegisters) {
        Quadruple::resetTempCnt();
        std::set<VariableEntry*>::iterator it;
        for (it=entrySet->begin(); it!=entrySet->end(); ++it) {
            VariableEntry *ve = (*it);
            if (ve->getReg() == "")
                continue;
            if (ve->varKind() == VariableEntry::VarKind::GLOBAL_VAR && !ve->isMem())
                continue;
            regMgr->purgeReg(ve->getReg());
        }
    }
    return inst_set;
}

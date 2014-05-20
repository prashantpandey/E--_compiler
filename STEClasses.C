
#include "IncludeHeaders.h"
#include "Value.h"
#include "ParserUtil.h"

void GlobalEntry::print(ostream& out, int indent) const
{
    printST(out, indent, '\0', ';', true);
    prtln(out, indent);
    if (rules_.size() == 0) {
        prtln(out, indent);
    } else {
        for(vector<RuleNode*>::const_iterator it = rules_.begin(); it != rules_.end(); ++it) {
            (*it)->print(out, indent+STEP_INDENT);
            endln(out, indent);
        }
    }
    prtSpace(out, indent+STEP_INDENT);
}


void GlobalEntry::checkType() const
{
    const SymTab *st = NULL;
    if ((st = symTab()) != nullptr) {
        SymTab::const_iterator it = st->begin();
        for (int i=0; it != (st->end()); i++, ++it)  {
            SymTabEntry *ste = (SymTabEntry *)(*it);
            ste->checkType();
        }
    }
    if (rules_.size() != 0) {
        for(vector<RuleNode*>::const_iterator it = rules_.begin(); it != rules_.end(); ++it) {
            (*it)->typeCheck();
        }
    }
}


static void pICode(vector<Quadruple*> *p) {
    ostringstream os;
    for(vector<Quadruple*>::iterator it = p->begin(); it != p->end(); ++it) {
        os << (*it)->toString();
    }
    cout << os.str() << endl;
}

void GlobalEntry::genFinalCode(string progName) {
    const SymTab *st = NULL;
    progCode_ = new ProgCode(progName);
    vector<Quadruple*> *iCodeTable_ = new vector<Quadruple*>();
    vector<string> *ruleNames = new vector<string>();

    // initialize the global vars in global/static section
    if ((st = symTab()) != nullptr) {
        SymTab::const_iterator it = st->begin();
        CodeModule* codeModGlobalSec =  new CodeModule("GlobalSec");
	codeModGlobalSec->insertInstructionSet(new Instruction(Instruction::InstructionSet::MOVI,"10000", SP_REG, "", "begin"));
        CodeModule* codeMode =  NULL;
        for (; it != (st->end()); ++it) {
            SymTabEntry *ste = (SymTabEntry *)(*it);
            if (ste->kind() == SymTabEntry::Kind::VARIABLE_KIND && ((VariableEntry*)ste)->varKind() == VariableEntry::VarKind::GLOBAL_VAR) {
                VariableEntry *ve = (VariableEntry*)ste;
                if (!ve->isMem()) {
                    ve->setReg(regMgr->fetchNextAvailReg(!Type::isFloat(ve->type()->tag())));
                }
                IntrCodeElem *elem = NULL;
                if(ve->initVal() == NULL) {
                    elem = new IntrCodeElem(new ValueNode(new Value(0, ve->type()->tag())), IntrCodeElem::ElemType::VAL_TYPE);
                }
                else {
                    mergeVec(iCodeTable_, ve->initVal()->iCodeGen());
                    elem = ve->initVal()->getTVar();
                }
                iCodeTable_->push_back(new Quadruple(OpNode::OpCode::ASSIGN, elem, NULL, new IntrCodeElem(ve, IntrCodeElem::ElemType::VAR_TYPE)));

            }
            if (ste->kind() == SymTabEntry::Kind::FUNCTION_KIND) {
                FunctionEntry *fe = (FunctionEntry *)ste;
                codeMode = new CodeModule(fe->name());
                codeMode->insertInstructionSet(fe->codeGen());
                progCode_->insertModule(codeMode);
            }

        }
        pICode(iCodeTable_);
        codeModGlobalSec->insertInstructionSet(Quadruple::iCodeToAsmGen(iCodeTable_));
        if (rules_.size() != 0) {
            for(vector<RuleNode*>::const_iterator it = rules_.begin(); it != rules_.end(); ++it) {
                codeMode = new CodeModule("Rule");
                codeMode->insertInstructionSet((*it)->codeGen());
                ruleNames->push_back((*it)->getJmpName());
                codeMode->insertInstructionSet(new Instruction(Instruction::InstructionSet::JMP, "EventMStart"));
                progCode_->insertModule(codeMode);
            }
        }
        vector<Instruction*> *inst_set = new vector<Instruction*>();

        string evnReg = regMgr->fetchNextAvailReg(true);
        inst_set->push_back(new Instruction(Instruction::InstructionSet::IN, evnReg));
        inst_set->push_back(new Instruction(Instruction::InstructionSet::IN, evnReg));
        inst_set->push_back(new Instruction(Instruction::InstructionSet::IN, evnReg));
        inst_set->push_back(new Instruction(Instruction::InstructionSet::IN, evnReg, "", "", "EventMStart"));
        inst_set->push_back(new Instruction(Instruction::InstructionSet::JMPC, "GT 64 " + evnReg, "EventMOut", "", ""));
        bool anyEvent = false;
        for(unsigned int i=0; i < ruleNames->size(); i++) {
            string name = ruleNames->at(i);
            if (name.compare("any") == 0) {
                anyEvent = true;
                continue;
            }
            int f = name.at(0);
            inst_set->push_back(new Instruction(Instruction::InstructionSet::JMPC, "EQ " + to_string(f) + " " + evnReg, PrimitivePatNode::labelPrefix + name));
        }
        if(anyEvent)
            inst_set->push_back(new Instruction(Instruction::InstructionSet::JMP, PrimitivePatNode::labelPrefix + "any"));
        inst_set->push_back(new Instruction(Instruction::InstructionSet::JMP, "EventMStart"));
        inst_set->push_back(new Instruction(Instruction::InstructionSet::PRTS, "\"\\nDone\\n\"", "", "", "EventMOut"));
        codeModGlobalSec->insertInstructionSet(inst_set);
	
        progCode_->insertModule(codeModGlobalSec);
    }

    // TODO: Add code for function and rule modules
}

void GlobalEntry::serializeAsm(ostream& os) const
{
    vector<CodeModule*> *modules = progCode_->getModule();
    os << "JMP begin\n";
    for(vector<CodeModule*>::iterator it = modules->begin(); it != modules->end(); ++it) {
        os << "//\n";
	for (unsigned int i = 0; i < (*it)->getInstructions()->size(); i++) {
	    Instruction *inst = (*it)->getInstructions()->at(i);
	    if(inst->getInstr() == Instruction::InstructionSet::BLANK) {
		Instruction *nextInst = (*it)->getInstructions()->at(i + 1);
		if (nextInst->getLabel() != "") {
		    string labelToReplace = nextInst->getLabel();
		    nextInst->setLabel("");
		    string labelToReplaceWith = inst->getLabel();
		    //merge labels
		    for (unsigned int j = 0; j < (*it)->getInstructions()->size(); j++) {
			nextInst = (*it)->getInstructions()->at(j);
			switch(nextInst->getInstr())
			{
			case Instruction::InstructionSet::JMP: 
			    if (nextInst->getParam1() == labelToReplace)
				nextInst->setParam1(labelToReplaceWith);
			    break;
			case Instruction::InstructionSet::JMPC: 
			    if (nextInst->getParam2() == labelToReplace)
				nextInst->setParam2(labelToReplaceWith);
			    break;
			default:
			    if (nextInst->getLabel() == labelToReplace)
				nextInst->setLabel(labelToReplaceWith);
			    break;
			}
		    }
		}
	    }
	}
        for (vector<Instruction*>::iterator inst = ((*it)->getInstructions())->begin(); inst != ((*it)->getInstructions())->end(); ++inst) {
            os << (*inst)->toString();
        }
    }
}

void EventEntry::print(ostream& out, int indent) const
{
    out << "event"  << " " << name() ;
    printST(out, indent, '(', ')', false);

}


void VariableEntry::print(ostream& out, int indent) const
{
    out << type()->fullName() << " " << name();
    if (initVal()) {
        out << " = ";
        initVal()->print(out);
    }
}


void VariableEntry::checkType() const
{
    if (initVal()) {
        const Type *rhsT = initVal_->doTypeCheck();
        if (isConst()) {
            errMsg("Cannot change read only variables.", this);
        }
        else if (rhsT->tag() != Type::TypeTag::ERROR) {
            if (rhsT->tag() == Type::TypeTag::CLASS && !Type::isSubType(rhsT, type())) {
                errMsg("Assignment between incompatible types", this);
            }
            else if (rhsT->tag() !=  type()->tag() && !Type::isSubType(rhsT, type())) {
                errMsg("Assignment between incompatible types", this);
            }
        }
    }
}

vector<Instruction*>* VariableEntry::codeGen() {
    string val;
    vector<Instruction*> *inst_vec = new vector<Instruction*>();
    regName_ = regMgr->fetchNextAvailReg(!Type::isFloat(type()->tag()));
    if(initVal() == NULL || initVal()->value() == NULL) {
        val = "0";
    }
    else {
        inst_vec = fetchExprRegValue();
        val = "0";//TODO:initVal()->getTVar();
    }

    Instruction::InstructionSet movInst = Instruction::InstructionSet::MOVI;
    Instruction::InstructionSet storeInst = Instruction::InstructionSet::STI;
    if (Type::isString(type()->tag())) {
        movInst = Instruction::InstructionSet::MOVS;
    }
    else if (Type::isFloat(type()->tag())) {
        movInst = Instruction::InstructionSet::MOVF;
        storeInst = Instruction::InstructionSet::STF;
    }
    inst_vec->push_back(new Instruction(movInst, val, regName_));
    regMgr->purgeReg(val);
    switch(varKind()) {
    case VariableEntry::VarKind::LOCAL_VAR :
        inst_vec->push_back(Instruction::decrSP());
        break;
    case VariableEntry::VarKind::PARAM_VAR :
    case VariableEntry::VarKind::GLOBAL_VAR :
        /* If is mem is set then storing the corresponding
         * global variable register to the global section,
         * updating the stack pointer and purging the register*/
        if (isMem()) {
            inst_vec->push_back(new Instruction(storeInst, regName_, SP_REG));
            inst_vec->push_back(Instruction::decrSP());
            regMgr->purgeReg(regName_);
        }
        break;
    case VariableEntry::VarKind::UNDEFINED :
        break;
    case VariableEntry::VarKind::TEMP_VAR :
        break;
    }
    return inst_vec;
}

vector<Instruction*>* VariableEntry::fetchExprRegValue() {
    vector<Instruction*>* exprInst = new vector<Instruction*>();
    ExprNode* expr = initVal();
    switch(expr->exprNodeType()) {
    case ExprNode::ExprNodeType::OP_NODE:
        insertQuadrupleSet(initVal()->iCodeGen());
        break;
    case ExprNode::ExprNodeType::REF_EXPR_NODE:
        ((VariableEntry*)((RefExprNode*)expr)->symTabEntry())->getReg();
        break;
    case ExprNode::ExprNodeType::VALUE_NODE:
        ((ValueNode*)expr)->value()->toString();
        break;
    case ExprNode::ExprNodeType::INV_NODE:
        insertQuadrupleSet(initVal()->iCodeGen());
        break;
    }
    // TODO: Call code generation on the quadruple table
    // Also perform the required optimization over the quadruple table and
    // generate the respective resultant reg value
    return exprInst;
}

void FunctionEntry::printICode() {
    pICode(iCodeTable_);
}


vector<Instruction*>* FunctionEntry::codeGen() {

    if(body() == NULL) {
        return NULL;
    }

    vector<Instruction*> *inst_vec = new vector<Instruction*>();
    aLabel_ = regMgr->getNextLabel();
    exitLabel_ = regMgr->getNextLabel();
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, "2" ,TEMP_REG, aLabel_,
                                        "Function Start-" + name() + ", Setting temp register to load params"));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::STI, BP_REG, SP_REG, "",
                                        "", "Saving BP"));
    inst_vec->push_back(Instruction::decrSP());
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::MOVI, SP_REG, BP_REG, "", "", "Saving SP to BP"));
    vector<Quadruple*> *iquad;
    const SymTab *st = NULL;
    if ((st = symTab()) != nullptr) {
        for (SymTab::const_iterator it = st->begin(); it != (st->end()); ++it)  {
            SymTabEntry *ste = (SymTabEntry *)(*it);
            if ((ste->kind() == SymTabEntry::Kind::VARIABLE_KIND)) {
                VariableEntry *ve = (VariableEntry *) ste;
                if (ve->varKind() == VariableEntry::VarKind::LOCAL_VAR) {
                    IntrCodeElem *elem = NULL;
                    if(ve->initVal() == NULL) {
                        elem = new IntrCodeElem(new ValueNode(new Value(0, ve->type()->tag())), IntrCodeElem::ElemType::VAL_TYPE);
                    }
                    else {
                        iquad = ve->initVal()->iCodeGen();
                        mergeVec(iCodeTable_, iquad);
                        elem = ve->initVal()->getTVar();
                    }
                    iCodeTable_->push_back(new Quadruple(OpNode::OpCode::ASSIGN, elem, NULL, new IntrCodeElem(ve, IntrCodeElem::ElemType::VAR_TYPE)));
                } else if(ve->varKind() == VariableEntry::VarKind::PARAM_VAR) {
                    bool isFloat = Type::isFloat(ve->type()->tag());
                    ve->setReg(regMgr->fetchNextAvailReg(!isFloat, ve, 0, inst_vec));
                    inst_vec->push_back(new Instruction(isFloat ? Instruction::InstructionSet::LDF : Instruction::InstructionSet::LDI, TEMP_REG,
                                                        ve->getReg(), "", "", "Loading param: " + ve->name()));
                    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, TEMP_REG, "1" ,TEMP_REG));

                }
            }
        }
    }

    iquad = body_->iCodeGen();

    mergeVec(iCodeTable_, iquad);
    printICode();
    mergeVec(inst_vec, Quadruple::iCodeToAsmGen(iCodeTable_));

    inst_vec->push_back(new Instruction(Instruction::InstructionSet::MOVI, BP_REG, SP_REG, "", exitLabel_,"Function Exit: Restoring BP"));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, "1", SP_REG));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::LDI, SP_REG, BP_REG, "", "", "Loading BP from stack"));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, "1", SP_REG));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::LDI, SP_REG, RET_ADDR_REG, "", "", "Getting Return Address from stack"));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, to_string(getArgCnt()), SP_REG));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::JMPI, RET_ADDR_REG));
    return inst_vec;
}

void FunctionEntry::checkType() const
{
    const SymTab *st = NULL;
    if ((st = symTab()) != nullptr) {
	for (SymTab::const_iterator it = st->begin(); it != (st->end()); ++it)  {
	    SymTabEntry *ste = (SymTabEntry *)(*it);
	    if ((ste->kind() == SymTabEntry::Kind::VARIABLE_KIND)) {
		VariableEntry *ve = (VariableEntry *) ste;
		if (ve->varKind() == VariableEntry::VarKind::LOCAL_VAR) {
		    if(ve->initVal() != NULL) {
			ve->initVal()->doTypeCheck();
		    }
		}
	    }
	}
    }

    if (body())
	body()->typeCheck();
}

void ClassEntry::print(ostream& out, int indent) const
{
    out << "class" << " " << name();
}

void UnknownKindEntry::print(ostream& out, int indent) const
{
    // Nothing required to be printed.
}

void BlockEntry::print(ostream& out, int indent) const
{
    out << " {";
    printST(out, indent, '\0', ';', true);
    out << "}";

}

void WhileBlockEntry::print(ostream& out, int indent) const
{
    out << " {";
    printST(out, indent, '\0', ';', true);
    out << "}";

}

void FunctionEntry::print(ostream& out, int indent) const
{
    out <<  type()->fullName() << " " << name() << "(";
    const SymTab *st = NULL;
    int i = 0;
    if ((st = symTab()) != nullptr) {
        SymTab::const_iterator it = st->begin();
        for (i=0; it != (st->end()); i++, ++it)  {
            SymTabEntry *ste = (SymTabEntry *)(*it);
            if ((ste->kind() == SymTabEntry::Kind::VARIABLE_KIND)) {
                VariableEntry *ve = (VariableEntry *) ste;
                if (ve->varKind() != VariableEntry::VarKind::PARAM_VAR) {
                    break;
                }
            } else {
                break;
            }
        }
        if (i != 0) {
            printST(out, indent, '\0', '\0', false, 0, i);
        }

    }
    out << ")";
    if (body() != NULL || (st != nullptr && st->size() > i)) {
        cout << " {";
        if (st != nullptr && st->size() > i)
            printST(out, indent, '\0', ';', true, i, st->size());
        else
            prtln(out, indent);
        if (body() != NULL)
            body()->printWithoutBraces(out, indent);
        cout << "}";
    }
}

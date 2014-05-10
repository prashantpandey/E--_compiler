
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



void GlobalEntry::genFinalCode(string progName) {
    const SymTab *st = NULL;
    progCode_ = new ProgCode(progName);

    // initialize the global vars in global/static section
    if ((st = symTab()) != nullptr) {
	SymTab::const_iterator it = st->begin();
	CodeModule* codeModGlobalSec =  new CodeModule("GlobalSec");
	CodeModule* codeMode =  NULL;
	for (; it != (st->end()); ++it) {
	    SymTabEntry *ste = (SymTabEntry *)(*it);
	    if (ste->kind() == SymTabEntry::Kind::VARIABLE_KIND && ((VariableEntry*)ste)->varKind() == VariableEntry::VarKind::GLOBAL_VAR) {
		codeModGlobalSec->insertInstructionSet(((VariableEntry*)ste)->codeGen());
	    }
	    if (ste->kind() == SymTabEntry::Kind::FUNCTION_KIND) {
		FunctionEntry *fe = (FunctionEntry *)ste;
		codeMode = new CodeModule(fe->name());
		codeMode->insertInstructionSet(fe->codeGen());
		progCode_->insertModule(codeMode);
	    }

	}
	if (rules_.size() != 0) {
	    for(vector<RuleNode*>::const_iterator it = rules_.begin(); it != rules_.end(); ++it) {
		codeMode = new CodeModule("Rule");
		codeMode->insertInstructionSet((*it)->codeGen());
		codeMode->insertInstructionSet(new Instruction(Instruction::InstructionSet::JMPI, RET_ADDR_EREG));
		progCode_->insertModule(codeMode);
	    }
	}
	Instruction* firstInst = codeModGlobalSec->firstInst();
	if (firstInst)
	    firstInst->setLabel("begin");
	progCode_->insertModule(codeModGlobalSec);
    }

    // TODO: Add code for function and rule modules
}

void GlobalEntry::serializeAsm(ostream& os) const
{
    vector<CodeModule*> *modules = progCode_->getModule();
    
    for(vector<CodeModule*>::iterator it = modules->begin(); it != modules->end(); ++it){
	for (vector<Instruction*>::iterator inst = ((*it)->getInstructions())->begin(); inst != ((*it)->getInstructions())->end(); ++inst){
	    os << (*inst)->toString();
	}
	    os << "\n";
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
    if(initVal() == NULL || initVal()->value() == NULL){
	val = "0";
    }
    else {
	inst_vec = fetchExprRegValue();
	val = getTReg();
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
	    // TODO: Call code generation on the quadruple table 
	    break;
	case ExprNode::ExprNodeType::REF_EXPR_NODE:
	    setTReg(((VariableEntry*)((RefExprNode*)expr)->symTabEntry())->getReg());
	    break;
	case ExprNode::ExprNodeType::VALUE_NODE:
	    setTReg(((ValueNode*)expr)->value()->toString());
	    break;
	case ExprNode::ExprNodeType::INV_NODE:
	    insertQuadrupleSet(initVal()->iCodeGen());
	    // TODO: Call code generation on the quadruple table 
	    break;
    }
    // TODO: Call code generation on the quadruple table 
    // Also perform the required optimization over the quadruple table and
    // generate the respective resultant reg value
    return exprInst;
}

vector<Instruction*>* FunctionEntry::codeGen() {

    if(body() == NULL){
	return NULL;
    }

    vector<Instruction*> *inst_vec = new vector<Instruction*>();
    aLabel_ = regMgr->getNextLabel();
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::STI, BP_REG, SP_REG, "", aLabel_, "Function Start: Saving BP"));
    inst_vec->push_back(Instruction::decrSP());
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::MOVI, SP_REG, BP_REG));

    const SymTab *st = NULL;
    if ((st = symTab()) != nullptr) {
	for (SymTab::const_iterator it = st->begin(); it != (st->end()); ++it)  {
	    SymTabEntry *ste = (SymTabEntry *)(*it);
	    if ((ste->kind() == SymTabEntry::Kind::VARIABLE_KIND)) {
		VariableEntry *ve = (VariableEntry *) ste;
		if (ve->varKind() == VariableEntry::VarKind::LOCAL_VAR) {
		    mergeVec(inst_vec, ve->codeGen());
		}
	    }
	}
    }
    
    mergeVec(inst_vec, body()->codeGen());
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::MOVI, BP_REG, SP_REG, "", "" ,"Function Exit: Restoring BP"));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, "1", SP_REG));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::LDI, SP_REG, BP_REG, "", "", "Loading BP from stack"));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, "1", SP_REG));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::LDI, SP_REG, RET_ADDR_REG, "", "", "Getting Return Address from stack"));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::ADD, SP_REG, to_string(getArgCnt()+1), SP_REG));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::JMPI, RET_ADDR_REG));
    return inst_vec;
}

void FunctionEntry::checkType() const
{
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

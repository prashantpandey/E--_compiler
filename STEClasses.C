#include "STEClasses.h"
#include "RegMgr.h"
#include "CodeGen.h"
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
	CodeModule* codeMod = NULL;
        for (; it != (st->end()); ++it) {
            SymTabEntry *ste = (SymTabEntry *)(*it);
            if(ste->kind() == SymTabEntry::Kind::VARIABLE_KIND && ((VariableEntry*)ste)->varKind() == VariableEntry::VarKind::GLOBAL_VAR) {
		codeMod = new CodeModule("GlobalSec");
                codeMod->insertInstructionSet(((VariableEntry*)ste)->codeGen());
            }
        }
	progCode_->insertModule(codeMod);
    }
    
    // TODO: Add code for function and rule modules
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
        const Type *rhsT = initVal()->typeCheck();
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

vector<Instruction*>* VariableEntry::codeGen() const {
    
    string regName;
    vector<Instruction*> inst_vec;
    switch(varKind()){
	case VariableEntry::VarKind::GLOBAL_VAR :  
	

						    regName = regMgr->fetchNextAvailReg(!isFloat(type->tag()));
						    regName_ = regName;

						    // initVal type is ExprNode*, check how does this work
						    if(isInt(type->tag()))
						    {
		         				inst_vec.push_back(new Instruction(Instruction::InstructionSet::MOVI, initVal(), regName));

							/* If is mem is set then storing the corresponding
							 * global variable register to the global section,
							 * updating the stack pointer and purging the register*/
							
							if(isMem()){
							    inst_vec.push_back(new Instruction(Instruction::InstructionSet::STI, regName, GLOBALI_REG));
							    inst_vec.push_back(CodeGen::decrSP());
							    regMgr->purgeReg(regName);
							}
						    
						    }

						    else if(isString(type->tag())){
							inst_vec.push_back(new Instruction(Instruction::InstructionSet::MOVS, initVal(), regName));
							if(isMem()){
							    inst_vec.push_back(new Instruction(Instruction::InstructionSet::STI, regName, GLOBALI_REG));
							    inst_vec.push_back(CodeGen::decrSP());
							    regMgr->purgeReg(regName);
							}
						    
						    }

						    else if(isFloat(type->tag())){
							inst_vec.push_back(new Instruction(Instruction::InstructionSet::MOVF, initVal(), regName));
							if(isMem()){
							    inst_vec.push_back(new Instruction(Instruction::InstructionSet::STF, regName, GLOBALF_REG));
							    inst_vec.push_back(CodeGen::decrSP());
							    regMgr->purgeReg(regName);
							}
						    
						    }
						    
						    break;
	case VariableEntry::VarKind::LOCAL_VAR :
						    break;
	case VariableEntry::VarKind::PARAM_VAR :    
						    break;
	case VariableEntry::VarKind::UNDEFINED : 
						    break;
    }
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

#include "STEClasses.h"
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

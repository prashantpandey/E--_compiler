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

#include "CodeGen.h"

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

string Quadruple::fetchTempVar(){
    ostringstream os;
    os << "T" << tempCnt_++;
    return os.str();
}

void Quadruple::resetTempCnt(){
    tempCnt_ = 0;
}

bool Quadruple::isEqual(Quadruple *quad){
    if(quad->opc_ == opc_) {
	if(quad->opr1_->name().compare(opr1_->name()) == 0) {
	    if(quad->opr2_->name().compare(opr2_->name()) ==0) {
		return true;
	    }
	}
    }
    return false;
}

vector<Instruction*>* Quadruple::iCodeToAsmGen(vector<Quadruple*> *quad){
   // TODO:: IMplement Expression Optimization 

    VariableEntry *ve1, *ve2, *ve3;
    string regName1 = "", regName2 = "", regName3 = "";
    vector<Instruction*>* inst_set = new vector<Instruction*>();
    Instruction *instr;
    OpNode::OpCode opc;
    for(vector<Quadruple*>::iterator it = quad->begin(); it != quad->end(); ++it){
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
//	    instr = new Instruction(getInstr(opc), regName1, regName2, regName3);
//	    inst_set->push_back(instr);
	
    }
    return inst_set;
}

/*
Instruction::InstructionSet getInstr(OpNode::OpCode opc){
    switch(opc){
    
	case OpNode::OpCode::UMINUS:
	case OpNode
    }
}
*/
bool Quadruple::checkRegOrTemp(VariableEntry *ve, string &regName){

	    if(ve != NULL){
		    if(ve->isTemp()){ // temperary true
			regName = regMgr->fetchNextAvailReg(!Type::isInt(ve->type()->tag()), ve, 0);
			ve->setReg(regName);
		    }
		    else{
			regName = ve->getReg();
		    }
	    }
    delete(ve);
    return ve->isTemp();
}

/*
 * =====================================================================================
 *
 *       Filename:  RegMgr.C
 *
 *    Description: Implementation of RegMgr.h
 *
 *        Version:  1.0
 *        Created:  04/25/2014 01:15:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ankur, Prashant, Kavita
 *   Organization:  Stony Brook University.
 *
 * =====================================================================================
 */

#include "RegMgr.h"


int RegMgr::cnt_ = 0;

RegMgr::RegMgr() {

    iReg_ = new bool[INT_REG_AVAIL];
    fReg_ = new bool[FLOAT_REG_AVAIL];
    iCountStart_ = fCountStart_ = 0;
    iRegUse_ = TOTAL_REG - INT_REG_AVAIL;
    fRegUse_ = TOTAL_REG - FLOAT_REG_AVAIL;

    for (int i = INT_REG_AVAIL - 1, j = FLOAT_REG_AVAIL - 1; i > -1 || j > -1; i--,j--) {
        if (i > -1)
            iReg_[i] = true;
        if (j > -1)
            fReg_[j] = true;
    }
}


string RegMgr::fetchNextAvailReg(bool isInt, VariableEntry *ve, int priority, vector<Instruction*> *instructionSet) {
    bool *reg = iReg_;
    int avail_reg =  INT_REG_AVAIL;
    int countStart = iCountStart_;
    ostringstream os;
    if (!isInt) {
        reg = fReg_;
        avail_reg = FLOAT_REG_AVAIL;
        countStart = fCountStart_;
        os << "F";
    } else {
        os << "R";
    }
    int tmp = countStart;

    do {
        if (reg[countStart]) {
            reg[countStart] = false;
            os << setw(3) << setfill('0') << countStart + TOTAL_REG - avail_reg;
            setfill(' ');
            countStart = (countStart + 1) % avail_reg;
            break;
        } else {
            countStart = (countStart + 1) % avail_reg;
        }
    } while (countStart != tmp);

    //Handle out of register

    if (isInt) {
        iCountStart_ = countStart;
        iRegUse_++;
    }
    else {
        fCountStart_ = countStart;
        fRegUse_++;
    }
    string reg_str = os.str();
    if (ve != NULL) {
	ve->setReg(reg_str);
	ve->setMem(false);
        regMap_.insert(make_pair<string, VEntryPriority*>(string(reg_str), new VEntryPriority(ve, priority)));
    }
    return reg_str;
}


void RegMgr::purgeReg(string regName) {

    const char *tmp = regName.c_str();
    tmp++;

    int reg = atoi(tmp);

    switch(regName[0])
    {
    case 'R' :
        iCountStart_ = reg - ( TOTAL_REG - INT_REG_AVAIL);
        iReg_[iCountStart_] = true;
    case 'F' :
        fCountStart_ = reg - (TOTAL_REG - FLOAT_REG_AVAIL);
        fReg_[fCountStart_] = true;
    }
    if (regMap_.count(regName) == 1) {
        regMap_.erase(regName);
    }
}

string RegMgr::getVEReg(VariableEntry *ve, vector<Instruction*> *inst_vec) {

    if (!ve->isMem())
        return ve->getReg();
    bool isFloat = Type::isFloat(ve->type()->tag());
    switch(ve->varKind()) {
    case  VariableEntry::VarKind::GLOBAL_VAR:
        ve->setReg(fetchNextAvailReg(!isFloat, ve, 0, inst_vec));
        inst_vec->push_back(new Instruction(Instruction::InstructionSet::SUB, GLOBAL_REG, to_string(ve->offSet()) ,TEMP_REG));
        inst_vec->push_back(new Instruction(isFloat ? Instruction::InstructionSet::LDF : Instruction::InstructionSet::LDI, TEMP_REG));
        break;
    case  VariableEntry::VarKind::LOCAL_VAR:
        ve->setReg(fetchNextAvailReg(!isFloat, ve, 0, inst_vec));
        inst_vec->push_back(new Instruction(Instruction::InstructionSet::SUB, BP_REG, to_string(ve->offSet()) ,TEMP_REG));
        inst_vec->push_back(new Instruction(isFloat ? Instruction::InstructionSet::LDF : Instruction::InstructionSet::LDI, TEMP_REG));
        break;
    case  VariableEntry::VarKind::TEMP_VAR:
        ve->setReg(fetchNextAvailReg(!isFloat, ve, 0, inst_vec));
    case  VariableEntry::VarKind::PARAM_VAR:
        //TODO
    default:
	break;
    }
    return ve->getReg();
}

string RegMgr::getNextLabel() {
    ostringstream os;
    os << "Label" << cnt_++;
    return os.str();
}

RegMgr *regMgr = &RegMgr::getInstance();

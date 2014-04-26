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


RegMgr::RegMgr() {

    iReg_ = new bool[INT_REG_AVAIL];
    fReg_ = new bool[FLOAT_REG_AVAIL];
    iCountStart_ = fCountStart_ = 0;

    for (int i = INT_REG_AVAIL - 1, j = FLOAT_REG_AVAIL - 1; i > -1 || j > -1; i--,j--) {
        if (i > -1)
            iReg_[i] = true;
        if (j > -1)
            fReg_[j] = true;
    }
}


string RegMgr::fetchNextAvailReg(bool isInt) {
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

    if (isInt)
        iCountStart_ = countStart;
    else
        fCountStart_ = countStart;
    return os.str();
}


void RegMgr::purgeReg(string regName) {

    const char *tmp = regName.c_str();
    tmp++;
    int reg = atoi(tmp);

    switch(regName[0]) {
    case 'R' :
        iCountStart_ = reg - ( TOTAL_REG - INT_REG_AVAIL);
        iReg_[iCountStart_] = true;
    case 'F' :
        fCountStart_ = reg - (TOTAL_REG - FLOAT_REG_AVAIL);
        fReg_[fCountStart_] = true;
    }
}


RegMgr *regMgr = &RegMgr::getInstance();
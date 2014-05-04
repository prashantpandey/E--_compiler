/*
 * =====================================================================================
 *
 *       Filename:  RegMgr.h
 *
 *    Description:  Manages register allocation and keeps bookmarking of free registers.
 *
 *        Version:  1.0
 *        Created:  04/25/2014 12:49:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ankur, Prashant, Kavita
 *   Organization:  Stony Brook University
 *
 * =====================================================================================
 */


#ifndef REGMGR_H
#define REGMGR_H

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <stack>
#include <unordered_map>
#include "STEClasses.h"

using namespace std;

#define SP_REG "R000"
#define BP_REG "R001"
#define RETI_REG "R002"
#define RETF_REG "F000"
#define GLOBAL_REG "R003"

#define GLOBAL_MAX_REG_COUNT 100

#define TOTAL_REG 1000
#define INT_REG_AVAIL 996
#define FLOAT_REG_AVAIL 999

class VariableEntry;
class VEntryPriority {

private:
    
    VariableEntry *ve_;
    int priority_;

public:

    VEntryPriority(VariableEntry *ve, int priority) {
        ve_ = ve;
        priority_ = priority;
    }

    VariableEntry* entry() const {
        return ve_;
    }

    int priority() const {
        return priority_;
    }

    void priority(int priority) {
        priority_ = priority;
    }
};

class RegMgr {

private:

    bool *iReg_, *fReg_;
    int iCountStart_, fCountStart_;
    unordered_map<string,VEntryPriority*> regMap_;

    RegMgr();

public:
    static RegMgr& getInstance() {
        static RegMgr instance; // Guaranteed to be destroyed.
        return instance;
    }

    string fetchNextAvailReg(bool isInt, VariableEntry *ve = NULL, int priority = 0);
    void purgeReg(string regName);


    ~RegMgr() {
        delete iReg_;
        delete fReg_;
    }

};

extern RegMgr *regMgr;

#endif

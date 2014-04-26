/*
 * =====================================================================================
 *
 *       Filename:  RegMgr.h
 *
 *    Description:  : Manages register allocation and keeps bookmarking of free registers.

 *
 *        Version:  1.0
 *        Created:  04/25/2014 12:49:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */


#ifndef REGMGR_H
#define REGMGR_H

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

#define SP_REG "R000"
#define BP_REG "R001"
#define RETI_REG "R002"
#define RETF_REG "F000"
#define GLOBALI_REG "R003"
#define GLOBALF_REG "F001"

#define TOTAL_REG 1000
#define INT_REG_AVAIL 996
#define FLOAT_REG_AVAIL 998

class RegMgr {

private:

    bool *iReg_, *fReg_;
    int iCountStart_, fCountStart_;

    RegMgr();

public:
    static RegMgr& getInstance() {
        static RegMgr instance; // Guaranteed to be destroyed.
        return instance;
    }

    string fetchNextAvailReg(bool isInt);
    void purgeReg(string regName);


    ~RegMgr() {
        delete iReg_;
        delete fReg_;
    }

};



#endif

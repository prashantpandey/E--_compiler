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


#ifndef CODEGEN_H
#define CODEGEN_H

#define SP_REG "R999"
#define BP_REG "R998"
#define RETI_REG "R997"
#define RETF_REG "F997"
#define GLOBALI_REG "R996"
#define GLOBALF_REG "F996"

#define INT_REG_AVAIL 

class RegMgr {

    private:

	static bool *iReg, *fReg;
	RegMgr();

    public:
	static RegMgr& getInstance() {
	    static RegMgr instance; // Guaranteed to be destroyed.
	    return instance;
	}

	string fetchNextAvailReg(bool isInt);
	void purgeReg(string regName);

};

RegMgr *regMgr = &RegMgr::getInstance();

#endif

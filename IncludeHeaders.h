/*
 * =====================================================================================
 *
 *       Filename:  includeHeaders.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  05/10/2014 02:36:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */


#ifndef INCLUDE_HEADERS_H
#define INCLUDE_HEADERS_H

#include "SymTab.h"
#include "SymTabEntry.h"

class IntrCodeElem {
    public:
	enum class ElemType {
	    VAR_TYPE, TEMP_VAR_TYPE, VAL_TYPE,
	    INV_NODE_TYPE, REF_EXPR_TYPE, QUAD_TYPE,
	    LABEL_TYPE, REG_TYPE, PARAM_TYPE
	};

	IntrCodeElem(const ProgramElem *elem, ElemType type) {
	    progElem_ = elem;
	    type_ = type;
	};

	const ProgramElem* getElem() const { return progElem_; };
	ElemType getType() { return type_; };

    private:
	ElemType type_;
	const ProgramElem* progElem_;
};

#include "Ast.h"
#include "CodeGen.h"
#include "STEClasses.h"
#include "RegMgr.h"

#endif

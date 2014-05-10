#ifndef STE_CLASSES_H
#define STE_CLASSES_H

#include "SymTab.h"
#include "SymTabEntry.h"
#include "Ast.h"
#include "CodeGen.h"

class StmtNode;
class RuleNode;
class ExprNode;
class PatNode;
class VariableEntry;
class OpNode;
class CompoundStmtNode;
class PrimitivePatNode;

class ProgCode;
class Instruction;
class Quadruple;

extern string newName(const string&);

/****************************************************************
  The first few classes dont really add any functionality over
  the base class, except for providing a convenient constructor.
****************************************************************/

class GlobalEntry: public SymTabEntry {
public:
    GlobalEntry(string name, int line=0, int column=0, string file=""):
        SymTabEntry(name, SymTabEntry::Kind::GLOBAL_KIND, line, column,file), rules_() { globalCntI_ = 0; globalCntF_ = 0; };
    ~GlobalEntry() {};

    void checkType() const;
    void genFinalCode(string progName);
    void serializeFinalCode() const;
    void serializeAsm(ostream& fos) const;

    const vector<RuleNode*> rules() const {
        return rules_;
    };
    vector<RuleNode*> rules() {
        return rules_;
    };
    const RuleNode* rule(int i) const {
        return rules_[i];
    }
    RuleNode* rule(int i) {
        return rules_[i];
    }
    void addRule(RuleNode* re) {
        rules_.push_back(re);
    };

    int getCntI() { return globalCntI_; };
    int getCntF() { return globalCntF_; };

    void incrCntI() { globalCntI_++; };
    void incrCntF() { globalCntF_++; };

    void print(ostream&, int indent=0) const;

private:
    int globalCntI_;
    int globalCntF_;
    vector<RuleNode*> rules_;
    ProgCode* progCode_;
};

class BlockEntry: public SymTabEntry {
public:
    BlockEntry(string name, int line=0, int column=0, string file=""):
        SymTabEntry(name, SymTabEntry::Kind::BLOCK_KIND, line, column, file, (Type*)&Type::voidType) {};
    ~BlockEntry() {};
    void print(ostream& out, int indent=0) const;

};

class RuleBlockEntry: public BlockEntry {
public:
    RuleBlockEntry(int line=0, int column=0, string file=""):
        BlockEntry(newName("rule"), line,column, file) {
        kind(SymTabEntry::Kind::RULE_BLOCK_KIND);
    };
    ~RuleBlockEntry() {};
};

class WhileBlockEntry: public BlockEntry {
public:
    WhileBlockEntry(vector<int> wl, int line=0, int column=0, string file=""):
        BlockEntry(newName("while"), line,column, file) {
        kind(SymTabEntry::Kind::WHILE_BLOCK_KIND);
        whileLabel_ = wl;
    };
    ~WhileBlockEntry() {};
    void print(ostream& out, int indent=0) const;

    void insertWhileLabel(int wl) {
        whileLabel_.push_back(wl);
    };
    
    /*
    void setWhileLabel(vector<int> wl) {
        whileLabel_ = wl;
    };
    */

    vector<int> getWhileLabel() { return whileLabel_; };

    string returnStringLabel() const {
        string label;
        unsigned int i = 0;
	label.append("while_");
        for(i = 0; i <= whileLabel_.size(); i++) {
            label.append(to_string(whileLabel_[i]));
	    label.append("_");
        }
        return label;
    }

    int nestedWhileCount() const {
        return whileLabel_.size();
    };
    
    vector<int> copyWhileLabel() const {
        vector<int> copiedWhileLabel(whileLabel_);
        return copiedWhileLabel;
    };

private:
    vector<int> whileLabel_;
};

/****************************************************************
  Following classes add more significant functionality to that
  provided by SymTabEntry.
****************************************************************/

class VariableEntry: public SymTabEntry {
public:
    enum VarKind {GLOBAL_VAR, LOCAL_VAR, PARAM_VAR, UNDEFINED};

public:
    VariableEntry(string name, VarKind v, Type* type=nullptr,
                  ExprNode* init=nullptr, int ln=0, int col=0, string file=""):
        SymTabEntry(name, SymTabEntry::Kind::VARIABLE_KIND, ln, col, file, type) {
        vkind_ = v;
        initVal(init);
        const_ = false;
	inMem_ = false;
	regName_ = "";
	temp_ = false;
	iCodeTable_ = new vector<Quadruple*>();
    };

    VariableEntry(const VariableEntry &v);
    ~VariableEntry() {};

    VarKind varKind() const {
        return vkind_;
    };
    void varKind(VarKind v) {
        vkind_ = v;
    };

    int offSet() const {
        return offSet_;
    } ;
    void offSet(int o) {
        offSet_ = o;
    };

    void checkType() const;

    vector<Instruction*>* codeGen();

    const ExprNode* initVal() const {
        return initVal_;
    }
    
    vector<Instruction*>* fetchExprRegValue();

    ExprNode* initVal() {
        return initVal_;
    };

    /* Adding this field to mark variable as constant. */
    void setConst(bool isConst) {
        const_ = isConst;
    }
    bool isConst() const {
        return const_;
    };

    void initVal(ExprNode *init) {
        initVal_ = init;
    };

    void insertQuadrupleSet(vector<Quadruple *> *instrVector) {
	if (instrVector != NULL)
	    iCodeTable_->insert(iCodeTable_->end(), instrVector->begin(), instrVector->end());
    }

    void setMem (bool inMem) { inMem_ = inMem; };
    void setMem() { setMem(true); };
    bool isMem() { return inMem_; };

    bool isTemp() { return temp_; }
    void setTemp() { temp_ = true; }

    void setReg(string regName) { regName_ = regName; };
    string getReg() { return regName_; };
    
    void setTReg(string regName) { tReg_ = regName; };
    string getTReg() { return tReg_; };

    void print(ostream& os, int indent=0) const;

private:
    VarKind vkind_;
    int offSet_;
    bool const_;
    ExprNode* initVal_;
    bool inMem_;
    string regName_;
    string tReg_;
    bool temp_;
    vector<Quadruple*>* iCodeTable_;
};

class ClassEntry: public SymTabEntry {
    public:
	ClassEntry(string name, int line=0, int column=0, string file="")
	    : SymTabEntry(name, SymTabEntry::Kind::CLASS_KIND, line,column, file) {};
	~ClassEntry() {};

	void print(ostream& os, int indent) const;
};

class FunctionEntry: public SymTabEntry {
    public:
	FunctionEntry(string name, Type* type=nullptr,
		int line=0, int column=0, string file=""):
	    SymTabEntry(name, SymTabEntry::Kind::FUNCTION_KIND, line,column, file, type) {
		body_ = nullptr;
		argCnt_ = 0;
	    };
	~FunctionEntry() {};

	void checkType() const;

	vector<Instruction*>* codeGen();

	const CompoundStmtNode* body() const {
	    return body_;
	};
	CompoundStmtNode* body() {
	    return body_;
	};
	void body(CompoundStmtNode* n) {
	    body_ = n;
	};

	int getArgCnt() const {
	    return argCnt_;
	}
	
        string getALabel() const {
	    return aLabel_;
	}

	void incrementArgCnt(int offset) {
	    argCnt_ += offset;
	}

	void print(ostream& os, int indent) const;

    private:
	CompoundStmtNode* body_;
	int argCnt_;
        string aLabel_;
};

class EventEntry: public SymTabEntry {
    public:
	EventEntry(string name, int line=0, int column=0, string file=""):
	    SymTabEntry(name, SymTabEntry::Kind::EVENT_KIND, line,column, file) {};

	EventEntry(string name, Kind kind, int line=0, int column=0, string file=""):
	    SymTabEntry(name, kind, line,column, file) {};

	~EventEntry() {};

	int getArgCnt() const {
	    return argCnt_;
	}
	void incrementArgCnt(int offset) {
	    argCnt_ += offset;
	}

	void print(ostream& out, int indent=0) const;

        vector<Instruction*>* codeGen(vector<VariableEntry*>* params);

    private:
	int argCnt_;
};

class UnknownKindEntry: public SymTabEntry {
    public:
	UnknownKindEntry(string name, int line=0, int column=0, string file=""):
	    SymTabEntry(name, SymTabEntry::Kind::UNKNOWN_KIND, line,column, file) {};

	UnknownKindEntry(string name, Kind kind, int line=0, int column=0, string file=""):
	    SymTabEntry(name, kind, line,column, file) {};

	~UnknownKindEntry() {};

	void print(ostream& out, int indent=0) const;
};
#endif

#ifndef AST_H
#define AST_H

#include "all.h"
#include "IncludeHeaders.h"
#include "Value.h"
#include "ProgramElem.h"

class BlockEntry;
class EFSA;
class EventEntry;
class FunctionEntry;
class LabelEntry;
class OpNode;
//enum class OpCode;
class PatNode;
//enum class PatNode;
class PrimitivePatNode;
class RulezNode;
class SymTabEntry;
class VariableEntry;

class Instruction;
class Quadruple;
class IntrCodeElem;

/*****************************************************************************
   Here is the class hierarchy:
                                               ProgramElem
					           |
                                                AstNode
     +--------------------------------------------+----------------+
     |		         |             |                           |
 BasePatNode      ExprNode          RuleNode                    StmtNode
     |               |                                             |
     |               |                                             |
     |      +---------+----------+-----------+                     |
     |      |         |          |           |                     |
     |  RefExprNode  OpNode  ValueNode  InvocationNode             |
     |                                                             |
     |                                      +---------------+------+-----+------------+-------------+--------------+
     |                                      |               |            |	      |	            |		   |
     |                                 ExprStmtNode   CompoundStmtNode IfNode	  WhileNode    ReturnStmtNode  BreakStmtNode
     |
     |
   +-+------------------+
   |                    |
   |                    |
PrimitivePatNode    PatNode

******************************************************************************/

class AstNode: public ProgramElem {
public:
    enum class NodeType  {
        PAT_NODE,
        EXPR_NODE,
        REACTION_NODE,
        STMT_NODE,
        SEQUENCE_NODE,
        RULE_NODE
    };

public:
    AstNode(NodeType nt, int line=0, int column=0, string file="");
    AstNode(const AstNode&); // copy constructor
    virtual ~AstNode() {};
    //virtual AstNode* clone() = 0;

    NodeType nodeType() const {
        return nodeType_;
    }

    virtual const Type* typeCheck() {
        return NULL;
    };
    virtual void print(ostream& os, int indent=0) const=0;
    //virtual EFSA* codeGen() {
    virtual vector<Instruction*> *codeGen() {
        return new vector<Instruction*>();
    };

    virtual vector<Quadruple*> *iCodeGen() {
        return new vector<Quadruple*>();
    };

    virtual void renameRV(string prefix) {}; // new names start with given prefix
    virtual bool operator==(const AstNode&) const {
        return false;
    };
    virtual bool operator!=(const AstNode& a) const
    {
        return !operator==(a);
    };

private:
    NodeType nodeType_;
    const AstNode* operator=(const AstNode& other); /* disable asg */
};

inline ostream& operator<<(ostream& os, const AstNode& an) {
    an.print(os);
    return os;
};

/****************************************************************/
class ExprNode: public AstNode {
public:
    enum class ExprNodeType   {
        REF_EXPR_NODE,
        OP_NODE,
        VALUE_NODE,
        INV_NODE
    };

public:
    ExprNode(ExprNodeType et, const Value* val=0, int line=0, int column=0,
             string file=""); // val is saved, but not copied
    ExprNode(const ExprNode&);
    virtual ~ExprNode() {};

    virtual ExprNode* clone() const=0;

    ExprNodeType exprNodeType() const {
        return exprType_;
    };
    void exprNodeType(ExprNodeType t) {
        exprType_ = t;
    };

    const Type* doTypeCheck() {
        const Type *type = typeCheck();
        setResultType(type);
        return type;
    }

    const Value* value() const {
        return val_;
    }

    const Type* coercedType() const {
        return coercedType_;
    }

    void coercedType(const Type* type) {
        coercedType_ = type;
    }

    void setResultType(const Type* type) {
        resultType_ = type;
    }

    void setTVar(IntrCodeElem *var) {
        tVar_ = var;
    };

    IntrCodeElem* getTVar() {
        return tVar_;
    };

    Type* getResultType() {
        const Type* temp = coercedType_?coercedType_:resultType_;
        return new Type(temp->tag());
    }

    virtual vector<Quadruple*>* iCodeGen() = 0;
    
    void print(ostream& os, int indent=0) const=0;

private:
    virtual const Type* typeCheck() const = 0;
    ExprNodeType exprType_;
    const Value *val_; // reference semantics for val_ and coercedType_
    const Type* coercedType_;
    const Type* resultType_;
    IntrCodeElem *tVar_;
};

/****************************************************************/
class RefExprNode: public ExprNode {
public:
    RefExprNode(string ext, SymTabEntry* ste=NULL,
                int line=0, int column=0, string file="");
    RefExprNode(const RefExprNode&);
    ExprNode* clone() const {
        return new RefExprNode(*this);
    }

    ~RefExprNode() {};

    string ext() const {
        return ext_;
    };
    void ext(string str) {
        ext_ = str;
    };

    SymTabEntry* symTabEntry() const {
        return sym_;
    };
    void symTabEntry(SymTabEntry *ste)  {
        sym_ = ste;
    };

    vector<Quadruple*>* iCodeGen();

    void print(ostream& os, int indent=0) const;

private:
    const Type* typeCheck() const;
    string ext_;
    SymTabEntry* sym_;
};

/****************************************************************/

#define MAX_OP_ARITY 2

class OpNode: public ExprNode {
public:

    enum class OpCode {
        UMINUS, PLUS, MINUS, MULT, DIV, MOD,
        EQ, NE, GT, LT, GE, LE,
        AND, OR, NOT,
        BITNOT, BITAND, BITOR, BITXOR, SHL, SHR,
        ASSIGN, PRINT, INVALID,
        JMP, JMPC, CALL, RET, MOVIF, IN,
        DEFAULT
    };

    enum class OpPrintType {
        PREFIX, INFIX, POSTFIX
    };
    struct OpInfo {
        OpCode code_;
        const char* name_;
        int arity_;
        int needParen_;
        OpPrintType prtType_;
        Type::TypeTag argType_[3];
        // operators with > 2 args can be supported
        // only if types of args k through N are identical, for 1 <= k <= 3,
        // and are given by argType[k-1]

        Type::TypeTag outType_;
        const char *typeConstraints_;
    };

public:
    static const int VARIABLE = 100;
public:
    OpNode(OpCode op, ExprNode *l, ExprNode *r=NULL,
           int line=0, int column=0, string file="");
    OpNode(const OpNode&);
    ExprNode* clone() const {
        return new OpNode(*this);
    }
    ~OpNode() {};

    OpCode opCode() const {
        return opCode_;
    };
    const ExprNode* arg(unsigned int i) const
    {
        return (i < arg_.size())? arg_[i] : NULL;
    };
    unsigned int arity() const {
        return arity_;
    }

    void opCode(OpCode a) {
        opCode_ = a;
    };
    ExprNode* arg(unsigned int i)
    {
        return (i < arg_.size())? arg_[i] : NULL;
    };
    vector<ExprNode*>* args()
    {
        return &arg_;
    }

    void print(ostream& os, int indent=0) const;

    vector<Quadruple*>* iCodeGen();

    vector<Quadruple*>* iCodeGen(string trueLabel, string falseLabel, int flag);

    static OpCode negOpCode(OpCode opc);


private:
    const Type* typeCheck() const;
    unsigned int arity_;
    OpCode   opCode_;
    vector<ExprNode*> arg_;
};

/****************************************************************/

class ValueNode: public ExprNode {
public:
    ValueNode(Value* val=0, int line=0, int column=0, string file="")
        : ExprNode(ExprNode::ExprNodeType::VALUE_NODE, val, line,column,file) {
        if (val != NULL) type((Type*)(val->type()));
    }
    ValueNode(const ValueNode& val): ExprNode(val) {};
    ExprNode* clone() const {
        return new ValueNode(*this);
    }
    ~ValueNode() {};

    vector<Quadruple*>* iCodeGen();

    void print(ostream& os, int indent=0) const;

private:
    const Type* typeCheck() const;
    /* val_ field is already included in ExprNode, so no new data members */
};

/****************************************************************/

class InvocationNode: public ExprNode {
    // Used to represent function invocation
public:
    InvocationNode(const SymTabEntry *ste, vector<ExprNode*>* param=0,
                   int line=0, int column=0, string file="");
    InvocationNode(const InvocationNode&);
    ExprNode* clone() const  {
        return new InvocationNode(*this);
    }
    ~InvocationNode() {};

    const SymTabEntry* symTabEntry() const {
        return ste_;
    };

    const vector<ExprNode*>* params() const {
        return params_;
    };
    vector<ExprNode*>* params() {
        return params_;
    }
    void params(vector<ExprNode*>* args) {
        params_ = args;
    };
    const ExprNode* param(unsigned int i) const
    {
        return (params_ != NULL && i < params_->size())? (const ExprNode*)((*params_)[i]) : NULL;
    };
    ExprNode* param(unsigned int i)
    {
        return (ExprNode*)((const InvocationNode*)this->param(i));
    }
    void param(ExprNode* arg, unsigned int i)
    {
        if (params_ != NULL && i < params_->size()) (*params_)[i] = arg;
    };

    vector<Quadruple*>* iCodeGen();

    vector<Instruction*>* codeGen();

    void print(ostream& os, int indent=0) const;

private:
    const Type* typeCheck() const;
    vector<ExprNode*>* params_;
    const SymTabEntry *ste_; // reference semantics
};


/****************************************************************/
// There are 3 kinds of PatNodes:
//   PrimitivePatNodes are of the form: event|cond
//   AtomicPatNodes consist of PrimitivePatNodes with one or more "||"
//      operators, plus an optional negation symbol
//   PatNodes consist of PatNodes or AtomicPatNodes composed with
//      ".", "||" and "*" operators
// We have a single base class for pattern nodes called BasePatNode. In
// addition, the functionality of Atomic and PatNodes have been
// combined into a single class PatNode.

class BasePatNode: public AstNode {
public:
    enum class PatNodeKind {
        PRIMITIVE, EMPTY, NEG, SEQ, OR, STAR, UNDEFINED
    };

    BasePatNode(PatNodeKind pk, int ln=0, int col=0, string f=""):
        AstNode(AstNode::NodeType::PAT_NODE, ln, col, f) {
        parent_ = NULL;
        root_ = NULL;
        patKind_ = pk;
    };
    BasePatNode(const BasePatNode& bpn): AstNode(bpn) {
        patKind_ = bpn.patKind_;
        parent_ = bpn.parent_;
        root_ = bpn.root_;
    }
    ~BasePatNode() {};
    //virtual BasepatNode* clone() const { return new BasePatNode(*this);}

    PatNodeKind kind() const {
        return patKind_;
    };
    void kind(PatNodeKind p) {
        patKind_ = p;
    }

    virtual const Type* typeCheck() const = 0;

    const BasePatNode* parent() const {
        return parent_;
    }

    BasePatNode* parent() {
        return parent_;
    }

    virtual string getLabel() = 0;

    virtual vector<Quadruple*>* iCodeGen() = 0;
    virtual bool hasSeqOps() const=0;
    virtual bool hasNeg() const=0;
    virtual bool hasAnyOrOther() const=0;
    virtual bool isNegatable() const {
        return ((!hasSeqOps()) && (!hasNeg()));
    }

    const static string labelPrefix;

private:
    PatNodeKind patKind_;
    BasePatNode* parent_;
    BasePatNode* root_;
};

/****************************************************************/

class PrimitivePatNode: public BasePatNode {
public:
    PrimitivePatNode(EventEntry* ee, vector<VariableEntry*>* params,
                     ExprNode* c=NULL,
                     int line=0, int column=0, string file="");
    //PrimitivePatNode(const PrimitivePatNode& ppn);
    ~PrimitivePatNode() {};
    //BasePatNode* clone() { return new PrimitivePatNode(*this); }

    const EventEntry* event() const {
        return ee_;
    }

    EventEntry* event() {
        return ee_;
    }

    const vector<const VariableEntry*>* params() const {
        return (vector<const VariableEntry*>*)params_;
    }
    vector<VariableEntry*>* params() {
        return params_;
    }

    virtual vector<Quadruple*>* iCodeGen();

    const ExprNode* cond() const {
        return cond_;
    }
    ExprNode* cond() {
        return cond_;
    }
    void cond(ExprNode* c) {
        cond_ = c;
    }

    ExprNode* condition() {
        return condition_;
    }
    const ExprNode* condition() const {
        return condition_;
    }

    const list<const OpNode*>& asgs() const {
        return (list<const OpNode*>&)asgs_;
    }
    list<OpNode*>& asgs() {
        return asgs_;
    }

    const Type* typeCheck() const;
    bool hasSeqOps() const;
    bool hasNeg() const;
    bool hasAnyOrOther() const;

    string getLabel();

    //-const Type* typeCheck();
    void print(ostream& os, int indent=0) const;

private:

    EventEntry* ee_;
    vector<VariableEntry*>* params_;
    /* cond_ may contain assignments as well as other expressions */
    /* condition_ contains all expresions in cond_ other than assignments */
    ExprNode* cond_;
    ExprNode* condition_;
    list<OpNode*> asgs_;
};

/****************************************************************/
class PatNode: public BasePatNode {
public:
    PatNode(int line=0, int column=0, string file="");
    PatNode(PatNodeKind pk, BasePatNode *p1, BasePatNode*p2=NULL, int line=0, int column=0, string file="");

    ~PatNode() {};
    //AstNode* clone()
    //  { return new PatNode(*this); }

    const BasePatNode* pat1() const {
        return pat1_;
    }
    BasePatNode* pat1() {
        return pat1_;
    }
    const BasePatNode* pat2() const {
        return pat2_;
    }
    BasePatNode* pat2() {
        return pat2_;
    }

    virtual vector<Quadruple*>* iCodeGen();

    bool hasNeg() const;
    bool hasSeqOps() const;
    bool hasAnyOrOther() const;

    void print(ostream& os, int indent=0) const;
    const Type* typeCheck() const;

    string getLabel() {
        return pat1_->getLabel();
    };
private:
    PatNode(const PatNode&);

    BasePatNode *pat1_;
    BasePatNode *pat2_;
};


/****************************************************************/

class StmtNode: public AstNode {
public:
    enum class StmtNodeKind {
        ILLEGAL=-1, EXPR, IF, COMPOUND, RETURN, WHILE, BREAK, PRINT
    };
public:
    StmtNode(StmtNodeKind skm, int line=0, int column=0, string file=""):
        AstNode(AstNode::NodeType::STMT_NODE, line,column,file) {
        skind_ = skm;
        iCodeTable_ = new vector<Quadruple*>();
    };
    ~StmtNode() {};
    //AstNode* clone()
    //  { return new StmtNode(*this); }

    StmtNodeKind stmtNodeKind() const {
        return skind_;
    }

    virtual const Type* typeCheck() const = 0;

    vector<Instruction*>* fetchExprRegValue(ExprNode* expr);
    virtual vector<Instruction*>* codeGen() {
        return new vector<Instruction*>();
    };

    vector<Quadruple*>* getICodeTable() {
        return iCodeTable_;
    };

    void insertQuadrupleSet(vector<Quadruple *> *instrVector) {
        if (instrVector != NULL)
            mergeVec(iCodeTable_, instrVector);
    }

    void print(ostream& os, int indent) const = 0;
private:
    StmtNodeKind skind_;
    vector<Quadruple*>* iCodeTable_;
};

/****************************************************************/

class ReturnStmtNode: public StmtNode {
public:
    ReturnStmtNode(ExprNode *e, FunctionEntry* fe,
                   int line=0, int column=0, string file=""):
        StmtNode(StmtNode::StmtNodeKind::RETURN,line,column,file) {
        expr_ = e;
        fun_ = fe;
    };
    ~ReturnStmtNode() {};

    const ExprNode* exprNode() const {
        return expr_;
    }
    const FunctionEntry* funEntry() const {
        return fun_;
    }

    ExprNode* exprNode() {
        return expr_;
    }
    FunctionEntry* funEntry() {
        return fun_;
    }

    const Type* typeCheck() const;

    vector<Quadruple*>* iCodeGen();

    void print(ostream& os, int indent) const {
        os << "return ";
        if(expr_ != NULL) expr_->print(os, indent);
        else os << "NULL";
    };

private:
    ExprNode* expr_;
    FunctionEntry* fun_;
};

/****************************************************************/

class BreakStmtNode: public StmtNode {
public:
    BreakStmtNode(int num, BlockEntry* be,
                  int line=0, int column=0, string file=""):
        StmtNode(StmtNode::StmtNodeKind::BREAK, line, column, file) {
        num_ = num;
        blockEntry_ = be;
    };
    ~BreakStmtNode() {};

    const int num() const {
        return num_;
    }
    const BlockEntry* blockEntry() const {
        return blockEntry_;
    }

    const Type* typeCheck() const;

    vector<Quadruple*>* iCodeGen();

    int num() {
        return num_;
    }

    BlockEntry* blockEntry() {
        return blockEntry_;
    }

    void print(ostream& os, int indent) const {
        os << "break " << num_;
    }

private:
    int num_;
    string brkNumLabel_;
    BlockEntry *blockEntry_;
};

/****************************************************************/

class ExprStmtNode: public StmtNode {
public:
    ExprStmtNode(ExprNode* e, int line=0, int column=0, string file=""):
        StmtNode(StmtNode::StmtNodeKind::EXPR,line,column,file) {
        expr_ = e;
    };
    ~ExprStmtNode() {};
    //AstNode* clone()
    //  { return new ExprStmtNode(*this); }

    const ExprNode* exprNode() const {
        return expr_;
    }
    ExprNode* exprNode() {
        return expr_;
    }

    const Type* typeCheck() const;

    vector<Quadruple*>* iCodeGen() {
        return expr_->iCodeGen();
    }

    void print(ostream& os, int indent) const {
        if (expr_ != NULL) {
            expr_->print(os, indent);
        }
    };

private:
    ExprNode* expr_;
};

/****************************************************************/

class CompoundStmtNode: public StmtNode {
public:
    CompoundStmtNode(list<StmtNode*> *Slist, int ln=0, int col=0, string fl=""):
        StmtNode(StmtNode::StmtNodeKind::COMPOUND, ln,col,fl) {
        stmts_ = Slist;
    };
    ~CompoundStmtNode() { };
    //AstNode* clone()
    //  { return new CompoundStmtNode(*this); }

    const list<StmtNode*>* stmts() const {
        return stmts_;
    }

    const Type* typeCheck() const;

    vector<Instruction*>* codeGen();

    list<StmtNode*>* stmts() {
        return stmts_;
    }

    vector<Quadruple*>* iCodeGen() {
        vector<Quadruple*>* quad = new vector<Quadruple*>();
        for(list<StmtNode*>::iterator it = stmts_->begin(); it != stmts_->end(); ++it) {
            if ((*it))
		mergeVec(quad, (*it)->iCodeGen());
        }
        /*
        for(StmtNode* stmt : stmts_) {
            mergeVec(quad, stmt->iCodeGen());
        }
        */
        return quad;
    };

    void add(StmtNode *s)
    {
        if(stmts_ != NULL) stmts_->push_back(s);
    };

    void  printWithoutBraces(ostream& os, int indent) const;
    void  print(ostream& os, int indent) const;

private:
    CompoundStmtNode(const CompoundStmtNode&);

    list<StmtNode*>   *stmts_;
};

/****************************************************************/

class IfNode: public StmtNode {
public:

    IfNode(ExprNode* cond, StmtNode* thenStmt,
           StmtNode* elseStmt=NULL, int line=0, int column=0, string file="");
    ~IfNode() {};
    //AstNode* clone()
    //  { return new IfNode(*this); }

    const ExprNode* cond() const {
        return cond_;
    }
    const StmtNode* elseStmt() const {
        return else_;
    };
    const StmtNode* thenStmt() const  {
        return then_;
    };

    const Type* typeCheck() const;
    ExprNode* cond() {
        return cond_;
    }
    StmtNode* elseStmt() {
        return else_;
    };
    StmtNode* thenStmt() {
        return then_;
    };

    vector<Quadruple*>* iCodeGen();

    void print(ostream& os, int indent) const;

private:
    ExprNode *cond_;
    StmtNode *then_, *else_;
    string endLabel_;
    string elseLabel_;
    string startLabel_;

    IfNode(const IfNode&);
};

/****************************************************************/

class WhileNode: public StmtNode {
public:

    WhileNode(ExprNode* cond, StmtNode* compStmt, string key,
              int line=0, int column=0, string file="");

    ~WhileNode() {};

    const ExprNode* cond() const {
        return cond_;
    }
    const StmtNode* compStmt() const {
        return comp_;
    };

    const Type* typeCheck() const;

    ExprNode* cond() {
        return cond_;
    }
    StmtNode* compStmt() {
        return comp_;
    };

    vector<Quadruple*>* iCodeGen();

    void print(ostream& os, int indent) const;

private:
    ExprNode *cond_;
    StmtNode *comp_;
    string startLabel_;
    string beginLabel_;
    string endLabel_;
    string key_;

    WhileNode(const WhileNode&);
};

/****************************************************************/

class PrtNode : public StmtNode {

public:
    PrtNode(string prt, RefExprNode *refExpr, int ln = 0, int col = 0, string fl = "" ): 
    StmtNode(StmtNode::StmtNodeKind::PRINT, ln,col,fl) {
	prt_ = prt;
	refExpr_ = refExpr;
    };
    
    const Type* typeCheck() const;

    void print(ostream& os, int indent) const;
    
    vector<Quadruple*>* iCodeGen();

    RefExprNode *getRefExpr() { return refExpr_; };
    string getPrt() { return prt_; };
    
private:
    RefExprNode *refExpr_;
    string prt_;
};

/****************************************************************/

class RuleNode: public AstNode {
public:
    RuleNode(BlockEntry *re, BasePatNode* pat, StmtNode* reaction,
             int line=0, int column=0, string file="");
    ~RuleNode() {};
    //AstNode* clone()
    //  { return new RuleNode(*this); }

    const BlockEntry* ruleEntry() const {
        return rste_;
    };
    BlockEntry* ruleEntry() {
        return rste_;
    };

    const BasePatNode* pat() const {
        return pat_;
    };
    BasePatNode* pat() {
        return pat_;
    };

    const StmtNode* reaction() const {
        return reaction_;
    };
    StmtNode* reaction() {
        return reaction_;
    };

    string getJmpName() {
        return jmpName_;
    }

    void printICode();

    const Type* typeCheck() const;

    void print(ostream& os, int indent=0) const;

    vector<Instruction*>* codeGen();

private:
    BlockEntry    *rste_;
    BasePatNode *pat_;
    StmtNode *reaction_;
    string jmpName_;

    RuleNode(const RuleNode&);
    vector<Quadruple*>* iCodeTable_;
};

/****************************************************************/
#endif

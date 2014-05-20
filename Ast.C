#include "IncludeHeaders.h"
#include "ParserUtil.h"


const string BasePatNode:: labelPrefix = "eventLabel_";

AstNode::AstNode(NodeType nt, int line, int column, string file):
    ProgramElem(NULL, line, column, file) {
    nodeType_ = nt;
}

AstNode::AstNode(const AstNode& ast): ProgramElem(ast) {
    nodeType_ = ast.nodeType();
}

/****************************************************************/

bool stmtNoSemicolonAtEnd(const StmtNode *st)
{
    return (!st || (st)->stmtNodeKind() == StmtNode::StmtNodeKind::EXPR || (st)->stmtNodeKind() == StmtNode::StmtNodeKind::RETURN);
}

ExprNode::ExprNode(ExprNodeType et, const Value* val, int line, int column,
                   string file):
    AstNode(AstNode::NodeType::EXPR_NODE, line, column, file)
{
    exprType_ = et;
    val_ = val;
    coercedType_ = nullptr;
    tVar_ = NULL;
}


ExprNode::ExprNode(const ExprNode& e) : AstNode(e)
{
    ExprNode::exprType_ = e.exprNodeType();
    ExprNode::val_ = new Value(*e.value());
    ExprNode::coercedType_ = e.coercedType();
}


RefExprNode::RefExprNode(const RefExprNode& re) : ExprNode(re)
{
    ext_ = re.ext();
    sym_ = re.symTabEntry();
}

/* Added by KA */
const Type* RefExprNode::typeCheck() const
{
    const SymTabEntry *ste = symTabEntry();
    if (ste != nullptr && ste->kind() != SymTabEntry::Kind::VARIABLE_KIND)
    {
        errMsg(ext() + " Not of variable kind", this);
        return &Type::errorType;
    }
    else if (ste != nullptr && ((VariableEntry*)(ste))->varKind() == VariableEntry::VarKind::UNDEFINED) {
        errMsg("undefined variable " + ext(), this);
        return &Type::errorType;
    }
    return ste->type();
}

RuleNode::RuleNode(BlockEntry *re, BasePatNode* pat, StmtNode* reaction, int line,
                   int column, string file):
    AstNode(AstNode::NodeType::RULE_NODE,line,column,file)
{
    rste_ = re;
    pat_ = pat;
    reaction_ = reaction;
    iCodeTable_ = new vector<Quadruple*>();
}

vector<Instruction*>* RuleNode::codeGen() {

    vector<Instruction*> *inst_vec = new vector<Instruction*>();
    inst_vec->push_back(new Instruction(pat_->getLabel()));
    jmpName_ = pat_->getLabel().substr(PrimitivePatNode::labelPrefix.length());
    iCodeTable_ = pat_->iCodeGen();
    mergeVec(iCodeTable_, reaction_->iCodeGen());
    printICode();
    mergeVec(inst_vec, Quadruple::iCodeToAsmGen(iCodeTable_));
    return inst_vec;
}

void RuleNode::print(ostream& os, int indent) const
{
    prtSpace(os, indent);
    pat_->print(os,indent);
    os << "-->  ";
    if (reaction_)
        reaction_->print(os, indent);
    os << ";";
}

void RuleNode::printICode() {
    ostringstream os;
    for(vector<Quadruple*>::iterator it = iCodeTable_->begin(); it != iCodeTable_->end(); ++it) {
        os << (*it)->toString();
    }
    cout << os.str();
}

const Type*  RuleNode::typeCheck() const {
    bool flag = false;
    if(pat()->typeCheck()->tag() == Type::TypeTag::ERROR) {
        flag = true;
    }
    if(reaction()->typeCheck()->tag() == Type::TypeTag::ERROR) {
        flag = true;
    }

    if(flag) {
        return &Type::errorType;
    }
    else {
        return &Type::unkType;
    }
}

WhileNode::WhileNode(ExprNode* cond, StmtNode* compStmt, string key,
                     int line, int column, string file):
    StmtNode(StmtNode::StmtNodeKind::WHILE, line, column, file)
{
    cond_ = cond;
    comp_ = compStmt;
    key_ = key;
}

void WhileNode::print(ostream& os, int indent) const
{
    os << "while (";
    cond()->print(os, indent);
    os << ")";
    if(compStmt()) {
        compStmt()->print(os, indent);
    }
    if(!compStmt() || compStmt()->stmtNodeKind() != StmtNode::StmtNodeKind::COMPOUND) {
        endln(os, indent);
    }
}

const Type* WhileNode::typeCheck() const {
    bool flag = true;
    const Type *cond_type = cond_->doTypeCheck();
    if (cond_type->tag() != Type::TypeTag::BOOL)
    {
        flag = false;
        if(cond_type->tag() != Type::TypeTag::ERROR)
            errMsg("Boolean argument expected", this);
    }
    if (compStmt() != NULL && compStmt()->typeCheck()->tag() != Type::TypeTag::VOID) {
        flag = false;
    }

    if(flag) {
        return &Type::voidType;
    }
    return &Type::errorType;
}

// WhileNode is handled in three parts
// 1. The conditional expression
// 2. The loop body
// 3. The jump/break statement
vector<Quadruple*>* WhileNode::iCodeGen() {
    vector<Quadruple*>* inst_vec = new vector<Quadruple*>();

    startLabel_ = key_ + "_start";
    endLabel_ = key_ + "_end";

    if(cond_ != NULL) {
        mergeVec(inst_vec, cond_->iCodeGen());
        inst_vec->at(0)->setLabel(startLabel_);
    }

    IntrCodeElem *endLabelTemp =  new IntrCodeElem(new IntrLabel(endLabel_), IntrCodeElem::ElemType::LABEL_TYPE);
    IntrCodeElem *cmpVar = new IntrCodeElem(new ValueNode(new Value(0, Type::TypeTag::INT)), IntrCodeElem::ElemType::VAL_TYPE);
    Quadruple *tempQuad = new Quadruple(OpNode::OpCode::EQ, cmpVar, cond_->getTVar(), NULL);
    inst_vec->push_back(tempQuad);
    inst_vec->push_back(new Quadruple(OpNode::OpCode::JMPC, new IntrCodeElem(tempQuad, IntrCodeElem::ElemType::QUAD_TYPE), endLabelTemp));

    mergeVec(inst_vec, comp_->iCodeGen());

    IntrCodeElem *startLabelTemp =  new IntrCodeElem(new IntrLabel(startLabel_), IntrCodeElem::ElemType::LABEL_TYPE);
    inst_vec->push_back(new Quadruple(OpNode::OpCode::JMP, startLabelTemp));

    inst_vec->push_back(new Quadruple(OpNode::OpCode::DEFAULT, endLabelTemp));

    return inst_vec;
}

IfNode::IfNode(ExprNode* cond, StmtNode* thenStmt,
               StmtNode* elseStmt, int line, int column, string file):
    StmtNode(StmtNode::StmtNodeKind::IF, line, column, file)
{
    cond_ = cond;
    then_ = thenStmt;
    else_ = elseStmt;
}

void IfNode::print(ostream& os, int indent) const
{
    os << "if (";
    cond()->print(os, indent);
    os << ") ";
    if (thenStmt())
        thenStmt()->print(os, indent);
    if (!thenStmt() || thenStmt()->stmtNodeKind() != StmtNode::StmtNodeKind::COMPOUND)
        endln(os, indent);
    if (elseStmt()) {
        prtSpace(os, indent);
        os << "else ";
        elseStmt()->print(os, indent);
        if (!elseStmt() || elseStmt()->stmtNodeKind() != StmtNode::StmtNodeKind::COMPOUND)
            endln(os, indent);
    }
}

/* Added by KA */
const Type* IfNode::typeCheck() const
{
    bool flag = true;
    const Type *cond_type = cond_->doTypeCheck();
    if (cond_type->tag() != Type::TypeTag::BOOL)
    {
        flag = false;
        if(cond_type->tag() != Type::TypeTag::ERROR)
            errMsg("Boolean argument expected", this);
    }
    if(elseStmt() != NULL && elseStmt()->typeCheck()->tag() != Type::TypeTag::VOID) {
        flag = false;
    }
    if(thenStmt() != NULL && thenStmt()->typeCheck()->tag() != Type::TypeTag::VOID) {
        flag = false;
    }

    if(flag) {
        return &Type::voidType;
    }
    return &Type::errorType;
}

vector<Quadruple*>* IfNode::iCodeGen() {
    vector<Quadruple*>* inst_vec = new vector<Quadruple*>();

    elseLabel_ = regMgr->getNextLabel();
    endLabel_ = regMgr->getNextLabel();
    IntrCodeElem *endLabelTemp =  new IntrCodeElem(new IntrLabel(endLabel_), IntrCodeElem::ElemType::LABEL_TYPE);
    IntrCodeElem *elseLabelTemp =  new IntrCodeElem(new IntrLabel(elseLabel_), IntrCodeElem::ElemType::LABEL_TYPE);

    if(cond_ != NULL) {
        mergeVec(inst_vec, cond_->iCodeGen());
    }

    // TODO: implement short circuit of expressions
    IntrCodeElem *cmpVar = new IntrCodeElem(new ValueNode(new Value(0, Type::TypeTag::INT)), IntrCodeElem::ElemType::VAL_TYPE);
    Quadruple *tempQuad = new Quadruple(OpNode::OpCode::EQ, cmpVar, cond_->getTVar(), NULL);
    inst_vec->push_back(new Quadruple(OpNode::OpCode::JMPC, new IntrCodeElem(tempQuad, IntrCodeElem::ElemType::QUAD_TYPE), elseStmt() ? elseLabelTemp : endLabelTemp));

    mergeVec(inst_vec, then_->iCodeGen());

    if(elseStmt() != NULL) {
        inst_vec->push_back(new Quadruple(OpNode::OpCode::JMP, endLabelTemp));
        vector<Quadruple*>* stmtLst = else_->iCodeGen();
        stmtLst->at(0)->setLabel(elseLabel_);
        mergeVec(inst_vec, stmtLst);
    }

    inst_vec->push_back(new Quadruple(OpNode::OpCode::DEFAULT, endLabelTemp));

    return inst_vec;
}

PrimitivePatNode::PrimitivePatNode(EventEntry* ee, vector<VariableEntry*>* params,
                                   ExprNode* c,
                                   int line, int column, string file):
    BasePatNode(BasePatNode::PatNodeKind::PRIMITIVE, line, column, file)
{

    ee_ = ee;
    params_ = params;
    cond_ = c;

}

string PrimitivePatNode::getLabel() {
    return labelPrefix + ee_->name();
}

// TODO: Add const to the param var
// copy the type from the param vars from the event declaration
const Type* PrimitivePatNode::typeCheck() const {

    int event_error_flag = 0;
    int cond_error_flag = 0;
    const EventEntry *ee = event();

    if(ee != NULL && ee->kind() == SymTabEntry::Kind::UNKNOWN_KIND) {
        errMsg("Undefined reference to " + ee->name(), this);
        return &Type::errorType;
    }

    else if (ee != NULL && ee->kind() != SymTabEntry::Kind::UNKNOWN_KIND && ee->kind() != SymTabEntry::Kind::EVENT_KIND) {
        errMsg(ee->name() + " was not declared in the current scope", this);
        return &Type::errorType;
    }
    else {
        const vector<const VariableEntry*>* callParams = params();
        int callParamsSize = callParams->size();
        const SymTab *st = ee->symTab();
        if (st != NULL) {
            /*
               int i = 0;
               vector<const VariableEntry*>::const_iterator ic = callParams->begin();
               SymTab::const_iterator it = st->begin();
               for (i=0; it != (st->end()); ++it)  {
               VariableEntry *ve = (VariableEntry*) (*it);
               if (ve->varKind() == VariableEntry::VarKind::PARAM_VAR) {
               i++;
               }
               }
               */
            if (ee->getArgCnt() < callParamsSize) {
                errMsg("Event " + ee->name() +  " requires " + to_string(ee->getArgCnt()) + " arguments ", this);
                return &Type::errorType;
            }
        }
        else if (callParamsSize > 0) {
            errMsg(ee->name() +  ": mismatch in the number of arguments", this);
            return &Type::errorType;
        }
        else {
            event_error_flag = 1;
        }

    }

    const ExprNode *exp = cond();
    if (exp!= NULL && exp->exprNodeType() == ExprNode::ExprNodeType::OP_NODE)
    {
        if(cond_->doTypeCheck() != &Type::errorType)
            cond_error_flag = 1;
    }
    if(event_error_flag && cond_error_flag)
        return &Type::voidType;

    return &Type::errorType;
}

PatNode::PatNode(PatNodeKind pk, BasePatNode *p1, BasePatNode*p2, int line, int column, string file):
    BasePatNode(pk, line, column, file)
{
    pat1_ = p1;
    pat2_ = p2;
}


void ValueNode::print(ostream& os, int indent) const
{
    if(Value::printType && coercedType())
        os << "(" << Type::name(coercedType()->tag()) << ")";
    value()->print(os, indent);
}

const Type* ValueNode::typeCheck() const {
    return type();
}

vector<Quadruple*>* ValueNode::iCodeGen() {
    vector<Quadruple*> *quad = new vector<Quadruple*>();
    IntrCodeElem* tempVar = new IntrCodeElem(this, IntrCodeElem::ElemType::VAL_TYPE);
    setTVar(tempVar);
    return quad;
}

void RefExprNode::print(ostream& os, int indent) const
{
    if(!Value::printType)
        os << ext();
    else {
        if(coercedType())
            os << "(" << Type::name(coercedType()->tag()) << ")";
        os << symTabEntry()-> type() ->fullName();
    }
}

vector<Quadruple*>* RefExprNode::iCodeGen() {
    vector<Quadruple*> *quad = new vector<Quadruple*>();
    IntrCodeElem *tempVar = new IntrCodeElem(sym_, IntrCodeElem::ElemType::REF_EXPR_TYPE);
    if(getResultType()->tag() != sym_->type()->tag()) {
        IntrCodeElem *newTemp = new IntrCodeElem(new VariableEntry(Quadruple::fetchTempVar(),
                VariableEntry::VarKind::TEMP_VAR, getResultType()), IntrCodeElem::ElemType::TEMP_VAR_TYPE);
        quad->push_back(new Quadruple(OpNode::OpCode::MOVIF, tempVar, NULL, newTemp));
    }
    setTVar(tempVar);
    return quad;
}

InvocationNode::InvocationNode(const SymTabEntry *ste, vector<ExprNode*>* param,
                               int line, int column, string file):
    ExprNode(ExprNode::ExprNodeType::INV_NODE, NULL, line, column, file)
{
    params_ = param;
    ste_ = ste;
}

InvocationNode::InvocationNode(const InvocationNode &ie):ExprNode(ie)
{
    params_ = ie.params_;
    ste_ = ie.symTabEntry();
}

void InvocationNode::print(ostream& os, int indent) const
{
    const SymTabEntry *ste = symTabEntry();
    if (ste == nullptr) {
        os << "Function not defined";
    }
    else {
        FunctionEntry *fe = (FunctionEntry *) ste;
        if(coercedType())
            os << "(" << Type::name(coercedType()->tag()) << ")";
        os << fe -> name() << "(";
        if (params() != nullptr) {
            bool prtComma = false;
            for (std::vector<ExprNode*>::const_iterator it = params()->begin(); it != params()->end(); ++it) {
                if (prtComma)
                    os << ", ";
                (*it)->print(os, indent);
                prtComma = true;
            }
        }
        os << ")";
    }

}

const Type* InvocationNode::typeCheck() const {
    bool flag = true;
    const SymTabEntry *ste = symTabEntry();
    if(ste != nullptr && ste->kind() == SymTabEntry::Kind::UNKNOWN_KIND) {
        errMsg("undefined reference to " + ste->name(), this);
        return &Type::errorType;
    }
    else if (ste != nullptr && ste->kind() != SymTabEntry::Kind::UNKNOWN_KIND && ste->kind() != SymTabEntry::Kind::FUNCTION_KIND) {
        errMsg(ste->name() + " was not declared in the current scope", this);
        return &Type::errorType;
    }
    else {
        const vector<ExprNode*>* callParams = params();
        int callParamsSize = callParams->size();
        const SymTab *st = ste->symTab();
        if (st != NULL) {
            int i = 1;
            vector<ExprNode*>::iterator ic = params_->begin();
            SymTab::const_iterator it = st->begin();
            for (; it != (st->end())  && ic != (callParams->end()); ++it, ++ic)  {
                VariableEntry *ve = (VariableEntry*) (*it);
                if (ve->varKind() == VariableEntry::VarKind::PARAM_VAR) {
                    if(ve->type()->tag()  == Type::TypeTag::CLASS || (*ic)->doTypeCheck()->tag() == Type::TypeTag::CLASS) {
                        if(ve->type()->fullName().compare((*ic)->doTypeCheck()->fullName()) != 0) {
                            errMsg("Type mismatch for argument " + to_string(i) + " to " + ste->name(), this);
                            flag = false;
                        }
                    }
                    else if (ve->type()->tag() != (*ic)->doTypeCheck()->tag()) {
                        if(!Type::isSubType((*ic)->doTypeCheck(), ve->type())) {
                            errMsg("Type mismatch for argument " + to_string(i) + " to " + ste->name(), this);
                            flag = false;
                        }
                        else {
                            (*ic)->coercedType((ve->type()));
                        }
                    }
                    i++;
                }
            }
            if (((FunctionEntry*)ste)->getArgCnt() != callParamsSize) {
                errMsg(to_string(((FunctionEntry*)ste)->getArgCnt()) + " arguments expected for " + ste->name(), this);
                return &Type::errorType;
            }
        }
        else if (callParamsSize > 0) {
            errMsg(((FunctionEntry*)ste)->getArgCnt() + " arguments expected for " + ste->name(), this);
            return &Type::errorType;
        }
        else {
            return ((FunctionEntry*)ste)->type();
        }
    }
    if(flag) {
        return ((FunctionEntry*)ste)->type();
    }
    return &Type::errorType;
}

vector<Quadruple*>* InvocationNode::iCodeGen() {
    vector<Quadruple*>* quad = new vector<Quadruple*>();
    vector<IntrCodeElem*>* paramTVar = new vector<IntrCodeElem*>();

    for(vector<ExprNode*>::const_iterator it = params_->begin(); it != params_->end(); ++it) {
        vector<Quadruple*>* tempQuad = (*it)->iCodeGen();
        mergeVec(quad, tempQuad);
        paramTVar->push_back((*it)->getTVar());
    }

    IntrCodeParams* invcParam = new IntrCodeParams(paramTVar);
    IntrCodeElem* invcVar = new IntrCodeElem(this, IntrCodeElem::ElemType::INV_NODE_TYPE);
    IntrCodeElem* tempVar = new IntrCodeElem(new VariableEntry(Quadruple::fetchTempVar(), VariableEntry::VarKind::TEMP_VAR, getResultType()), IntrCodeElem::ElemType::TEMP_VAR_TYPE);
    quad->push_back(new Quadruple(OpNode::OpCode::CALL, invcVar, new IntrCodeElem(invcParam, IntrCodeElem::ElemType::PARAM_TYPE), tempVar));
    setTVar(tempVar);

    return quad;
}

vector<Instruction*>* InvocationNode::codeGen() {
    vector<Instruction*>* inst_vec = new vector<Instruction*>();

    // TODO: Add code for pushing all the params to the stack
    string label = regMgr->getNextLabel();
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::MOVL, "\"" + label + "\"", RET_ADDR_REG));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::STI, RET_ADDR_REG, SP_REG));
    inst_vec->push_back(Instruction::decrSP());
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::JMP, ((FunctionEntry*)symTabEntry())->getALabel()));
    inst_vec->push_back(new Instruction(Instruction::InstructionSet::PRTI, SP_REG, "", "", label));

    return inst_vec;
}

vector<Instruction*>* StmtNode::fetchExprRegValue(ExprNode* expr) {
    vector<Instruction*>* exprInst = new vector<Instruction*>();

    switch(expr->exprNodeType()) {
    case ExprNode::ExprNodeType::OP_NODE:
        insertQuadrupleSet(expr->iCodeGen());
        return Quadruple::iCodeToAsmGen(getICodeTable());
    case ExprNode::ExprNodeType::REF_EXPR_NODE:
        break;
    case ExprNode::ExprNodeType::VALUE_NODE:
        break;
    case ExprNode::ExprNodeType::INV_NODE:
        insertQuadrupleSet(expr->iCodeGen());
        return Quadruple::iCodeToAsmGen(getICodeTable());
    }
    // TODO: Call code generation on the quadruple table
    // Also perform the required optimization over the quadruple table and
    // generate the respective resultant reg value
    return exprInst;
}

const Type* ReturnStmtNode::typeCheck() const {
    const ExprNode *expr = exprNode();
    const FunctionEntry* funEnt = funEntry();
    if(expr != nullptr) {
        const Type* retType = expr_->doTypeCheck();
        if(funEnt != nullptr) {
            const Type* funRetType = funEnt->type();
            if(retType->tag() == funRetType->tag()) {
                return retType;
            }
            if (Type::isSubType(retType, funRetType)) {
                expr_->coercedType(funRetType);
                return funRetType;
            }
            if(funRetType->tag() == Type::TypeTag::VOID) {
                errMsg(" " + funEnt->name() + ": No return value expected for a void function", this);
                return &Type::errorType;
            }
            else {
                errMsg(" " + funEnt->name() + ": Return value incompatible with current function's type", this);
                return &Type::errorType;
            }
        }
        else {
            errMsg("return statement is not defined in the scope of the function", this);
            return &Type::errorType;
        }
    }
    else if(funEnt != nullptr) {
        if(funEnt->type()->tag() != Type::TypeTag::VOID)
            errMsg("return statement doesn't have a return value", this);
        return &Type::errorType;
    }
    return &Type::errorType;
}


vector<Quadruple*>* ReturnStmtNode::iCodeGen() {
    vector<Quadruple*>* inst_vec = new vector<Quadruple*>();
    mergeVec(inst_vec, expr_->iCodeGen());
    inst_vec->push_back(new Quadruple(OpNode::OpCode::RET, expr_->getTVar()));
    return inst_vec;
}

const Type* BreakStmtNode::typeCheck() const {
    const BlockEntry *be = blockEntry();
    if(be->name().compare("unknown") == 0) {
        errMsg("break statement is not declared inside a while loop", this);
        return &Type::errorType;
    }
    else if(num() <= ((WhileBlockEntry*)blockEntry())->nestedWhileCount()) {
        return &Type::voidType;
    }
    else {
        errMsg("break statement doesn't have the correct number argument", this);
        return &Type::errorType;
    }
    return &Type::errorType;
}

vector<Quadruple*>* BreakStmtNode::iCodeGen() {
    vector<Quadruple*>* inst_vec = new vector<Quadruple*>();
    WhileBlockEntry *wBE = (WhileBlockEntry*)blockEntry();
    int cnt = wBE->nestedWhileCount();
    vector<int> label = wBE->getWhileLabel();
    ostringstream os;
    os << "while_";
    int i = 0;
    for(vector<int>::iterator it = label.begin(); it != label.end() && i < cnt; ++it, i++) {
        os << to_string(*it) << "_";
    }
    os << "end";
    inst_vec->push_back(new Quadruple(OpNode::OpCode::JMP, new IntrCodeElem(new IntrLabel(os.str()), IntrCodeElem::ElemType::LABEL_TYPE)));
    return inst_vec;
}

const Type* ExprStmtNode::typeCheck() const {
    const ExprNode *expr = exprNode();
    if(expr != NULL && expr_->doTypeCheck()->tag() != Type::TypeTag::ERROR) {
        return &Type::unkType;
    }
    return &Type::errorType;
}

vector<Quadruple*>* PrimitivePatNode::iCodeGen() {
    vector<Quadruple*>* quad_vec = new vector<Quadruple*>();
    for (vector<VariableEntry*>::const_iterator it = params_->begin();
            it != params_->end(); ++it) {
        VariableEntry *ve = (*it);
        IntrCodeElem* tempVarEnt = new IntrCodeElem(new VariableEntry(Quadruple::fetchTempVar(), VariableEntry::VarKind::TEMP_VAR,
                ve->type()), IntrCodeElem::ElemType::TEMP_VAR_TYPE);
        quad_vec->push_back(new Quadruple(OpNode::OpCode::IN, NULL, NULL, tempVarEnt));
    }
    return quad_vec;
}


void PrimitivePatNode::print(ostream& os, int indent) const
{
    os << ee_->name();
    if (ee_->name().compare("any")) {
        os << "(";
        bool printComma = false;
        for (vector<VariableEntry*>::const_iterator it = params_->begin();
                it != params_->end(); ++it) {
            if (printComma)
                os << ", ";

            // TODO: handle the unknown param type in PrimitivePatNode::print().
            (*it)->print(os, indent);
            printComma = true;
        }
        os << ")";
    }
    if (cond()) {
        os << "|";
        cond()->print(os, indent);
    }
}

void PatNode::print(ostream& os, int indent) const
{
    PatNodeKind pk = kind();
    os << "(";
    if (pat1_ == NULL || pk == BasePatNode::PatNodeKind::UNDEFINED) {
        os << "Unknown event Called";
    } else {
        if (pk == BasePatNode::PatNodeKind::NEG)
            os << "!";
        pat1_->print(os, indent);
        if (pk == BasePatNode::PatNodeKind::SEQ)
            os << ":";
        else if (pk == BasePatNode::PatNodeKind::STAR)
            os << "**";
        else if (pk == BasePatNode::PatNodeKind::OR)
            os << " \\/ ";
        if (pat2_) {
            pat2_->print(os, indent);
        }
    }
    os << ")";
}


vector<Quadruple*>* PatNode::iCodeGen()  {
    vector<Quadruple*>* inst_vec = pat1_->iCodeGen();

    return inst_vec;
}

const Type* PatNode::typeCheck() const {


    const BasePatNode *p1 = pat1();
    const BasePatNode *p2 = pat2();

    int p1_type_check = 0;
    int p2_type_check = 0;
    if(p1 == NULL)
    {
        errMsg(" Atleast one event pattern operand expected ", this);
        return &Type::errorType;
    }
    else if(p1 != NULL)	    /* Atleast PAT1 should not be NULL  */
    {
        if (p1->typeCheck() != &Type::errorType)
            p1_type_check = 1;
        switch(kind())
        {
        case BasePatNode::PatNodeKind::PRIMITIVE:

            if(p2 != NULL)
            {
                errMsg(" Only one event pattern operand expected ", this);
                return &Type::errorType;
            }
            break;

        case BasePatNode::PatNodeKind::EMPTY:
            break;

        case BasePatNode::PatNodeKind::NEG:

            if(p2 != NULL)
            {
                errMsg(" Only one event pattern operand expected ", this);
                return &Type::errorType;
            }
            else if(!p1->isNegatable())
            {
                errMsg(" Only simple patterns without `.', `*', and `!' operatorscan be negated ", this);
                return &Type::errorType;
            }

            break;

        case BasePatNode::PatNodeKind::SEQ:
            if(p2 == NULL)
            {
                errMsg(" Event pattern operand expected ", this);
                return &Type::errorType;
            }
            /*
               if(p1->hasAnyOrOther() || p2->hasAnyOrOther()) {
               errMsg(" Only simple patterns without `.', `*', and `!' operatorscan be negated ", this);
               return &Type::errorType;
               }
               */
            if (p2->typeCheck() != &Type::errorType)
                p2_type_check = 1;
            break;

        case BasePatNode::PatNodeKind::OR:
            if(p2 == NULL)
            {
                errMsg(" Event pattern operand expected ", this);
                return &Type::errorType;
            }
            /*
               if(p1->hasAnyOrOther() || p2->hasAnyOrOther()) {
               errMsg(" Only simple patterns without `.', `*', and `!' operatorscan be negated ", this);
               return &Type::errorType;
               }
               */
            if (p2->typeCheck() != &Type::errorType)
                p2_type_check = 1;
            break;

        case BasePatNode::PatNodeKind::STAR:
            if(p2 != NULL)
            {
                errMsg(" Only one event pattern operand expected ", this);
                return &Type::errorType;
            }
            /*
               if(p1->hasAnyOrOther()) {
               errMsg(" Only simple patterns without `.', `*', and `!' operatorscan be negated ", this);
               return &Type::errorType;
               }
               */
            break;

        case BasePatNode::PatNodeKind::UNDEFINED:
            if(p2 != NULL)
            {
                errMsg(" Only one event pattern operand expected ", this);
                return &Type::errorType;
            }

            break;

        default :
            errMsg(" No such event pattern operand kind ", this);
            return &Type::errorType;
        }
    }
    if(p1_type_check && p2_type_check)
        return &Type::voidType;

    return &Type::errorType;
}

/* Check if PrimitivePatNode is of NegationKind */
bool PrimitivePatNode::hasNeg() const
{
    if(kind() == BasePatNode::PatNodeKind::SEQ)
        return true;
    return false;
}


/* Check if PrimitivePatNode is of NegationKind */
bool PrimitivePatNode::hasSeqOps() const
{
    if(kind() == BasePatNode::PatNodeKind::SEQ || kind() == BasePatNode::PatNodeKind::STAR)
        return true;
    return false;
}


/* Check if PrimitivePatNode is of NegationKind */
bool PrimitivePatNode::hasAnyOrOther() const
{
    /*
       if(kind() == BasePatNode::PatNodeKind::UNDEFINED)
       return true;
       return false;
       */
    if(event()->name().compare("any") == 0 || kind() == BasePatNode::PatNodeKind::UNDEFINED) {
        return true;
    }
    else
        return false;
}

/* */
bool PatNode::hasNeg() const
{
    if(kind() == BasePatNode::PatNodeKind::NEG)
        return true;
    return false;
}

bool PatNode::hasSeqOps() const
{

    if(kind() == BasePatNode::PatNodeKind::SEQ || kind() == BasePatNode::PatNodeKind::STAR)
        return true;
    return false;
}

bool PatNode::hasAnyOrOther() const
{
    bool flag1 = true;
    bool flag2 = true;
    if(pat1() != NULL) {
        flag1 = pat1()->hasAnyOrOther();
    }
    if(pat2() != NULL) {
        flag2 = pat2()->hasAnyOrOther();
    }

    if(flag1 || flag2) {
        return true;
    }
    return false;
}

void CompoundStmtNode::printWithoutBraces(ostream& os, int indent) const
{
    const std::list<StmtNode*> *stmtList = stmts();
    for (std::list<StmtNode*>::const_iterator it=stmtList->begin();
            it != stmtList->end(); ++it) {
        if ((*it) != NULL) {
            prtSpace(os, indent);
            (*it)->print(os, indent);
            if (stmtNoSemicolonAtEnd(*it))
                endln(os, indent);
        }
    }
}

void CompoundStmtNode::print(ostream& os, int indent) const
{
    os << "{";
    if (stmts()->size() != 0) {
        prtln(os, indent);
        printWithoutBraces(os, indent);
    }
    os << "}";
    endln(os, indent);
}

const Type* CompoundStmtNode::typeCheck() const {
    bool flag = false;
    const list<StmtNode*>* listStmts = stmts();

    if(listStmts == NULL) {
        return &Type::errorType;
    }
    for(list<StmtNode*>::const_iterator it = listStmts->begin(); it != listStmts->end(); ++it) {
        if((*it) != NULL && (*it)->typeCheck()->tag() != Type::TypeTag::UNKNOWN) {
            flag = true;
        }
    }
    if (flag) {
        return &Type::errorType;
    }

    return &Type::unkType;
}

vector<Instruction*>* CompoundStmtNode::codeGen() {
    vector<Instruction*>* inst_vec = new vector<Instruction*>();
    for(list<StmtNode*>::iterator it = stmts_->begin(); it != stmts_->end(); ++it) {
        mergeVec(inst_vec, (*it)->codeGen());
    }
    return inst_vec;
}

RefExprNode::RefExprNode(string ext, SymTabEntry* ste,
                         int line, int column, string file):
    ExprNode(ExprNode::ExprNodeType::REF_EXPR_NODE, NULL, line, column, file)
{
    ext_ = ext;
    sym_ = ste;
}
/****************************************************************/
extern const OpNode::OpInfo opInfo[] = {
    // print name, arity, paren_flag, fixity, arg types, out type, constraints
    //
    // Paren_flag -- opnode->print() outputs is surrounded by parenthesis if
    // this flag is set. As set below, the expression may not print correctly
    // in some rare cases, e.g., ~(b * c) will get printed as ~b * c,
    // which actually corresponds to (~b)*c. To ensure that things get printed
    // correctly all the time, more paren_flags should be set to 1, but this
    // will lead to more clutter in printed output. Basically, what we have done
    // here is to look are expressions by type -- arithmetic, relational,
    // boolean, bit operations, etc. Within each type, the highest priority
    // operator is printed without paren. This will work correctly, as long
    // as the language doesn't permit mixing of different types of expressions.
    // But this assumption doesn't always hold, as in the example above. Also,
    // there is an exception to this general approach in the case of unary minus
    // and * -- since (-a)*b and -(a*b) have the same meaning, we can exclude
    // paren for * without an error.
    //
    // Codes for constraints:
    // first character:
    //    N: No additional constraint over what is given by argTypes
    //    I: all arguments must have identical type
    //    S: one of the arguments must have a type that is a supertype of
    //        of all other arguments. All other arguments require a coercion
    //        operation to be introduced so as to convert their type to S.
    //    s: one of the arguments must have a type that is a subtype of
    //        of all other arguments.
    //    L: all list arguments (and list output) must have same type. In
    //        addition, all non-list arguments (and output) must have same
    //        type as that of elements in these lists
    //    T: all tuple arguments to the function must have same type.
    //    A: (assignment). Type of second argument must be a subtype of
    //       the first argument
    //
    // second character:
    //    O: output type is the same as out type. (In the following cases,
    //        the output type need not be equal to out type, but a subtype
    //        of it.) Since a TypeTag provides complete type information only

    //        for primitive types, `O' is applicable only in this case.
    //    digit: output type is the same as that of the digit'th argument
    //       In this case, a third character may be used, the code for
    //       which is as follows:
    //         'e' denotes that the output is of type alpha, where
    //             the type of digit'th argument is list(alpha)
    //         'l' denotes that the output is of type list(alpha), where
    //             alpha is the type of the digit'th argument.
    //    S: The output type is the same as that of the argument with the
    //        most general type. (Typically used with first character 'S')
    //    s: The output type is the same as that of the argument with the
    //        least general type. (Typically used with first character 'S')
    //    P: The output type is the product of the types of all arguments
    //    p: The output type is a component of the input tuple type. The
    //        following character specifies the component. A digit k specifies
    //        that the component number as k. The character 'a' indicates that
    //        the component number is given by an integer argument to the
    //        operator. The argument number is given by the following digit.
    //        'p' can be used only in conjunction with first character 'P'.
    //    L: Output type is the same as type of list arguments. Can be used
    //        only in conjunction with first character L.
    //    e: Output type is the same as type of element of list arguments.
    //        Can be used only in conjunction with first character L.
    //

    {OpNode::OpCode::UMINUS, "-",  1, 0, OpNode::OpPrintType::PREFIX, {Type::NUMERIC}, Type::SIGNED, "N1"},
    {OpNode::OpCode::PLUS, "+",  2, 1, OpNode::OpPrintType::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
    {OpNode::OpCode::MINUS, "-",  2, 1, OpNode::OpPrintType::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
    {OpNode::OpCode::MULT, "*",  2, 0, OpNode::OpPrintType::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
    {OpNode::OpCode::DIV, "/",  2, 1, OpNode::OpPrintType::INFIX, {Type::NUMERIC, Type::NUMERIC}, Type::NUMERIC, "SS"},
    {OpNode::OpCode::MOD, "%",  2, 1, OpNode::OpPrintType::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "S2"},
    {OpNode::OpCode::EQ, "==", 2, 0, OpNode::OpPrintType::INFIX, {Type::PRIMITIVE, Type::PRIMITIVE}, Type::BOOL, "SO"},
    {OpNode::OpCode::NE, "!=", 2, 0, OpNode::OpPrintType::INFIX, {Type::PRIMITIVE, Type::PRIMITIVE}, Type::BOOL, "SO"},
    {OpNode::OpCode::GT, ">",  2, 0, OpNode::OpPrintType::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
    {OpNode::OpCode::LT, "<",  2, 0, OpNode::OpPrintType::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
    {OpNode::OpCode::GE, ">=", 2, 0, OpNode::OpPrintType::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
    {OpNode::OpCode::LE, "<=", 2, 0, OpNode::OpPrintType::INFIX, {Type::SCALAR, Type::SCALAR}, Type::BOOL, "SO"},
    {OpNode::OpCode::AND, "&&",  2, 1, OpNode::OpPrintType::INFIX, {Type::BOOL, Type::BOOL}, Type::BOOL, "NO"},
    {OpNode::OpCode::OR, "||",  2, 1, OpNode::OpPrintType::INFIX, {Type::BOOL, Type::BOOL}, Type::BOOL, "NO"},
    {OpNode::OpCode::NOT, "!",  1, 0, OpNode::OpPrintType::PREFIX, {Type::BOOL}, Type::BOOL, "NO"},
    {OpNode::OpCode::BITNOT, "~",  1, 0, OpNode::OpPrintType::PREFIX, {Type::INTEGRAL}, Type::INTEGRAL, "N1"},
    {OpNode::OpCode::BITAND, "&",  2, 1, OpNode::OpPrintType::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "Ss"},
    {OpNode::OpCode::BITOR, "|",  2, 1, OpNode::OpPrintType::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "SS"},
    {OpNode::OpCode::BITXOR, "^",  2, 0, OpNode::OpPrintType::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "SS"},
    {OpNode::OpCode::SHL, "<<", 2, 1, OpNode::OpPrintType::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "N1"},
    {OpNode::OpCode::SHR, ">>", 2, 1, OpNode::OpPrintType::INFIX, {Type::INTEGRAL, Type::INTEGRAL}, Type::INTEGRAL, "N1"},
    {OpNode::OpCode::ASSIGN, "=",  2, 0, OpNode::OpPrintType::INFIX, {Type::NATIVE, Type::NATIVE}, Type::VOID, "AO"},
    {OpNode::OpCode::PRINT, "print", OpNode::VARIABLE, 1, OpNode::OpPrintType::PREFIX, {Type::NATIVE}, Type::VOID, "NO"},
    {OpNode::OpCode::INVALID, "invalid",            0, 0, OpNode::OpPrintType::PREFIX, {}, Type::ERROR, "NO"}
};


bool checkType(const Type::TypeTag typeTag, const Type *type) {
    Type::TypeTag tag = type->tag();
    switch(typeTag) {
    case Type::TypeTag::NUMERIC : {
        return Type::isNumeric(tag);
    }
    case Type::TypeTag::SIGNED : {
        return Type::isSigned(tag);
    }
    case Type::TypeTag::INTEGRAL : {
        return Type::isIntegral(tag);
    }
    case Type::TypeTag::PRIMITIVE : {
        return Type::isPrimitive(tag);
    }
    case Type::TypeTag::SCALAR : {
        return Type::isScalar(tag);
    }
    case Type::TypeTag::BOOL : {
        return Type::isBool(tag);
    }
    case Type::TypeTag::NATIVE : {
        return Type::isNative(tag);
    }
    default:
        break;
    }
    //cout >> "TypeTag invalid";
    return false;
}

bool argTypeCheck(const Type::TypeTag argType[], unsigned arity, const Type** argTypes, const OpNode* opNode) {

    for (unsigned i=0; i < arity; i++) {
        const Type *type = argTypes[i];
        if (!(checkType(argType[i], type))) {
            errMsg("Incompatible type for argument " + to_string(i+1) + " for operator `"+ opInfo[static_cast<int>(opNode->opCode())].name_ +"'", opNode);
            return false;
        }
    }
    return true;
}

const Type* OpNode::typeCheck() const {
    //TODO: try to write generic code for n operands
    int iopcode = static_cast<int>(opCode_);
    const Type** argTypes = (const Type**)new Type*[arity_];
    for (unsigned i=0; i < arity_; i++) {
        if (arg_[i]) {
            const Type *type = arg_[i]->doTypeCheck();
            arg_[i]->setResultType(type);
            argTypes[i] = type;
            if (type->tag() == Type::TypeTag::ERROR) {
                delete[] argTypes;
                return Type::type[0];
            }
        }
    }
    bool error = false;
    switch (opInfo[iopcode].typeConstraints_[0])
    {
    case 'N':
        if (!argTypeCheck(opInfo[iopcode].argType_, arity_, argTypes, this)) {
            error = true;
        }
        break;
    case 'S':
        if (!argTypeCheck(opInfo[iopcode].argType_, arity_, argTypes, this)) {
            error = true;
            break;
        }
        if(argTypes[1]->tag() == argTypes[0]->tag())
            break;
        if(opInfo[iopcode].typeConstraints_[1] != 'O') {
            if (Type::isSubType(argTypes[1], argTypes[0]))
                arg_[1]->coercedType(argTypes[0]);
            else if(Type::isSubType(argTypes[0], argTypes[1]))
                arg_[0]->coercedType(argTypes[1]);
        }
        break;
    case 'A':
        if (argTypes[1]->tag() != argTypes[0]->tag() && !Type::isSubType(argTypes[1], argTypes[0])) {
            errMsg("Assigned expression must be a subtype of target", this);
            error = true;
        }
        if (argTypes[1]->tag() != argTypes[0]->tag() && Type::isSubType(argTypes[1], argTypes[0]))
            arg_[1]->coercedType(argTypes[0]);
        break;
    }

    if (error) {
        delete[] argTypes;
        return Type::type[0];
    }

    const Type* returnType = Type::type[0];
    switch(opInfo[iopcode].typeConstraints_[1])
    {
    case '1':
    case '2':
    {
        int index = atoi(&opInfo[iopcode].typeConstraints_[1]) - 1;
        const Type *oprType = arg_[index]->coercedType() != NULL ? arg_[index]->coercedType():argTypes[index];
        if(!checkType(opInfo[iopcode].outType_, oprType)) {
            if((returnType = Type::getCoercedType(opInfo[iopcode].outType_, oprType))) {
                arg_[index]->coercedType(returnType);
                break;
            }
            else {
                returnType = Type::type[0];
                error = true;
                //TODO: show correct error
                errMsg("Incorrect Operand", this);
            }
        }
        else
            returnType = oprType;
        break;
    }
    case 'O':
        returnType = new Type(opInfo[iopcode].outType_);
        break;
    case 's':
        if (Type::isSubType(argTypes[1], argTypes[0]))
            returnType = argTypes[1];
        else
            returnType = argTypes[0];
    case 'S':
        if (Type::isSubType(argTypes[1], argTypes[0]))
            returnType = argTypes[0];
        else
            returnType = argTypes[1];
    }
    delete[] argTypes;
    return returnType;
}

OpNode::OpNode(OpCode op, ExprNode* a1, ExprNode* a2,
               int ln, int col, string file):
    ExprNode(ExprNode::ExprNodeType::OP_NODE, NULL, ln,col,file) {
    opCode_ = op;
    if (a1 != NULL) {
        arity_ = 1;
        arg_.push_back(a1);
        if (a2 != NULL) {
            arity_++;
            arg_.push_back(a2);
        }
    }
}

OpNode::OpNode(const OpNode &other):
    ExprNode(other) {
    arity_ = other.arity();
    opCode_ = other.opCode();
    for (unsigned int i=0; (i < other.arity()); i++) {
        if (other.arg_[i]) {
            arg_.push_back((other.arg_[i])->clone());
        }
        else {
            arg_.push_back(NULL);
        }
    }
}

const char* opCodeStr_[] = {
    "UMINUS", "PLUS", "MINUS", "MULT", "DIV", "MOD",
    "EQ", "NE", "GT", "LT", "GE", "LE",
    "AND", "OR", "NOT",
    "BITNOT", "BITAND", "BITOR", "BITXOR", "SHL", "SHR",
    "ASSIGN", "PRINT", "INVALID"
};

vector<Quadruple*>* OpNode::iCodeGen() {
    vector<Quadruple*>* quad = new vector<Quadruple*>();
    vector<IntrCodeElem*>* operands = new vector<IntrCodeElem*>();
    string reg;
    for(int i = 0; i < (signed int)arity_; i++) {
        mergeVec(quad, arg_[i]->iCodeGen());
        operands->push_back(arg_[i]->getTVar());

        /*
            switch(arg_[i]->exprNodeType()) {
            case ExprNode::ExprNodeType::OP_NODE:
            operands->push_back(new IntrCodeElem(new VariableEntry(arg_[i]->getTReg(), VariableEntry::VarKind::TEMP_VAR, getResultType()), IntrCodeElem::ElemType::TEMP_VAR_TYPE));
            break;
            case ExprNode::ExprNodeType::REF_EXPR_NODE:
            operands->push_back(new IntrCodeElem(((RefExprNode*)arg_[i])->symTabEntry(), IntrCodeElem::ElemType::VAR_TYPE));
            break;
            case ExprNode::ExprNodeType::VALUE_NODE:
            operands->push_back(new IntrCodeElem(new VariableEntry(arg_[i]->value()->toString(), VariableEntry::VarKind::TEMP_VAR, t), IntrCodeElem::ElemType::VAL_TYPE));
            break;
            case ExprNode::ExprNodeType::INV_NODE:
            if (Type::isInt(t->tag()) || Type::isString(t->tag()))
            reg = RETI_REG;
            else if (Type::isFloat(t->tag()))
            reg = RETF_REG;
            operands->push_back(new IntrCodeElem(new VariableEntry(reg, VariableEntry::VarKind::TEMP_VAR, t), IntrCodeElem::ElemType::INV_NODE_TYPE));
            break;
            }
            }
            */
    }
    IntrCodeElem* tempVarEnt = new IntrCodeElem(new VariableEntry(Quadruple::fetchTempVar(), VariableEntry::VarKind::TEMP_VAR,
            getResultType()), IntrCodeElem::ElemType::TEMP_VAR_TYPE);
    switch(arity_) {
    case 1:
        quad->push_back(new Quadruple(opCode_, operands->at(0), NULL, tempVarEnt));
        break;
    case 2:
        if(opCode_ == OpNode::OpCode::ASSIGN)
            quad->push_back(new Quadruple(opCode_, operands->at(1), NULL, operands->at(0)));
        else
            quad->push_back(new Quadruple(opCode_, operands->at(0), operands->at(1), tempVarEnt));
        break;
    }

    setTVar(tempVarEnt);
    return quad;
}

void
OpNode::print(ostream& os, int indent) const {
    int iopcode = static_cast<int>(opCode_);
    if (opInfo[iopcode].prtType_ == OpNode::OpPrintType::PREFIX) {

        os << opInfo[iopcode].name_;

        if(coercedType())
            os << "(" << Type::name(coercedType()->tag()) << ")";
        if (arity_ > 0) {
            if (opInfo[iopcode].needParen_)
                os << '(';
            for (unsigned i=0; i < arity_-1; i++) {
                if (arg_[i])
                    arg_[i]->print(os, indent);
                else os << "NULL";
                os << ", ";
            }
            if (arg_[arity_-1])
                arg_[arity_-1]->print(os, indent);
            else os << "NULL";
            if (opInfo[iopcode].needParen_)
                os << ") ";
        }
    }
    else if ((opInfo[iopcode].prtType_ == OpNode::OpPrintType::INFIX) && (arity_ == 2)) {
        if(coercedType())
            os << "(" << Type::name(coercedType()->tag()) << ")";
        if (opInfo[iopcode].needParen_)
            os << "(";
        if (arg_[0])
            arg_[0]->print(os, indent);
        else os << "NULL";
        os << opInfo[iopcode].name_;
        if (arg_[1])
            arg_[1]->print(os, indent);
        else os << "NULL";
        if (opInfo[iopcode].needParen_)
            os << ")";
    }
    else internalErr("Unhandled case in OpNode::print");
}




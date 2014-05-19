
#ifndef INTRCODEELEM_HEADERS_H
#define INTRCODEELEM_HEADERS_H
class IntrCodeElem {
public:
    enum class ElemType {
        VAR_TYPE, TEMP_VAR_TYPE, VAL_TYPE,
        INV_NODE_TYPE, REF_EXPR_TYPE, QUAD_TYPE,
        LABEL_TYPE, PARAM_TYPE
    };

    IntrCodeElem(ProgramElem *elem, ElemType type) {
        progElem_ = elem;
        type_ = type;
    };

    ProgramElem* getElem()  {
        return progElem_;
    };
    ElemType getType() {
        return type_;
    };

    bool equals(IntrCodeElem *p);
    bool uses(IntrCodeElem *p);

    static string toString(IntrCodeElem *ice);

private:
    ElemType type_;
    ProgramElem* progElem_;
};

#endif

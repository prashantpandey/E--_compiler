#include "Type.h"
#include "SymTabEntry.h"

#include <stdlib.h>

const char* typeName[] = {
  "error", "unknown", "void", 
  "bool", 
  "string",
  "byte",
  "unsigned int", "int", 
  "double",
  "class", 
  "event", "function",
  "numeric",
  "integral",
  "signed",
  "unsigned",
  "scalar",
  "primitive",
  "native",
  "data",
  "any"
};

const string Type::name(TypeTag t) {
  if((t >= ERROR) && (t <= ANY))
    return string(typeName[t]);
  else return string();
}

Type
  Type::errorT(ERROR), Type::unkT(UNKNOWN), Type::voidT(VOID),
  Type::boolT(BOOL),
  Type::stringT(STRING),
  Type::byteT(BYTE), 
  Type::uintT(UINT), Type::intT(INT), 
  Type::doubleT(DOUBLE);

const Type
  Type::errorType(ERROR), Type::unkType(UNKNOWN), Type::voidType(VOID),
  Type::boolType(BOOL),
  Type::stringType(STRING),
  Type::byteType(BYTE), 
  Type::uintType(UINT), Type::intType(INT), 
  Type::doubleType(DOUBLE);

const Type* Type::type[] = {
  &Type::errorType, &Type::unkType, &Type::voidType, 
  &Type::boolType, 
  &Type::stringType,
  &Type::byteType, 
  &Type::uintType, &Type::intType,
  &Type::doubleType,
  NULL, NULL, NULL,
  NULL, NULL,
  NULL, NULL, NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/****************************************************************
   Type Class 
****************************************************************/

Type::Type(TypeTag tag) {
  tag_ = tag; 
  argTypes_ = NULL; 
  retType_ = NULL;
};     

Type::Type(SymTabEntry* td, TypeTag t) {   // Must be enum, class or struct
  tag_ = t; 
  typeDesc_ = td; 
};

Type::Type(vector<Type*>* tupleType, TypeTag t) {// For tuples, modules, events
  tag_ = t; 
  argTypes(tupleType); 
};

Type::Type(vector<Type*>* argt, Type* rt) {      // For functions
  tag_ = FUNCTION; 
  retType_ = rt; 
  argTypes_ = argt; 
};

string 
Type::fullName() const {
  string info;
  switch (tag_) {
    case CLASS:
      return info + name() + " " + typeDesc_->name() ;

    case EVENT:
    case FUNCTION:
      info += name(tag_) + " (";
      if (argTypes_ != NULL) {
		for (unsigned int i=0; i< argTypes_->size(); i++) {
		  if ((*argTypes_)[i] != NULL)
			info += (*argTypes_)[i]->fullName() ;  
		  if (i < argTypes_->size() - 1)
			info += ", ";
		}
	  }
      if (tag_ == FUNCTION) {
		info += "): ";
		info += retType_->fullName() + " ";
	  }
	  else info += ") ";
      return info;

    default:
      break;
  }
  return info+name();
}  

const Type&
Type::operator=(const Type& t) {
  tag_ = t.tag_;
  switch (tag_) {
    case CLASS: 
      typeDesc_ = t.typeDesc_;
      break;

    case EVENT:
    case FUNCTION: 
      if (t.argTypes_ != NULL) {
		argTypes_ = new vector<Type*>(t.argTypes_->size());
		for (unsigned int i=0; i<t.argTypes_->size(); i++) 
		  (*argTypes_)[i] = new Type(*((*t.argTypes_)[i]));
      } 
	  else argTypes_ = NULL;
	  if (tag_ == FUNCTION) {
		if (t.retType_ != NULL)
		  retType_ = new Type(*(t.retType_));
		else retType_ = new Type(VOID);
	  }
      break;

    default:
      break;
  }
  return *this;
}

void 
Type::print(ostream& os, int indent) const {
  os << fullName();
}

const Type* Type::getCoercedType(const Type::TypeTag tag, const Type* type) {
    switch(tag)
    {
	case TypeTag::SIGNED: if(isIntegral(type->tag()))
				return &intType;
			      else if (isFloat(type->tag()))
				return &doubleType;
			      break;
	//TODO: Need to handle more cases
	default : break;
			     
    }
    return &errorType;
}

bool Type::isSubType(const Type *type1, const Type *type2) {
    const Type::TypeTag curTag = type1->tag();
    if(isNumeric(curTag)) {
	if(curTag < type2->tag()) {
	    return true;
	}
	return false;
    }
    return false;
}

int Type::getTypeSize(const Type *type) {
    switch(type->tag()) 
    {
	case TypeTag::BOOL:
	    return 1;
	case TypeTag::BYTE:
	    return 1;
	case TypeTag::UINT:
	    return 4;
	case TypeTag::INT:
	    return 4;
	case TypeTag::DOUBLE:
	    return 8;
	case TypeTag::STRING:
	    return 8;
	case TypeTag::CLASS:
	    return 8;
	case TypeTag::EVENT:
	    return 8;
	case TypeTag::FUNCTION:
	    return 8;
	default:
	    return 0;
    }
}

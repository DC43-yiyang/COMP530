
#ifndef SQL_EXPRESSIONS
#define SQL_EXPRESSIONS

#include "MyDB_AttType.h"
#include <string>
#include <vector>

// create a smart pointer for database tables
using namespace std;
class ExprTree;
typedef shared_ptr <ExprTree> ExprTreePtr;

// this class encapsules a parsed SQL expression (such as "this.that > 34.5 AND 4 = 5")

// class ExprTree is a pure virtual class... the various classes that implement it are below
class ExprTree {

public:

	// here are a bunch of self-explanatory operations that allow manipulation of expressions
	virtual string toString () = 0;
	virtual ~ExprTree () {}
	virtual bool isEq () {return false;}
	virtual bool isId () {return false;}
	virtual bool isOr () {return false;}
	virtual bool isComp () {return false;}
	virtual bool isLTGT () {return false;}
	virtual bool isNotEq () {return false;}
	virtual bool isSum () {return false;}
	virtual bool isAvg () {return false;}
	virtual string getId () {return "";}
	virtual ExprTreePtr getLHS () {return nullptr;}
	virtual ExprTreePtr getRHS () {return nullptr;}
	virtual ExprTreePtr getChild () {return nullptr;}
	virtual bool referencesTable (string alias) {return false;}
	virtual bool referencesAtt (string alias, string attName) {return false;}
	virtual bool hasAgg () {
		if (isSum () || isAvg ())
			return true;
		ExprTreePtr lhs = getLHS ();
		ExprTreePtr rhs = getRHS ();
		ExprTreePtr child = getChild ();
		bool res = false;
		if (lhs != nullptr)
			res = res || lhs->hasAgg (); 
		if (rhs != nullptr)
			res = res || rhs->hasAgg (); 
		if (child != nullptr)
			res = res || child->hasAgg (); 
		return res;
	} 
};

class BoolLiteral : public ExprTree {

private:
	bool myVal;
public:
	
	BoolLiteral (bool fromMe) {
		myVal = fromMe;
	}

	string toString () {
		if (myVal) {
			return "bool[true]";
		} else {
			return "bool[false]";
		}
	}	
};

class DoubleLiteral : public ExprTree {

private:
	double myVal;
public:

	DoubleLiteral (double fromMe) {
		myVal = fromMe;
	}

	string toString () {
		return "double[" + to_string (myVal) + "]";
	}	

	~DoubleLiteral () {}
};

// this implement class ExprTree
class IntLiteral : public ExprTree {

private:
	int myVal;
public:

	IntLiteral (int fromMe) {
		myVal = fromMe;
	}

	string toString () {
		return "int[" + to_string (myVal) + "]";
	}

	~IntLiteral () {}
};

class StringLiteral : public ExprTree {

private:
	string myVal;
public:

	StringLiteral (char *fromMe) {
		fromMe[strlen (fromMe) - 1] = 0;
		myVal = string (fromMe + 1);
	}

	string toString () {
		return "string[" + myVal + "]";
	}

	~StringLiteral () {}
};

class Identifier : public ExprTree {

private:
	string tableName;
	string attName;
public:

	Identifier (char *tableNameIn, char *attNameIn) {
		tableName = string (tableNameIn);
		attName = string (attNameIn);
	}

	string toString () {
		return "[" + tableName + "_" + attName + "]";
	}	

	string getId () {
		return tableName + "_" + attName;
	}

	bool isId () {
		return true;
	}

	bool referencesTable (string alias) {
		return alias == tableName;
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return alias == tableName && attName == attNameToFind;
	}

	~Identifier () {}
};

class MinusOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	MinusOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "- (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~MinusOp () {}
};

class PlusOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	PlusOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	string toString () {
		return "+ (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~PlusOp () {}
};

class TimesOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	TimesOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	string toString () {
		return "* (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~TimesOp () {}
};

class DivideOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	DivideOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "/ (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~DivideOp () {}
};

class GtOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	GtOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "> (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool isLTGT () {
		return true;
	}
	
	bool isComp () {
		return true;
	}

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~GtOp () {}
};

class LtOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	LtOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "< (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool isLTGT () {
		return true;
	}

	bool isComp () {
		return true;
	}

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~LtOp () {}
};

class NeqOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	NeqOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	bool isNotEq () {
		return true;
	}

	bool isComp () {
		return true;
	}

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	string toString () {
		return "!= (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	~NeqOp () {}
};

class OrOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	OrOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "|| (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool isOr () {
		return true;
	}

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~OrOp () {}
};

class EqOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	EqOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	bool isComp () {
		return true;
	}

	bool isEq () {
		return true;
	}

	string getId () {
		string res = lhs->getId ();
		if (res != "")
			return res;
		else
			return rhs->getId ();
	}

	ExprTreePtr getLHS () {
		return lhs;
	}

	ExprTreePtr getRHS () {
		return rhs;
	}

	string toString () {
		return "== (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool referencesTable (string alias) {
		return lhs->referencesTable (alias) || rhs->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return lhs->referencesAtt (alias, attNameToFind) || rhs->referencesAtt (alias, attNameToFind);
	}

	~EqOp () {}
};

class NotOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	NotOp (ExprTreePtr childIn) {
		child = childIn;
	}

	bool isComp () {
		return true;
	}

	ExprTreePtr getChild () {
		return child;
	}

	string getId () {
		return child->getId ();
	}

	string toString () {
		return "!(" + child->toString () + ")";
	}	

	bool referencesTable (string alias) {
		return child->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return child->referencesAtt (alias, attNameToFind);
	}

	~NotOp () {}
};

class SumOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	SumOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "sum(" + child->toString () + ")";
	}	

	bool isSum () {
		return true;
	}

	bool referencesTable (string alias) {
		return child->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return child->referencesAtt (alias, attNameToFind);
	}


	~SumOp () {}
};

class AvgOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	AvgOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "avg(" + child->toString () + ")";
	}	

	bool isAvg () {
		return true;
	}

	bool referencesTable (string alias) {
		return child->referencesTable (alias);
	}

	bool referencesAtt (string alias, string attNameToFind) {
		return child->referencesAtt (alias, attNameToFind);
	}

	~AvgOp () {}
};

#endif


#ifndef SQL_PARSER_C
#define SQL_PARSER_C

#include <iostream>
#include <stdlib.h>
#include "ExprTree.h"
#include "../source/Parser.h"
#include "ParserHelperFunctions.h"
#include "ParserTypes.h"
#include <string>
#include <utility>

using namespace std;

/*************************************************/
/** HERE WE DEFINE ALL OF THE C FUNCTIONS TO    **/
/** MANIPULATE THE ABOVE DATA TYPES             **/
/*************************************************/

extern "C" {

struct AttList *appendAttList (struct AttList *appendToMe, struct AttList *appendMe) {
	for (auto a : appendMe->atts) {
		appendToMe->atts.push_back (a);
	}
	delete appendMe;
	return appendToMe;
}

struct AttList *makeAttList (char *attName, int whichType) {
	if (whichType == DOUBLE) {
		return new AttList (string (attName), make_shared <MyDB_DoubleAttType> ());
	} else if (whichType == BOOL) {
		return new AttList (string (attName), make_shared <MyDB_BoolAttType> ());
	} else if (whichType == INT) {
		return new AttList (string (attName), make_shared <MyDB_IntAttType> ());
	} else if (whichType == STRING) {
		return new AttList (string (attName), make_shared <MyDB_StringAttType> ());
	} else {
		return nullptr;
	}
}

struct FromList *appendFromList (struct FromList *appendToMe, char *tableName, char *aliasName) {
	appendToMe->aliases.push_back (make_pair (string (tableName), string (aliasName)));
	free (tableName);
	free (aliasName);
	return appendToMe;
}

struct CNF *makeCNF (struct Value *fromMe) {
	auto returnVal = new CNF (fromMe);
	delete fromMe;
	return returnVal;
}

struct ValueList *makeValueList (struct Value *fromMe) {
	auto returnVal = new ValueList (fromMe);
	delete fromMe;
	return returnVal;
}

struct ValueList *pushBackValue (struct ValueList *ontoMe, struct Value *withMe) {
	ontoMe->valuesToCompute.push_back (withMe->myVal);
	delete withMe;
	return ontoMe;
}

struct CNF *pushBackDisjunction (struct CNF *ontoMe, struct Value *withMe) {
	ontoMe->disjunctions.push_back (withMe->myVal);
	delete withMe;
	return ontoMe;
}

struct Value *times (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <TimesOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *plus (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <PlusOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *divide (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <DivideOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *minus (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <MinusOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *makeIdentifier (char *tableName, char *attName) {
	Value *returnVal = new Value (make_shared <Identifier> (tableName, attName));
	free (tableName);
	free (attName);
	return returnVal;
}

struct Value *makeDouble (double fromMe) {
	Value *returnVal = new Value (make_shared <DoubleLiteral> (fromMe));
	return returnVal;
}

struct Value *makeString (char *fromMe) {
	Value *returnVal = new Value (make_shared <StringLiteral> (fromMe));
	return returnVal;
}

struct Value *makeInt (int fromMe) {
	Value *returnVal = new Value (make_shared <IntLiteral> (fromMe));
	return returnVal;
}

struct Value *sum (struct Value *ofMe) {
	ofMe->myVal = make_shared <SumOp> (ofMe->myVal);		
	return ofMe;
}

struct Value *avg (struct Value *ofMe) {
	ofMe->myVal = make_shared <AvgOp> (ofMe->myVal);		
	return ofMe;
}

struct Value *nott (struct Value *ofMe) {
	ofMe->myVal = make_shared <NotOp> (ofMe->myVal);		
	return ofMe;
}

struct Value *eq (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <EqOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *neq (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <NeqOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *gt (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <GtOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *lt (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <LtOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct Value *orr (struct Value *lhs, struct Value *rhs) {
	lhs->myVal = make_shared <OrOp> (lhs->myVal, rhs->myVal);
	delete rhs;	
	return lhs;
}

struct SQLStatement *makeCreateTable (struct CreateTable *fromMe) {
	return new SQLStatement (fromMe);
	delete fromMe;
}

struct SQLStatement *makeSelectQuery (struct SFWQuery *fromMe) {
	return new SQLStatement (fromMe);
	delete fromMe;
}

struct FromList *makeFromList (char *tableName, char *aliasName) {
	struct FromList *returnVal = new FromList (tableName, aliasName);
	free (tableName);
	free (aliasName);
	return returnVal;
}

struct SFWQuery *makeQueryWithGroupBy (struct ValueList *selectClause, struct FromList *fromClause, 
	struct CNF *cnf, struct ValueList *grouping) {
	auto returnVal = new SFWQuery (selectClause, fromClause, cnf, grouping);
	delete selectClause;
	delete fromClause;
	delete cnf;
	delete grouping;
	return returnVal;
}

struct SFWQuery *makeQueryNoWhere (struct ValueList *selectClause, struct FromList *fromClause) {
	auto returnVal = new SFWQuery (selectClause, fromClause);
	delete selectClause;
	delete fromClause;
	return returnVal;
}

struct SFWQuery *makeQuery (struct ValueList *selectClause, struct FromList *fromClause, 
	struct CNF *cnf) {
	auto returnVal = new SFWQuery (selectClause, fromClause, cnf);
	delete selectClause;
	delete fromClause;
	delete cnf;
	return returnVal;
}

struct CreateTable *makeTableRegular (char *tableName, struct AttList *fromMe) {
	auto returnVal = new CreateTable (string (tableName), fromMe->atts);
	free (tableName);
	delete fromMe;
	return returnVal;
}

struct CreateTable *makeTableBPlusTree (char *tableName, struct AttList *fromMe, char *attName) {
	auto returnVal = new CreateTable (string (tableName), fromMe->atts, string (attName));
	free (tableName);
	delete fromMe;
	delete attName;
	return returnVal;
}

// structure that stores a list of aliases from a FROM clause
} // extern

#endif

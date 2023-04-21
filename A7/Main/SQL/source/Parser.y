
%{
	#include "Lexer.h"
	#include "ParserHelperFunctions.h" 
	//#include "Parser.h" 
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

%}

// this stores all of the types returned by production rules
%union {
	struct SQLStatement *myStatement;
	struct SFWQuery *mySelectQuery;
	struct CreateTable *myCreateTable;
	struct FromList *myFromList;
	struct AttList *myAttList;
	struct Value *myValue;
	struct ValueList *allValues;
	struct CNF *myCNF;	
	int myInt;
	char *myChar;
	double myDouble;
};

%pure-parser
%lex-param {void *scanner}
%parse-param {void *scanner}
%parse-param {struct SQLStatement **myStatement}

%token <myInt> INTEGER
%token <myChar> IDENTIFIER
%token <myDouble> DBL
%token <myChar> STR 
%token SELECT
%token FROM
%token WHERE
%token AS
%token BY
%token AND
%token OR
%token NOT
%token SUM
%token AVG
%token GROUP
%token INT
%token BOOL
%token BPLUSTREE
%token CREATE
%token DOUBLE
%token STRING
%token ON
%token TABLE

%type <myValue> Value
%type <myValue> MultExp
%type <myValue> Literal
%type <myCNF> CNF
%type <myValue> Disjunction
%type <myValue> Comparison
%type <allValues> ValueList
%type <myStatement> SQLStatement
%type <myCreateTable> CreateTable
%type <myAttList> AttList
%type <myAttList> Att
%type <myFromList> FromList
%type <mySelectQuery> SelectQuery 

%start SQLStatement

//******************************************************************************
// SECTION 3
//******************************************************************************
/* This is the PRODUCTION RULES section which defines how to "understand" the 
 * input language and what action to take for each "statment"
 */

%%

SQLStatement: SelectQuery
{
	$$ = makeSelectQuery ($1);
	*myStatement = $$;
}

| CreateTable 
{
	$$ = makeCreateTable ($1);
	*myStatement = $$;
}

//********* Create Table Statement

CreateTable: CREATE TABLE IDENTIFIER '(' 
		AttList ')' 
{
	$$ = makeTableRegular ($3, $5);	
}

| CREATE TABLE IDENTIFIER '(' 
		AttList ')' AS BPLUSTREE ON IDENTIFIER 
{
	$$ = makeTableBPlusTree ($3, $5, $10);
}

AttList : AttList ',' Att 
{
	$$ = appendAttList ($1, $3);
}

| Att 
{
	$$ = $1;
}
;

Att : IDENTIFIER INT
{
	$$ = makeAttList ($1, INT);
}

| IDENTIFIER DOUBLE
{
	$$ = makeAttList ($1, DOUBLE);
}

| IDENTIFIER STRING
{
	$$ = makeAttList ($1, STRING);
}

| IDENTIFIER BOOL
{
	$$ = makeAttList ($1, BOOL);
}

//********* SELECT-FROM-WHERE Query

SelectQuery: SELECT ValueList
             FROM FromList
	     WHERE CNF
	     GROUP BY ValueList
{
	$$ = makeQueryWithGroupBy ($2, $4, $6, $9);
}

| SELECT ValueList
             FROM FromList
	     WHERE CNF
{
	$$ = makeQuery ($2, $4, $6);
}

| SELECT ValueList
  FROM FromList
{
	$$ = makeQueryNoWhere ($2, $4);
}
;

FromList: IDENTIFIER AS IDENTIFIER ',' FromList
{
	$$ = appendFromList ($5, $1, $3);
}

| IDENTIFIER AS IDENTIFIER
{
	$$ = makeFromList ($1, $3);
}
;

CNF: CNF AND Disjunction
{
	$$ = pushBackDisjunction ($1, $3);	
}

| CNF AND '(' Disjunction ')'
{
	$$ = pushBackDisjunction ($1, $4);	
}

| Disjunction 
{
	$$ = makeCNF ($1);
}

| '(' Disjunction ')'
{
	$$ = makeCNF ($2);
}
;

Disjunction: Disjunction OR Comparison
{
	$$ = orr ($1, $3);
}

| Comparison
{
	$$ = $1;
}
;

Comparison: Value '>' Value
{
	$$ = gt ($1, $3);
}

| Value '<' Value
{
	$$ = lt ($1, $3);
}

| Value '<' '>' Value
{
        $$ = neq ($1, $4);
}

| Value '=' Value
{
	$$ = eq ($1, $3);
}

| NOT Comparison
{
	$$ = nott ($2);
}
;

ValueList: ValueList ',' Value
{
	$$ = pushBackValue ($1, $3);
}

| Value
{
	$$ = makeValueList ($1);
}
;

Value: MultExp '+' Value
{
	$$ = plus ($1, $3);
}

| MultExp '-' Value 
{
	$$ = minus ($1, $3);
}

| MultExp
{
	$$ = $1;
}

| SUM '(' Value ')'
{
	$$ = sum ($3);
}

| AVG '(' Value ')'
{
	$$ = avg ($3);
}
;

MultExp: Literal '*' MultExp
{
	$$ = times ($1, $3);
}

| Literal '/' MultExp
{
	$$ = divide ($1, $3);
}

| Literal 
{
	$$ = $1;
}

Literal: IDENTIFIER '.' IDENTIFIER
{
	$$ = makeIdentifier ($1, $3);
}

| DBL
{
	$$ = makeDouble ($1);
}

| STR 
{
	$$ = makeString ($1);	
}

| INTEGER
{
	$$ = makeInt ($1);
}

| '(' Value ')'
{
	$$ = $2;
}
;

%%


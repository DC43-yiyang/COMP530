/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     IDENTIFIER = 259,
     DBL = 260,
     STR = 261,
     SELECT = 262,
     FROM = 263,
     WHERE = 264,
     AS = 265,
     BY = 266,
     AND = 267,
     OR = 268,
     NOT = 269,
     SUM = 270,
     AVG = 271,
     GROUP = 272,
     INT = 273,
     BOOL = 274,
     BPLUSTREE = 275,
     CREATE = 276,
     DOUBLE = 277,
     STRING = 278,
     ON = 279,
     TABLE = 280
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define IDENTIFIER 259
#define DBL 260
#define STR 261
#define SELECT 262
#define FROM 263
#define WHERE 264
#define AS 265
#define BY 266
#define AND 267
#define OR 268
#define NOT 269
#define SUM 270
#define AVG 271
#define GROUP 272
#define INT 273
#define BOOL 274
#define BPLUSTREE 275
#define CREATE 276
#define DOUBLE 277
#define STRING 278
#define ON 279
#define TABLE 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 13 "/Users/niuyiyang/CODE/Github_code/COMP530/A7/Main/SQL/source/Parser.y"
{
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
}
/* Line 1529 of yacc.c.  */
#line 113 "/Users/niuyiyang/CODE/Github_code/COMP530/A7/Main/SQL/source/Parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif




/***********************************************************************
 * SECTION 1
 ***********************************************************************/
/* The code in %{ %} is included as it is in lex.yy.c file
 * it has C global variables, prototypes, and comments 
 */


%{

#include "Lexer.h"
#include "ParserHelperFunctions.h"
#include "Parser.h"
#include <stdio.h>  // For fileno()
#include <stdlib.h> // For malloc()
#include <string.h> // For strdup()

#define YY_USER_INIT (yylineno = 1)

%}


/******************************************************************************
 * SECTION 2
 ******************************************************************************/
/* This is the DEFINITION section which contains substitutions, code, and
 * start stats; will be copied into lex.yy.c
 */

%option noinput
%option nounput

%option bison-bridge
%option extra-type="struct LexerExtra *"
%option reentrant

/******************************************************************************
 * SECTION 3
 ******************************************************************************/
/* This is the RULES section which defines how to "scan" and what action
 * to take for each token
 */

%%

[Ss][Ee][Ll][Ee][Cc][Tt]	return (SELECT);

[Ff][Rr][Oo][Mm]		return (FROM);

[Ww][Hh][Ee][Rr][Ee]		return (WHERE);

[Gg][Rr][Oo][Uu][Pp]		return (GROUP);

[Bb][Yy]			return (BY);

[Aa][Ss]			return (AS);

[Aa][Nn][Dd]			return (AND);

[Nn][Oo][Tt]			return (NOT);

[Oo][Nn]			return (ON);

[Oo][Rr]			return (OR);

[Ss][Uu][Mm]			return (SUM);

[Aa][Vv][Gg]			return (AVG);

[Cc][Rr][Ee][Aa][Tt][Ee] 	return (CREATE);

[Tt][Aa][Bb][Le][Ee]		return (TABLE);

[Bb][Pp][Ll][Uu][Ss][Tt][Rr][Ee][Ee]	return (BPLUSTREE);

[Ii][Nn][Tt]			return (INT);

[Dd][Oo][Uu][Bb][Ll][Ee] 	return (DOUBLE);

[Ss][Tt][Rr][Ii][Nn][Gg]	return (STRING);

[Bb][Oo][Oo][Ll]		return (BOOL);

"="			return ('=');

"<"			return ('<');

">"			return ('>');

"+"			return ('+');

"-"			return ('-');

"/"    	        	return ('/');

"*"    	        	return ('*');

","    	        	return (',');

")"    	        	return (')');

"("    	        	return ('(');

"\."    	       	return ('.');

[0-9]+			{yylval->myInt = atoi (yytext);
			 return INTEGER;
			}

[+-]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)? {yylval->myDouble = atof (yytext);
  			 return DBL;
			}

[A-Za-z][A-Za-z0-9_-]* {yylval->myChar = strdup (yytext);
  			return IDENTIFIER;
			}     

\"(\\.|[^"])*\"		{yylval->myChar = strdup (yytext);
			return STR;
			}

\n                	++yylineno;

[ \t]             	;

.                 	yyerror(yyscanner, 0, "LEX_ERROR: invalid character");

%%

struct SQLStatement;

void yyerror(yyscan_t scanner, struct SQLStatement **me, const char *s) {
	struct LexerExtra * const extra = yyget_extra (scanner);
	snprintf (extra->errorMessage, sizeof (extra->errorMessage), "error at line %d: %s at %s\n", yyget_lineno (scanner), s, yyget_text (scanner));
}

int yywrap(yyscan_t scanner){
  return 1;
}

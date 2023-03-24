#ifndef _PARSER_H_
#define _PARSER_H_

#include "Lexer.h"

struct SQLStatement;

#ifdef __cplusplus
extern "C"
#endif
int yyparse (yyscan_t, struct SQLStatement **);

#endif	

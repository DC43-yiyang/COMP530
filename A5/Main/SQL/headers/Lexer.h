#ifndef _LEXER_H_
#define _LEXER_H_

#ifdef __cplusplus
extern "C" {
#endif

	struct LexerExtra {
		char errorMessage[500];
	};	  

	typedef void* yyscan_t;
	int yylex_init_extra(struct LexerExtra *, yyscan_t *);
	int yylex_destroy(yyscan_t);

	typedef struct yy_buffer_state *YY_BUFFER_STATE;
	YY_BUFFER_STATE yy_scan_string (const char *, yyscan_t);
	void yy_delete_buffer (YY_BUFFER_STATE, yyscan_t);

	struct SQLStatement;
	void yyerror(yyscan_t, struct SQLStatement **myStatement, const char *);

	union YYSTYPE;
	int yylex(union YYSTYPE *, yyscan_t);


#ifdef __cplusplus
}
#endif

#endif	

%option yylineno

%{
#include <stdio.h>
#include "parser.hpp"
#include <dsb/core/nid.h>
%}

%%
\->						return SYMBOL_DEPGET;
;						return SYMBOL_SEMICOLON;
[ \t]+					/* IGNORE */
\n						/* IGNORE */
\{						return SYMBOL_LBRACE;
\}						return SYMBOL_RBRACE;
\[						return SYMBOL_LSQUARE;
\]						return SYMBOL_RSQUARE;
\(						return SYMBOL_LBRACK;
\)						return SYMBOL_RBRACK;
:=						return SYMBOL_DEFINE;
==						return SYMBOL_COMPARE;
\<=						return SYMBOL_LEQ;
\>=						return SYMBOL_GEQ;
\<\<					return SYMBOL_SHIFTL;
\>\>					return SYMBOL_SHIFTR;
\<						return SYMBOL_LESS;
\>						return SYMBOL_GREATER;
\&						return SYMBOL_AND;
\|						return SYMBOL_OR;
=						return SYMBOL_ASSIGN;
for						return KEYWORD_FOR;
while					return KEYWORD_WHILE;
new						return KEYWORD_NEW;
param					return KEYWORD_PARAM;
return					return KEYWORD_RETURN;
if						return KEYWORD_IF;
else					return KEYWORD_ELSE;
break					return KEYWORD_BREAK;
continue				return KEYWORD_CONTINUE;
\$[a-zA-Z0-9]*			yylval.str = strdup(yytext); return VARIABLE;
\.						return SYMBOL_DOT;
[0-9]+					dsb_iton(atoi(yytext),&yylval.nid); return CONSTANT_NID;
\'.\'					yylval.nid.header = 0; yylval.nid.t = NID_TYPE_CHARACTER; yylval.nid.ll = yytext[1]; return CONSTANT_NID;
[a-zA-Z][a-zA-Z0-9]*	dsb_nid_fromStr(yytext, &yylval.nid); return CONSTANT_NID;
%%

%{
#include <stdio.h>
#include "parser.hpp"
#include <dsb/core/nid.h>
%}

%%
\->						return DEPGET;
;						return SEMICOLON;
[ \t]+					/* IGNORE */
\n						/* IGNORE */
\{						return LBRACE;
\}						return RBRACE;
\[						return LSQUARE;
\]						return RSQUARE;
\(						return LBRACK;
\)						return RBRACK;
:=						return DEFINE;
==						return COMPARE;
\<=						return LEQ;
\>=						return GEQ;
\<\<					return SHIFTL;
\>\>					return SHIFTR;
\<						return LESS;
\>						return GREATER;
\&						return AND;
\|						return OR;
=						return ASSIGN;
for						return FOR;
while					return WHILE;
new						return NEW;
param					return PARAM;
return					return RETURN;
\$[a-zA-Z0-9]*			yylval.str = strdup(yytext); return VARIABLE;
\.						return DOT;
[0-9]+					dsb_iton(atoi(yytext),&yylval.nid); return NIDNUM;
\'.\'					yylval.nid.header = 0; yylval.nid.t = NID_TYPE_CHARACTER; yylval.nid.ll = yytext[1]; return NIDCHAR;
[a-zA-Z][a-zA-Z0-9]*	dsb_nid_fromStr(yytext, &yylval.nid); return NIDWORD;
%%

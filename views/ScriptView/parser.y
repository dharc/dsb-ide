%{
#include <stdio.h>
#include <string.h>
#include <dsb/core/nid.h>

extern "C"
{
	int yyparse(void);
	//
	int yywrap()
	{
		return 1;
	}
}

// MUST BE INITIALISED to 0
int yydsbvarid;

int yylex(void);

void yyerror(const char *str)
{
	fprintf(stderr, "error: %s\n", str);
}

%}

%token DEPGET SEMICOLON LBRACK RBRACK LBRACE RBRACE LSQUARE RSQUARE DEFINE COMPARE GEQ LEQ SHIFTL SHIFTR LESS GREATER AND OR ASSIGN FOR WHILE NEW PARAM RETURN DOT

%union
{
	char *str;
	NID_t nid;
}

%token <nid> NIDNUM;
%token <nid> NIDCHAR;
%token <str> VARIABLE;
%token <nid> NIDWORD;

%type <nid> path;
%type <nid> nid;
%type <nid> expression;
%type <nid> codeblock;

%%

statements:
	|
	statement SEMICOLON statements
	;

nid:
	NIDNUM
	|
	NIDCHAR
	|
	NIDWORD
	;


statement:
	RETURN path
	{
		printf("ret $_%d\n",yydsbvarid-1);
		yydsbvarid -= 1;
	}
	|
	RETURN nid
	{
		char tmp1[100];
		dsb_nid_toStr(&$2, tmp1, 100);
		printf("ret %s\n",tmp1);
	}
	|
	RETURN VARIABLE
	{
		printf("ret %s\n",$2);
	}
	|
	path DOT nid ASSIGN expression
	{
		char tmp2[100];
		dsb_nid_toStr(&$3, tmp2, 100);
		printf("set $_%d %s $_%d\n",yydsbvarid-2,tmp2,yydsbvarid-1);
		yydsbvarid -= 2;
	}
	|
	path DOT nid ASSIGN nid
	{
		char tmp2[100];
		char tmp3[100];
		dsb_nid_toStr(&$3, tmp2, 100);
		dsb_nid_toStr(&$5, tmp3, 100);
		printf("set $_%d %s %s\n",yydsbvarid-1,tmp2,tmp3);
		yydsbvarid -= 1;
	}
	|
	path DOT VARIABLE ASSIGN expression
	{
		char tmp3[100];
		dsb_nid_toStr(&$5, tmp3, 100);
		printf("set $_%d %s %s\n",yydsbvarid-1,$3,tmp3);
		yydsbvarid -= 1;
	}
	|
	path DOT nid DEFINE expression
	{
		char tmp1[100];
		char tmp2[100];
		char tmp3[100];
		dsb_nid_toStr(&$1, tmp1, 100);
		dsb_nid_toStr(&$3, tmp2, 100);
		dsb_nid_toStr(&$5, tmp3, 100);
		printf("Define %s.%s := %s\n",tmp1,tmp2,tmp3);
	}
	|
	VARIABLE ASSIGN expression
	{
		char tmp1[100];
		dsb_nid_toStr(&$3, tmp1, 100);
		printf("cpy %s %s\n",$1,tmp1);
	}
	;
	
subpath:
	LBRACK path RBRACK
	;

path:
	path DOT subpath
	{
		printf("get $_%d $_%d $_%d\n",yydsbvarid-2,yydsbvarid-2,yydsbvarid-1);
		yydsbvarid -= 1;
	}
	|
	nid DOT nid
	{
		char tmp1[100];
		char tmp2[100];
		dsb_nid_toStr(&$1, tmp1, 100);
		dsb_nid_toStr(&$3, tmp2, 100);
		printf("get $_%d %s %s\n",yydsbvarid,tmp1,tmp2);
		yydsbvarid++;
	}
	|
	path DOT nid
	{
		char tmp2[100];
		dsb_nid_toStr(&$3, tmp2, 100);
		printf("get $_%d $_%d %s\n",yydsbvarid-1,yydsbvarid-1,tmp2);
	}
	;

expression:
	LBRACE codeblock RBRACE
	{
		char tmp3[100];
		dsb_nid_toStr(&$2, tmp3, 100);
		printf("GenDef %s\n",tmp3);
	}
	|
	path DOT nid
	{
		char tmp2[100];
		dsb_nid_toStr(&$3, tmp2, 100);
		printf("get $_%d $_%d %s\n",yydsbvarid-1,yydsbvarid-1,tmp2);
	}
	;

codeblock:
	nid
	;

%%

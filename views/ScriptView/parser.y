%{
#include <stdio.h>
#include <string.h>
#include <dsb/core/nid.h>
#include <dsb/core/vm.h>
#include <malloc.h>

#define CODEBLOCK_SIZE			100
#define FIRST_INTERNAL_VAR		16

struct CodeBlock
{
	struct CodeBlock *prev;
	struct CodeBlock *next;
	NID_t code[CODEBLOCK_SIZE];
	int lines[CODEBLOCK_SIZE];
	int ip;
	int lastvar;
};

static struct CodeBlock *create_block()
{
	struct CodeBlock *block;
	block = (struct CodeBlock*)malloc(sizeof(struct CodeBlock));
	block->ip = 0;
	block->prev = 0;
	block->next = 0;
	block->lastvar = FIRST_INTERNAL_VAR;
	return block;
}

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

%locations

%token SYMBOL_DEPGET
%token SYMBOL_SEMICOLON
%token SYMBOL_LBRACK
%token SYMBOL_RBRACK
%token SYMBOL_LBRACE
%token SYMBOL_RBRACE
%token SYMBOL_LSQUARE
%token SYMBOL_RSQUARE
%token SYMBOL_DEFINE
%token SYMBOL_COMPARE
%token SYMBOL_GEQ
%token SYMBOL_LEQ
%token SYMBOL_SHIFTL
%token SYMBOL_SHIFTR
%token SYMBOL_LESS
%token SYMBOL_GREATER
%token SYMBOL_AND
%token SYMBOL_OR
%token SYMBOL_ASSIGN
%token SYMBOL_DOT
%token KEYWORD_FOR
%token KEYWORD_WHILE
%token KEYWORD_NEW
%token KEYWORD_PARAM
%token KEYWORD_RETURN
%token KEYWORD_IF
%token KEYWORD_ELSE
%token KEYWORD_BREAK
%token KEYWORD_CONTINUE

%union
{
	char *str;
	NID_t nid;
	struct CodeBlock *block;
}

%token <nid> CONSTANT_NID;
%token <str> VARIABLE;

%type <block> statement_list
%type <block> statement
%type <block> return_statement
%type <block> modi_statement
%type <block> graph_assign_statement
%type <block> graph_define_statement
%type <block> var_assign_statement
%type <block> param_statement
%type <block> path;
%type <block> subpath;
%type <block> base_path;
%type <block> expression;
%type <block> codeblock;

%start script
%%

script
	: statement_list
	  {
		/* Merge and complete the parse */
	  }
	;

statement_list
	: statement
	  {
		/* Pass the codeblock on */
		$$ = $1;
	  }
	| statement_list statement
	  {
		/* Combine code blocks */
		if ($1 != 0) $1->next = $2;
		if ($2 != 0) $2->prev = $1;
		$$ = ($1 == 0) ? $2 : $1;
	  }
	;


statement
	: return_statement
	  {
		$$ = $1;
	  }
	| modi_statement
	  {
	    $$ = $1;
	  }
	| param_statement
	  {
	    $$ = 0;
	  }
	//| if_statement
	//  {
	//    $$ = $1;
	//  }
	//| while_statement
	//  {
	//    $$ = $1;
	//  }
	;
	

param_statement
	: KEYWORD_PARAM VARIABLE SYMBOL_SEMICOLON
	  {
	  	$$ = create_block();
	  }
	;

return_statement
	: path SYMBOL_SEMICOLON										//Return a . or -> path
	  {
		$$ = $1;												//Pass on codeblock
		$$->lines[$$->ip] = @2.first_line;						//Save line number
		dsb_nid_op(VM_RET($$->lastvar), &$$->code[$$->ip++]);	//RET bytecode appended to path block.
	  }
	| CONSTANT_NID SYMBOL_SEMICOLON								//Return a constant NID
	  {
	  	$$ = create_block();
	  	$$->lines[$$->ip] = @2.first_line;						//Save line number
	  	dsb_nid_op(VM_RET(0), &$$->code[$$->ip++]);				//RET bytecode, constant form.
	  	$$->code[$$->ip++] = $1;								//Emit constant into the code.
	  }
	| VARIABLE SYMBOL_SEMICOLON									//Return value of a variable.
	  {
		$$ = create_block();
		$$->lines[$$->ip] = @2.first_line;						//Save line number
		//TODO Lookup variable. Must exist already...
		dsb_nid_op(VM_RET(0), &$$->code[$$->ip++]);				//RET bytecode with specified variable.
	  }
	;
	
	
modi_statement
	: graph_assign_statement
	  {
	    $$ = $1;
	  }
	| graph_define_statement
	  {
	    $$ = $1;
	  }
	| var_assign_statement
	  {
	    $$ = $1;
	  }
	;


graph_assign_statement
	: path SYMBOL_DOT CONSTANT_NID SYMBOL_ASSIGN expression SYMBOL_SEMICOLON
	  {
	  	$$ = $1;
	  	$$->lines[$$->ip] = @4.first_line;									//Save line number
	  	dsb_nid_op(VM_SET($1->lastvar,0,$5->lastvar), &$$->code[$$->ip++]);	//SET bytecode with var,const,var.
	  	$$->code[$$->ip++] = $3;											//Insert constant nid.
	  }
	| path SYMBOL_DOT CONSTANT_NID SYMBOL_ASSIGN CONSTANT_NID SYMBOL_SEMICOLON
	  {
	    $$ = $1;
	  	$$->lines[$$->ip] = @4.first_line;									//Save line number
	  	dsb_nid_op(VM_SET($1->lastvar,0,0), &$$->code[$$->ip++]);			//SET bytecode with var,const,const.
	  	$$->code[$$->ip++] = $3;											//Insert first constant nid.
	  	$$->code[$$->ip++] = $5;											//Insert second constant nid.
	  }
	| path SYMBOL_DOT VARIABLE SYMBOL_ASSIGN expression SYMBOL_SEMICOLON
	  {
	  	$$ = $1;
	  	$$->lines[$$->ip] = @4.first_line;									//Save line number
	  	//TODO Lookup variable name.
	    dsb_nid_op(VM_SET($1->lastvar,0,$5->lastvar), &$$->code[$$->ip++]);	//SET bytecode with var,const,const.
	  }
	;
	

graph_define_statement
	:
	;
	
var_assign_statement
	: VARIABLE SYMBOL_ASSIGN CONSTANT_NID SYMBOL_SEMICOLON
	  {
	  	$$ = 0;
	  }
	;
	
	
subpath
	: SYMBOL_LBRACK path SYMBOL_RBRACK
	  {
	    $$ = $2;
	  }
	;


base_path
	: CONSTANT_NID SYMBOL_DOT CONSTANT_NID
	  {
	  	$$ = create_block();										//New codeblock
	  	$$->lines[$$->ip] = @2.first_line;							//Save line number
	  	dsb_nid_op(VM_GET($$->lastvar,0,0),&$$->code[$$->ip++]);	//GET bytecode with var,const,const
	  	$$->code[$$->ip++] = $1;									//Insert first constant
	  	$$->code[$$->ip++] = $3;									//Insert second constant
	  }
	| VARIABLE SYMBOL_DOT CONSTANT_NID
	  {
		$$ = create_block();										//New codeblock
	  	$$->lines[$$->ip] = @2.first_line;							//Save line number
	  	//TODO Get variable number.
	  	dsb_nid_op(VM_GET($$->lastvar,0,0),&$$->code[$$->ip++]);	//GET bytecode with var,const,const
	  	$$->code[$$->ip++] = $3;									//Insert constant
	
	  }
	;
	
	
path
	: base_path
	  {
	    $$ = $1;
	  }
	| path SYMBOL_DOT CONSTANT_NID
	  {
	  	$$ = $1;
	  	$$->lines[$$->ip] = @2.first_line;										//Save line number
	  	dsb_nid_op(VM_GET($$->lastvar, $$->lastvar, 0), &$$->code[$$->ip++]);	//GET bytecode with var,var,const
	  	$$->code[$$->ip++] = $3;												//Insert constant
	  }
	| path SYMBOL_DOT VARIABLE
	  {
	  	$$ = $1;
	  }
	;

expression
	: SYMBOL_LBRACE codeblock SYMBOL_RBRACE
	  {
		$$ = $2;
	  }
	| path
	  {
		$$ = $1;
	  }
	;

codeblock
	: CONSTANT_NID
	{
		
	}
	;

%%

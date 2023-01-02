%{
	#include "parse.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	static Node * root;
	int yylex();
	int yyerror(char*);
%}
%token VAR IF GOTO PARA CALL END RET FUNC SYMBOL LABEL IMM
%token ADD SUB MUL DIV MOD ASG NOT LTH GTH LEQ GEQ AND OR EQU NEQ LBM RBM COL

%%
Root:
      Programs  {$$ = makeNode("Root", $1, 0); root = $$;}
    ;
Programs:
      {$$=NULL;}
    | Program Programs {$$=$1, $$->sib=$2;}
    ;
Program:
      Declaration {$$=$1;}
    | Initialization {$$=$1;}
    | FunctionDef {$$=$1;}
    ;
Declaration:
      VAR SYMBOL {$$=makeNode("Decl_int", $2, 1);}
    | VAR IMM SYMBOL {$$=makeNode("Decl_int[]", $2, 2); $2->sib=$3;}
    ;
Initialization:
      SYMBOL ASG IMM {$$=makeNode("Init_int", $1, 3); $1->sib=$3;}
    | SYMBOL LBM IMM RBM ASG IMM {$$=makeNode("Init_int[]", $1, 4); $1->sib=$3; $3->sib=$6;}
    ;
FunctionDef:
      FunctionHeader Statements FunctionEnd {$$=makeNode("f_def", $1, 5); $1->sib=$2; $2->sib=$3;}
    ;
FunctionHeader:
      FUNC LBM IMM RBM {$$=makeNode("f_header", $1, 6); $1->sib=$3;}
    ;
Statements:
      {$$=NULL;}
    | Statement Statements {$$=makeNode("state", $1, 7); $1->sib=$2;}
    ;
Statement:
      Expression {$$=$1;}
    | Declaration {$$=$1;}
    ;
FunctionEnd:
      END FUNC {$$=makeNode("f_end", $2, 8);}
    ;
Expression:
      SYMBOL ASG RightValue BinOp RightValue {$$=makeNode("=bop", $1, 9); $1->sib=$3; $3->sib=$4; $4->sib=$5;}
    | SYMBOL ASG Op RightValue {$$=makeNode("=op", $1, 10); $1->sib=$3; $3->sib=$4;}
    | SYMBOL ASG RightValue {$$=makeNode("=", $1, 11); $1->sib=$3;}
    | SYMBOL LBM RightValue RBM ASG RightValue {$$=makeNode("=[]", $1, 12); $1->sib=$3; $3->sib=$6;}
    | SYMBOL ASG SYMBOL LBM RightValue RBM {$$=makeNode("=v[]", $1, 13); $1->sib=$3; $3->sib=$5;}
    | IF RightValue LogicOp RightValue GOTO LABEL {$$=makeNode("if_goto", $2, 14); $2->sib=$3; $3->sib=$4; $4->sib=$6;}
    | GOTO LABEL {$$=makeNode("goto", $2, 15);}
    | LABEL COL {$$=makeNode("l", $1, 16);}
    | PARA RightValue {$$=makeNode("param", $2, 17);}
    | CALL FUNC {$$=makeNode("call_f", $2, 18);}
    | SYMBOL ASG CALL FUNC {$$=makeNode("=call_f", $1, 19); $1->sib=$4;}
    | RET RightValue {$$=makeNode("ret_v", $2, 20);}
    | RET {$$=makeNode("ret", NULL, 21);}
    ;
RightValue:
      SYMBOL {$$=$1;}
    | IMM {$$=$1;}
    ;
BinOp:
      Op {$$=$1;}
    | LogicOp {$$=$1;}
    ;
Op:
      ADD {$$=makeNode("+", NULL, 22);}
    | SUB {$$=makeNode("-", NULL, 23);}
    | MUL {$$=makeNode("*", NULL, 24);}
    | DIV {$$=makeNode("/", NULL, 25);}
    | MOD {$$=makeNode("%", NULL, 26);}
    | NOT {$$=makeNode("!", NULL, 27);}
    ;
LogicOp:
      LEQ {$$=makeNode("<=", NULL, 28);}
    | GEQ {$$=makeNode(">=", NULL, 29);}
    | EQU {$$=makeNode("==", NULL, 30);}
    | NEQ {$$=makeNode("!=", NULL, 31);}
    | LTH {$$=makeNode("<", NULL, 32);}
    | GTH {$$=makeNode(">", NULL, 33);}
    | AND {$$=makeNode("&&", NULL, 34);}
    | OR {$$=makeNode("||", NULL, 35);}
    ;
%%

int main(int argc, char* argv[])
{
    // for(int i=1;i<argc;++i){
    //     if(strcmp(argv[i],"-S")==0){
    //       continue;
    //     }
    //     if(strcmp(argv[i],"-e")==0){
    //       continue;
    //     }
    //     if(strcmp(argv[i],"-o")==0){
    //       outputfile=fopen(argv[i+1],"w");
    //       ++i;
    //       continue;
    //     }
    //     inputfile=fopen(argv[i],"r");
    // }
    // stdin=inputfile;
    // stdout=outputfile;
    yyparse();
    scanTree(root);
    return 0;
}
int yyerror(char* error){
    fprintf(stderr, "syntax error: %s \n", error);
    exit(0);
}
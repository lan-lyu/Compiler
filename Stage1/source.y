%{
	#include "parse.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	static Node * root;
	int yylex();
	int yyerror(char*);
%}
%token INT VOID CONST IF ELSE WHILE BREAK CONTINUE RETURN IMM IDENT ADD SUB MUL DIV MOD
%token ASG NOT LTH GTH LEQ GEQ AND OR EQU NEQ LBS RBS LBM RBM LBL RBL COM SEM

%%
Root:
      CompUnit  {$$ = makeNode("Root", $1, 0); root = $$;}
    ;
CompUnit:
      Decl      {$$=$1;}
    | FuncDef   {$$=$1;}
    | Decl CompUnit     {$$=$1; $$->sib=$2;}
    | FuncDef CompUnit  {$$=$1; $$->sib=$2;}
    ;
Decl:
      ConstDecl {$$=$1;}
    | VarDecl   {$$=$1;}
    ;
ConstDecl:
      CONST INT ConstDefs SEM  {$$=makeNode("ConstDecl", $3, 1);}
    ;
ConstDefs:
      ConstDef  {$$=$1;}
    | ConstDef COM ConstDefs   {$$=$1; $$->sib=$3;}
    ;
ConstDef:
      IDENT ArrayBm ASG InitVals {$$=makeNode("DefArray", $1, 2); $1->sib=$2; rightMost($1)->sib=$4;}
    | IDENT ASG InitVal {$$=makeNode("DefVar", $1, 3); $1->sib=$3;}
    ;
ArrayBm:
      LBM ConstExp RBM {$$=makeNode("ArrayIdx", $2, 4);}
    | LBM ConstExp RBM ArrayBm {$$=makeNode("ArrayIdx", $2, 5); $$->sib=$4;}
    ;
VarDecl:
      INT VarDefs SEM {$$=makeNode("VarDecl", $2, 6);}
    ;
VarDefs:
      VarDef {$$=$1;}
    | VarDef COM VarDefs {$$=$1; $$->sib=$3;}
    ;
VarDef:
      IDENT {$$=makeNode("int", $1, 7);}
    | IDENT ASG InitVal {$$=makeNode("DefVar", $1, 8); $1->sib = $3;}
    | IDENT ArrayBm {$$=makeNode("DeclArray", $1, 9); $1->sib=$2;}
    | IDENT ArrayBm ASG InitVal {$$=makeNode("DefArray", $1, 10); $1->sib=$2; rightMost($1)->sib=$4;}
    ;
InitVal:
      Exp {$$=makeNode("InitVal_Exp", $1, 11);}
    | LBL RBL {$$=makeNode("InitVal_BB", NULL, 12);}
    | LBL InitVals RBL {$$=makeNode("InitVal_BvB", $2, 13);}
    ;
InitVals:
      InitVal {$$=$1;}
    | InitVal COM InitVals {$$=$1; $$->sib=$3;}
    ;
FuncDef:
      INT IDENT LBS RBS Block {$$=makeNode("func_int", $2, 14); $2->sib=$5;}
    | VOID IDENT LBS RBS Block {$$=makeNode("func_void", $2, 15); $2->sib=$5;}
    | INT IDENT LBS FuncFParams RBS Block {$$=makeNode("func_int_p", $2, 16); $2->sib=$4; rightMost($2)->sib=$6;}
    | VOID IDENT LBS FuncFParams RBS Block {$$=makeNode("func_void_p", $2, 17); $2->sib=$4; rightMost($2)->sib=$6;}
    ;
FuncFParam:
      INT IDENT LBM RBM ArrayBm {$$=makeNode("pArrays", $2, 18); $2->sib=$5;}
    | INT IDENT LBM RBM {$$=makeNode("pArray", $2, 19);}
    | INT IDENT {$$=makeNode("p", $2, 20);}
    ;
FuncFParams:
      FuncFParam {$$=$1;}
    | FuncFParam COM FuncFParams {$$=$1; $$->sib=$3;}
    ;
Block:
      LBL BlockItem RBL {$$=makeNode("blk", $2, 21);}
    ;
BlockItem:
       {$$=NULL;}
    | Decl BlockItem {$$=$1; $1->sib=$2;}
    | Stmt BlockItem {$$=$1; $1->sib=$2;}
    ;
Stmt:
      LVal ASG Exp SEM {$$=makeNode("=", $1, 22); $1->sib=$3;}
    | SEM {$$=makeNode("sem", NULL, 23);}
    | Exp SEM {$$=makeNode("exp", $1, 24);}
    | Block {$$=$1;}
    | IF LBS Cond RBS Stmt {$$=makeNode("if", $3, 25); $3->sib=$5;}
    | IF LBS Cond RBS Stmt ELSE Stmt {$$=makeNode("ifel", $3, 26); $3->sib=$5; $5->sib=$7;}
    | WHILE LBS Cond RBS Stmt {$$=makeNode("while", $3, 27); $3->sib=$5;}
    | BREAK SEM {$$=makeNode("break", NULL, 28);}
    | CONTINUE SEM {$$=makeNode("continue", NULL, 29);}
    | RETURN SEM {$$=makeNode("r", NULL, 30);}
    | RETURN Exp SEM {$$=makeNode("rExp", $2, 31);}
    ;
Exp:
      AddExp {}
    ;
Cond:
      LOrExp {$$=$1;}
    ;
LVal:
      IDENT {$$=makeNode("lval", $1, 32);}
    | IDENT ArrayBm {$$=makeNode("lvalArray", $1, 33); $1->sib=$2;}
    ;
PrimaryExp:
      LBS Exp RBS {$$=$2;}
    | LVal {$$=$1;}
    | Number {$$=$1;}
    ;
Number:
      IMM {$$=$1;}
    ;
UnaryExp:
      PrimaryExp {$$=$1;}
    | IDENT LBS RBS {$$=makeNode("Unary_func", $1, 34);}
    | IDENT LBS FuncRParams RBS {$$=makeNode("Unary_func", $1, 35); $1->sib=$3;}
    | ADD UnaryExp {$$=makeNode("+", $2, 36);}
    | SUB UnaryExp {$$=makeNode("-", $2, 37);}
    | NOT UnaryExp {$$=makeNode("!", $2, 38);}
    ;
FuncRParams:
      Exp {$$=$1;}
    | Exp COM FuncRParams {$$=$1; $$->sib=$3;}
    ;
MulExp:
      UnaryExp {$$=$1;}
    | MulExp MUL UnaryExp {$$=makeNode("*", $1, 39); $1->sib=$3;}
    | MulExp DIV UnaryExp {$$=makeNode("/", $1, 40); $1->sib=$3;}
    | MulExp MOD UnaryExp {$$=makeNode("%", $1, 41); $1->sib=$3;}
    ;
AddExp:
      MulExp {$$=$1;}
    | AddExp ADD MulExp {$$=makeNode("+", $1, 42); $1->sib=$3;}
    | AddExp SUB MulExp {$$=makeNode("-", $1, 43); $1->sib=$3;}
    ;
RelExp:
      AddExp{$$=$1;}
    | RelExp LTH AddExp {$$=makeNode("<", $1, 44); $1->sib=$3;}
    | RelExp GTH AddExp {$$=makeNode(">", $1, 45); $1->sib=$3;}
    | RelExp LEQ AddExp {$$=makeNode("<=", $1, 46); $1->sib=$3;}
    | RelExp GEQ AddExp {$$=makeNode(">=", $1, 47); $1->sib=$3;}
    ;    
EqExp:
      RelExp {$$=$1;}
    | EqExp EQU RelExp {$$=makeNode("==", $1, 48); $1->sib=$3;}
    | EqExp NEQ RelExp {$$=makeNode("!=", $1, 49); $1->sib=$3;}
    ;
LAndExp:
      EqExp {$$=$1;}
    | LAndExp AND EqExp {$$=makeNode("&&", $1, 50); $1->sib=$3;}
    ;
LOrExp:
      LAndExp {$$=$1;}
    | LOrExp OR LAndExp {$$=makeNode("||", $1, 51); $1->sib=$3;}
    ;
ConstExp:
      AddExp {$$=$1;}
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
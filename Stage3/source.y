%{
	#include "parse.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
  int global;
	int yylex();
	int yyerror(char*);
%}
%token STORE IF GOTO CALL MALLOC END LOADADDR LOAD RET VAR REG FUNC LABEL IMM
%token MIN ASG NOT OP LBM RBM COL LOP

%%
Root:
    Program {}
  ;
Program:
    {}
  | GlobalVarDecl Program {}
  | FunctionDef Program {}
  ;
GlobalVarDecl:
    VAR ASG IMM {makeEXPR(0, "d_=", NULL, $3, NULL, $1);}
  | VAR ASG MALLOC IMM {makeEXPR(1, "d_malloc", NULL, $4, NULL, $1);}
  ;
FunctionDef:
    FunctionHeader Expressions FunctionEnd {}
  ;
FunctionHeader:
    FUNC LBM IMM RBM LBM IMM RBM {makeEXPR(2, "f_header", $1, $3, $6, NULL);}
  ;
Expressions:
     {}
  | Expression Expressions{}
  ;
FunctionEnd:
    END FUNC {makeEXPR(3, "f_end", $2, NULL, NULL, NULL);}
  ;
Expression:
    REG ASG REG OP REG {makeEXPR(4, "rOr", $4, $3, $5, $1);}
  | REG ASG REG LOP REG {makeEXPR(4, "rOr", $4, $3, $5, $1);}
  | REG ASG REG OP IMM {makeEXPR(5, "rOi", $4, $3, $5, $1);}
  | REG ASG REG LOP IMM {makeEXPR(5, "rOi", $4, $3, $5, $1);}
  | REG ASG OP REG {makeEXPR(6, "Or", $3, $4, NULL, $1);}
  | REG ASG REG {makeEXPR(7, "r", NULL, $3, NULL, $1);}
  | REG ASG IMM {makeEXPR(8, "i", NULL, $3, NULL, $1);}
  | REG LBM IMM RBM ASG REG {makeEXPR(9, "r[]=", NULL, $3, $6, $1);}
  | REG ASG REG LBM IMM RBM {makeEXPR(10, "=r[]", NULL, $3, $5, $1);}
  | IF REG LOP REG GOTO LABEL {makeEXPR(11, "if_goto", $3, $2, $4, $6);}
  | GOTO LABEL {makeEXPR(12, "goto", $2, NULL, NULL, NULL);}
  | LABEL COL {makeEXPR(13, "label", $1, NULL, NULL, NULL);}
  | CALL FUNC {makeEXPR(14, "call", $2, NULL, NULL, NULL);}
  | RET {makeEXPR(15, "ret", NULL, NULL, NULL, NULL);}
  | STORE REG IMM {makeEXPR(16, "store", $2, $3, NULL, NULL);}
  | LOAD IMM REG {makeEXPR(17, "l_i", $2, $3, NULL, NULL);}
  | LOAD VAR REG {makeEXPR(18, "l_r", $2, $3, NULL, NULL);}
  | LOADADDR IMM REG {makeEXPR(19, "la_i", $2, $3, NULL, NULL);}
  | LOADADDR VAR REG {makeEXPR(20, "la_r", $2, $3, NULL, NULL);}
  ;

%%

int main(int argc, char* argv[])
{
    for(int i=1;i<argc;++i){
        if(strcmp(argv[i],"-S")==0){
          continue;
        }
        if(strcmp(argv[i],"-e")==0){
          continue;
        }
        if(strcmp(argv[i],"-o")==0){
          outputfile=fopen(argv[i+1],"w");
          ++i;
          continue;
        }
        inputfile=fopen(argv[i],"r");
    }
    stdin=inputfile;
    stdout=outputfile;
    
    exprCnt = 0;
    yyparse();
    scanExpr();
    return 0;
}
int yyerror(char* error){
    fprintf(stderr, "syntax error: %s \n", error);
    exit(0);
}
%{
	#include "parse.h"
	#include "source.tab.h"
%}

%x COMMENT
integer -?[1-9][0-9]*|"0"
letter [a-zA-Z]
variable v{integer}
register (s|t|a|x){integer}
label l[0-9]+
fname f\_[a-zA-Z0-9_]+ 
space [ \t]+
enter [\n]
%%

"//".*      	;
"/*"           	{BEGIN COMMENT;}
<COMMENT>"*/"   {BEGIN 0;}
<COMMENT>.      {}
<COMMENT>\n     {}

store		{return STORE;}
if 			{return IF;}
goto        {return GOTO;}
call        {return CALL;}
malloc		{return MALLOC;}
end         {return END;}
loadaddr	{return LOADADDR;}
load		{return LOAD;}
return 		{return RET;}

{variable}		{yylval = strdup(yytext); return VAR;}
{register}		{yylval = strdup(yytext); return REG;}
{fname}         {yylval = strdup(yytext+2); return FUNC;}
{label}         {yylval = strdup(yytext); return LABEL;}
{integer}		{yylval = strdup(yytext); return IMM;}

"-"			{yylval = strdup(yytext); return OP;}
"!"			{yylval = strdup(yytext); return OP;}
"="			{return ASG;}
"<"			{yylval = strdup(yytext); return LOP;}
">"			{yylval = strdup(yytext); return LOP;}
"<="		{yylval = strdup(yytext); return LOP;}
">="		{yylval = strdup(yytext); return LOP;}
"&&"		{yylval = strdup(yytext); return LOP;}
"||"		{yylval = strdup(yytext); return LOP;}
"=="		{yylval = strdup(yytext); return LOP;}
"!="		{yylval = strdup(yytext); return LOP;}
"+"			{yylval = strdup(yytext); return OP;}
"*"			{yylval = strdup(yytext); return OP;}
"/"			{yylval = strdup(yytext); return OP;}
"%"			{yylval = strdup(yytext); return OP;}
"["			{return LBM;}
"]"			{return RBM;}
":"         {return COL;}

{space} 	{}
{enter}		{}

%%

int yywrap()
{
	return 1;
}

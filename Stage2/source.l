%{
	#include "parse.h"
	#include "source.tab.h"
%}

%x COMMENT
integer -?[1-9][0-9]*|"0"
label l[0-9]+
fname f\_[a-zA-Z0-9_]+ 
symbol [Ttp][0-9]+
vret return[ \t\r]*[\n]
space [ \t]+
enter [\n]
%%

"//".*      	;
"/*"           	{BEGIN COMMENT;}
<COMMENT>"*/"   {BEGIN 0;}
<COMMENT>.      {}
<COMMENT>\n     {}

var         {return VAR;}
if 			{return IF;}
goto        {return GOTO;}
param       {return PARA;}
call        {return CALL;}
end         {return END;}
return 		{return RET;}

{fname}         {yylval = makeNode(strdup(yytext), NULL, 36); return FUNC;}
{symbol} 		{yylval = makeNode(strdup(yytext), NULL, 37); return SYMBOL;}
{label}         {yylval = makeNode(strdup(yytext), NULL, 38); return LABEL;}
{integer}		{yylval = makeNode(strdup(yytext), NULL, 39); return IMM;}

"+"			{return ADD;}
"-"			{return SUB;}
"*"			{return MUL;}
"/"			{return DIV;}
"%"			{return MOD;}
"="			{return ASG;}
"!"			{return NOT;}
"<"			{return LTH;}
">"			{return GTH;}
"<="		{return LEQ;}
">="		{return GEQ;}
"&&"		{return AND;}
"||"		{return OR;}
"=="		{return EQU;}
"!="		{return NEQ;}
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

%{
	#include "parse.h"
	#include "source.tab.h"
	int c2i(char ch){
        if(ch>='0'&&ch<='9')
            return ch - '0';
        if(ch>='A'&&ch<='F')
                return ch-'A'+10;
        if(ch>='a'&&ch<='f')
                return ch-'a'+10;
        return -1;
	}
	int oct2dec(const char *oct){
        int len;
        int num = 0;
        int temp;
        int bits;
        int i;
        len = strlen(oct);
        for (i=0, temp=0; i<len; i++, temp=0)
        {
                temp = c2i( *(oct + i) );
                bits = (len - i - 1) * 3;
                temp = temp << bits;
                num = num | temp;
        }
        return num;
	}
	int hex2dec(const char *hex){
        int len;
        int num = 0;
        int temp;
        int bits;
        int i;
        len = strlen(hex);
        for (i=0, temp=0; i<len; i++, temp=0)
        {
                temp = c2i( *(hex + i) );
                bits = (len - i - 1) * 4;
                temp = temp << bits;
                num = num | temp;
        }
        return num;
	}
%}

%x COMMENT
integer [1-9][0-9]*|"0"
octint 0[0-7]+
hexint 0(x|X)[0-9a-fA-F]+
letter [a-zA-Z]
iden (_|{letter})(_|[0-9]|{letter})*
space [ \t]+
enter [\n]
%%

"//".*      	;
"/*"           	{BEGIN COMMENT;}
<COMMENT>"*/"   {BEGIN 0;}
<COMMENT>.      {}
<COMMENT>\n     {}

int 		{return INT;}
void 		{return VOID;}
const       {return CONST;}

if 			{return IF;}
else 		{return ELSE;}
while 		{return WHILE;}
break		{return BREAK;}
continue	{return CONTINUE;}
return 		{return RETURN;}

{integer}		{yylval = makeNode(strdup(yytext), NULL, 52); return IMM;}
{octint}		{int tmp_int = oct2dec(yytext+1); char tmp_buf[10] = {'\0'}; sprintf(tmp_buf, "%d", tmp_int); yylval = makeNode(strdup(tmp_buf), NULL, 52); return IMM;}
{hexint}		{int tmp_int = hex2dec(yytext+2); char tmp_buf[10] = {'\0'}; sprintf(tmp_buf, "%d", tmp_int); yylval = makeNode(strdup(tmp_buf), NULL, 52); return IMM;}
{iden} 			{yylval = makeNode(strdup(yytext), NULL, 53); return IDENT;}

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
"("			{return LBS;}
")"			{return RBS;}
"["			{return LBM;}
"]"			{return RBM;}
"{"			{return LBL;}
"}"			{return RBL;}
","			{return COM;}
";"			{return SEM;}


{space} 	{}
{enter}		{}

%%

int yywrap()
{
	return 1;
}

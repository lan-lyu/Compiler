#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>

typedef struct EXPRS
{
	int index;
	int type;
	const char* attr;
	const char* op;
	const char* src1;
	const char* src2;
	const char* dst;
}Expr;

static int exprCnt;

FILE* inputfile, *outputfile;
extern char* strdup(const char*);

void makeEXPR(int type, const char*, const char*, const char*, const char*, const char*);
void scanExpr();

#define YYSTYPE const char*
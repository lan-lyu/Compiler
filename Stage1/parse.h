#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum _node_type
{
	_vdcl = 0,

};

struct NODES;
typedef struct NODES
{
	int type;
	struct NODES* son;	//left son
	struct NODES* sib;	//right sibling
	const char* attr;	//attribute
}Node;

struct VARAS;
typedef struct VARAS
{
	char* oriName;	//original variable name
	char* yoreName;	//transferred variable name
	int valid; 		//check if valid in this environment
	int dimension;	//array dimension
	int* dimensize;	//each dimension size
	int size;		//whole size
	int* val;		//values
}Variable;

struct PARAM;
typedef struct PARAM
{
	char* oriName;
	char* yoreName;
	char* funcName;
	int dimension;
	int* dimesize;
}Para;

struct FUNC;
typedef struct FUNC
{
	char* funcName;		//function name
	int paramNum;
	int returnInt;
	int paramIdx;
	// char* yoreName;
	// Para* params;
}Func;

struct CALRESULT;
typedef struct CALRESULT
{
	int value;	// return value
	// int temp;	// for temp index query
	char yoreName[64];
}Calres;


FILE* inputfile, *outputfile;
extern char* strdup(const char*);

Node* makeNode(const char*, Node*, enum _node_type);
Node* rightMost(Node* node);
void scanNode(Node* node);
void scanTree(Node* root);
void printTree(Node* root, int layer);
int calAlg(Node* node);
void libFuncInit();

#define YYSTYPE Node*
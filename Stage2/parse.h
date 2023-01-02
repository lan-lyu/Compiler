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
	char* oriName;		//original variable name
	char* tigerName;	//transferred variable name
	// int valid; 			//check if valid in this environment
	int size;			//whole size
	int isPara;		//value, 2=parameter
	int position;
}Variable;

FILE* inputfile, *outputfile;
extern char* strdup(const char*);

Node* makeNode(const char*, Node*, enum _node_type);
void scanNode(Node* node);
void scanTree(Node* root);
void printTree(Node* root, int layer);
void scanState(Node* node);

#define YYSTYPE Node*
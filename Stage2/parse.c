#include "parse.h"
#define EQ(a,b) (strcmp(a,b)==0)
// #define DEBUG

int GblVarCnt;
Variable* GblVarList[80000];
int LocVarCnt;
Variable* LocVarList[80000];

char tmpBuffer[1024];
char* outBuffer;
char* mainBuffer;

int FuncSize;
int FuncParCnt;
int ParamCnt;


// initialization for the global variable
void initEnv(){
	GblVarCnt = LocVarCnt = 0;
	mainBuffer = (char*)malloc(sizeof(char)*(1<<28));
	memset(mainBuffer, 0, sizeof(char)*(1<<28));
	FuncSize = FuncParCnt = ParamCnt = 0;
}

void printTree(Node* root, int layer){
	if(root){
		if(root->son){
			printf("%d【%s %d】->【%s %d】\n", layer, root->attr, root->type, root->son->attr, root->son->type);
			printTree(root->son, layer+1);
		}
		else{
			printf("%d single:【%s %d】\n", layer, root->attr, root->type);
		}
		if (root->sib){
			printf("turn to sib:");
			printTree(root->sib, layer);
		}
	}
}

const char* getOp(int type){
	// get op
	switch (type)
	{
	case 22:
		return "+";
		break;
	case 23:
		return "-";
		break;
	case 24:
		return "*";
		break;
	case 25:
		return "/";
		break;
	case 26:
		return "%";
		break;
	case 27:
		return "!";
		break;
	case 28:
		return "<=";
		break;
	case 29:
		return ">=";
		break;
	case 30:
		return "==";
		break;
	case 31:
		return "!=";
		break;
	case 32:
		return "<";
		break;
	case 33:
		return ">";
		break;
	case 34:
		return "&&";
		break;
	case 35:
		return "||";
		break;	
	default:
		fprintf(stderr, "opeartion error!\n");
		break;
	}
	return NULL;
}

Node* makeNode(const char* attr, Node* son, enum _node_type type)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->type = type;
	newNode->son = son;
	newNode->sib = NULL;
	newNode->attr = attr;
	return newNode;
}

void makeGblVar(Node* node){
	#ifdef DEBUG
		printf("________makeGlobalVar:[%s, %u]\n",node->attr,node->type);
	#endif
	Variable* newVar = (Variable*)malloc(sizeof(Variable));
	char tmp_name[64] = {'\0'};
	sprintf(tmp_name, "v%d", GblVarCnt);
	newVar->tigerName = strdup(tmp_name);
	newVar->isPara = 0;
	switch (node->type)
	{
	case 1:
		newVar->oriName = strdup(node->son->attr);
		newVar->size = 1;
		printf("%s = 0\n", newVar->tigerName);
		#ifdef DEBUG
			printf("_________makeGlobalVar_int:[%s, %u]\n", newVar->tigerName, newVar->size);
		#endif
		break;
	case 2:
		newVar->oriName = strdup(node->son->sib->attr);
		newVar->size = atoi(node->son->attr);
		printf("%s = malloc %d\n", newVar->tigerName, newVar->size);
		#ifdef DEBUG
			printf("_________makeGlobalVar_int[]:[%s, %u]\n", newVar->tigerName, newVar->size);
		#endif
		break;
	default:
		fprintf(stderr, "Not correct global variable\n");
		break;
	}
	GblVarList[GblVarCnt++] = newVar;
}

void makeLclVar(Node* node){
	Variable* newVar = (Variable*)malloc(sizeof(Variable));
	newVar->isPara = 0;
	newVar->position = FuncSize;
	switch (node->type)
	{
	case 1:
		newVar->oriName = strdup(node->son->attr);
		newVar->size = 1;
		break;
	case 2:
		newVar->oriName = strdup(node->son->sib->attr);
		newVar->size = (atoi(node->son->attr)/4);
		break;
	default:
		break;
	}
	FuncSize += newVar->size;
	LocVarList[LocVarCnt++] = newVar;
}

void scanExp(Node* node){
	return; 
}

void simpleCal(Node* node, int reg){
	char* res = (char*)malloc(sizeof(char) * 64);
	sprintf(res, "t%d", reg);
	switch (node->type)
	{
	//symbol
	case 37:
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, node->attr)){
				// int
				if(GblVarList[i]->size==1){
					sprintf(tmpBuffer, "\tload %s %s\n", GblVarList[i]->tigerName, res);
					strcat(outBuffer, tmpBuffer);
					return;
				}
				//int[]
				else{
					sprintf(tmpBuffer, "\tloadaddr %s %s\n", GblVarList[i]->tigerName, res);
					strcat(outBuffer, tmpBuffer);
					return;
				}
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			// printf("::::%s, %d\n", LocVarList[i]->oriName, i);
			if(EQ(LocVarList[i]->oriName, node->attr)){
				//int
				if(LocVarList[i]->size==1){
					sprintf(tmpBuffer, "\tload %d %s\n", LocVarList[i]->position, res);
					strcat(outBuffer, tmpBuffer);
					return;
				}
				else{
					sprintf(tmpBuffer, "\tloadaddr %d %s\n", LocVarList[i]->position, res);
					strcat(outBuffer, tmpBuffer);
					return;
				}
			}
		}
		break;
	// num
	case 39:
		sprintf(tmpBuffer, "\t%s = %s\n", res, node->attr);
		strcat(outBuffer, tmpBuffer);
		break;
	default:
		break;
	}
	return;
}

void scanStates(Node* node){
	while(node){
		scanState(node);
		if(node && node->sib){
			node = node->sib->son;
		}
		else{
			return;
		}
	}
}

void scanState(Node* node){
	#ifdef DEBUG
		printf("_________scanState:[%s, %u]\n", node->attr, node->type);
	#endif
	if(node==NULL) return;
	Node* first, *second;
	int find1, find2;
	switch (node->type){
	//local declaration
	//decl_int
	case 1:
		makeLclVar(node);
		break;
	//decl_int[]
	case 2:
		makeLclVar(node);
		break;
	// v = v op v
	case 9:
		node = node->son;
		first = node->sib;
		second = first->sib->sib;
		simpleCal(first, 1);
		simpleCal(second, 2);
		sprintf(tmpBuffer, "\tt0 = t1 %s t2\n", getOp(first->sib->type));
		strcat(outBuffer, tmpBuffer);
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt3[0] = t0\n");
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(EQ(LocVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tstore t0 %d\n", LocVarList[i]->position);
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		break;
	// v = op v
	case 10:
		node = node->son;
		first = node->sib->sib;
		simpleCal(first, 1);
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt3[0] = t0\n");
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(EQ(LocVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tstore t0 %d\n", LocVarList[i]->position);
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		break;
	// =
	case 11:
		node = node->son;
		simpleCal(node->sib, 0);
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt3[0] = t0\n");
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(EQ(LocVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tstore t0 %d\n", LocVarList[i]->position);
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		break;
	// int[] = int
	case 12:
		node = node->son;
		first = node->sib;
		second = first->sib;
		simpleCal(first, 1);
		simpleCal(second, 0);
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt4 = t3 + t1\n");
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt4[0] = t0\n");
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(EQ(LocVarList[i]->oriName, node->attr)){
				// printf("----[%d %s %d]----\n", i, LocVarList[i]->oriName, LocVarList[i]->isPara);
				if(LocVarList[i]->isPara){
					sprintf(tmpBuffer, "\tload %d t3\n", LocVarList[i]->position);
					strcat(outBuffer, tmpBuffer);
				}
				else{
					sprintf(tmpBuffer, "\tloadaddr %d t3\n", LocVarList[i]->position);
					strcat(outBuffer, tmpBuffer);
				}
				sprintf(tmpBuffer, "\tt4 = t3 + t1\n");
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt4[0] = t0\n");
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		break;
	//=v[]
	case 13:
		node = node->son;
		first = node->sib;
		second = first->sib;
		simpleCal(second, 1);
		find1 = 0;
		find2 = 0;
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(!find1 && EQ(GblVarList[i]->oriName, first->attr)){
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt4 = t3 + t1\n");
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt0 = t4[0]\n");
				strcat(outBuffer, tmpBuffer);
				find1 = 1;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(!find1 && EQ(LocVarList[i]->oriName, first->attr)){
				if(LocVarList[i]->isPara){
					sprintf(tmpBuffer, "\tload %d t3\n", LocVarList[i]->position);
					strcat(outBuffer, tmpBuffer);
				}
				else{
					sprintf(tmpBuffer, "\tloadaddr %d t3\n", LocVarList[i]->position);
					strcat(outBuffer, tmpBuffer);
				}
				sprintf(tmpBuffer, "\tt4 = t3 + t1\n");
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt0 = t4[0]\n");
				strcat(outBuffer, tmpBuffer);
				find1 = 1;
			}
		}
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(!find2 && EQ(GblVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt3[0] = t0\n");
				strcat(outBuffer, tmpBuffer);
				find2 = 1;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(!find2 && EQ(LocVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tstore t0 %d\n", LocVarList[i]->position);
				strcat(outBuffer, tmpBuffer);
				find2 = 1;
			}
		}
		return;
		break;
	// if goto
	case 14:
		first = node->son;
		second = first->sib->sib;
		simpleCal(first, 1);
		simpleCal(second, 2);
		sprintf(tmpBuffer, "\tif t1 %s t2 goto %s\n", getOp(first->sib->type), second->sib->attr);
		strcat(outBuffer, tmpBuffer);
		break;
	// goto
	case 15:
		sprintf(tmpBuffer, "\tgoto %s\n", node->son->attr);
		strcat(outBuffer, tmpBuffer);
		break;
	// label
	case 16:
		sprintf(tmpBuffer, "%s:\n", node->son->attr);
		strcat(outBuffer, tmpBuffer);
		break;
	// para
	case 17:
		simpleCal(node->son, 1);
		sprintf(tmpBuffer, "\ta%d = t1\n", ParamCnt++);
		strcat(outBuffer, tmpBuffer);
		break;
	// void call
	case 18:
		ParamCnt = 0;
		sprintf(tmpBuffer, "\tcall %s\n", node->son->attr);
		strcat(outBuffer, tmpBuffer);
		break;
	// int call
	case 19:
		node = node->son;
		ParamCnt = 0;
		sprintf(tmpBuffer, "\tcall %s\n", node->sib->attr);
		strcat(outBuffer, tmpBuffer);
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(outBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt3[0] = a0\n");
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(EQ(LocVarList[i]->oriName, node->attr)){
				sprintf(tmpBuffer, "\tstore a0 %d\n", LocVarList[i]->position);
				strcat(outBuffer, tmpBuffer);
				return;
			}
		}
		break;
	//return x
	case 20:
		simpleCal(node->son, 1);
		sprintf(tmpBuffer, "\ta0 = t1\n");
		strcat(outBuffer, tmpBuffer);
		sprintf(tmpBuffer, "\treturn\n");
		strcat(outBuffer, tmpBuffer);
		break;
	//return
	case 21:
		sprintf(tmpBuffer, "\treturn\n");
		strcat(outBuffer, tmpBuffer);
		break;
	default:
		scanExp(node->son);
		break;
	}
}

void scanFunc(Node* node){
	// functionHeader = node->son
	// Statements = node->son->sib
	// funend = node->son->sib->sib
	outBuffer = (char*)malloc(sizeof(char)*(1<<28));
	memset(outBuffer, 0, sizeof(char)*(1<<28));
	LocVarCnt = 0;
	FuncSize = 0;
	ParamCnt = 0;

	Node* funcHeader = node->son;
	//parameters
	FuncParCnt = atoi(funcHeader->son->sib->attr);
	printf("%s [%d] ", funcHeader->son->attr, FuncParCnt);
	for(int i = 0; i < FuncParCnt; ++i){
		Variable* newPara = (Variable*)malloc(sizeof(Variable));
		sprintf(tmpBuffer, "\tstore a%d %d\n", i, i);
		strcat(outBuffer, tmpBuffer);
		char tmp_name[64] = {'\0'};
		sprintf(tmp_name, "p%d", LocVarCnt);
		newPara->oriName = strdup(tmp_name);
		newPara->size = 1;
		newPara->isPara = 1;
		newPara->position = FuncSize;
		FuncSize += 1;
		LocVarList[LocVarCnt++] = newPara;
	}
	//statements
	scanStates(node->son->sib->son);

	printf("[%d]\n", FuncSize);
	//if main, print mainBuffer
	if(EQ(funcHeader->son->attr, "f_main")){
		printf("%s", mainBuffer);
	}
	printf("%s", outBuffer);
	printf("end %s\n", node->son->sib->sib->son->attr);
}

void scanNode(Node* node){
	#ifdef DEBUG
		printf("________scanNode:%s, %u\n",node->attr,node->type);
	#endif
	Node* tmp_node = node;
    switch (node->type)
    {
	// var symbol
    case 1:
    // var imm symbol
    case 2:
        makeGblVar(node);
        break;
	// symbol = imm
    case 3:
		#ifdef DEBUG
			printf("_________asg_int:[%s, %u]\n",tmp_node->attr,tmp_node->type);
		#endif
        tmp_node = node->son;
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, tmp_node->attr)){
				#ifdef DEBUG
					printf("_________asg_int_find_global:[%s, %u]\n",tmp_node->attr,tmp_node->type);
				#endif
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(mainBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt4 = %s\n", tmp_node->sib->attr);
				strcat(mainBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt3[0] = t4\n");
				strcat(mainBuffer, tmpBuffer);
				return;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(EQ(LocVarList[i]->oriName, tmp_node->attr)){
				#ifdef DEBUG
					printf("_________asg_int_find_local:[%s, %u]\n",tmp_node->attr,tmp_node->type);
				#endif

				return;
			}
		}
        break;
	// symbol[imm] = imm
    case 4:
		#ifdef DEBUG
			printf("_________asg[]_int:[%s, %u]\n",tmp_node->attr,tmp_node->type);
		#endif
        tmp_node = node->son;
		for(int i = GblVarCnt - 1; i >= 0; --i){
			if(EQ(GblVarList[i]->oriName, tmp_node->attr)){
				#ifdef DEBUG
					printf("_________asg[]_int_find_global:[%s, size:%d]\n",tmp_node->attr,GblVarList[i]->size);
				#endif
				sprintf(tmpBuffer, "\tloadaddr %s t3\n", GblVarList[i]->tigerName);
				strcat(mainBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt4 = t3 + %s\n", tmp_node->sib->attr);
				strcat(mainBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt0 = %s\n", tmp_node->sib->sib->attr);
				strcat(mainBuffer, tmpBuffer);
				sprintf(tmpBuffer, "\tt4[0] = t0\n");
				strcat(mainBuffer, tmpBuffer);
				return;
			}
		}
		for(int i = LocVarCnt - 1; i >= 0; --i){
			if(EQ(LocVarList[i]->oriName, tmp_node->attr)){
				#ifdef DEBUG
					printf("_________asg[]_int_find_local:[%s, %u]\n",tmp_node->attr,tmp_node->type);
				#endif

				return;
			}
		}
        break;
	// func[imm]
    case 5:
		#ifdef DEBUG
			printf("_________asg_int:[%s, %u]\n",tmp_node->attr,tmp_node->type);
		#endif
        scanFunc(node);
        break;
    default:
        break;
    }

}

void scanTree(Node* root){
	initEnv();
	#ifdef DEBUG
		printf("==================Print Tree===============\n");
		printTree(root, 0);
		printf("===================Scan Tree===============\n");
	#endif
	root = root->son;
	// first scan for global variable
	while(root){
		scanNode(root);
		root = root->sib;
	}
	printf("\n");
}

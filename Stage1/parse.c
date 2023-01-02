#include "parse.h"
#define EQ(a,b) (strcmp(a,b)==0)
#define GRAPH
// #define DEBUG

int VarCnt;
Variable* VarList[2000];

int TmpVarCnt;
int blkLayer;
int LabelCnt;
int whileLabel;

int isBreak;
int inCond;
int condLabel;

int FuncCnt, FuncParamCnt, FuncParamIdx, GblParamCnt;
Func* FuncList[1000];
Para* ParamList[10000];
int needPara;

char tmpBuffer[1024];
char* outBuffer;
char* globalBuffer;

int inFunc;

int graphCnt;

// initialization for the global variable
void initEnv(){
	VarCnt = 0;
    blkLayer = LabelCnt = whileLabel = 0;
    isBreak = inCond = condLabel = 0;
	TmpVarCnt = 0;
	FuncCnt = FuncParamCnt = GblParamCnt = FuncParamIdx = needPara = 0;
	inFunc = 0;
    graphCnt = 0;
    libFuncInit();
}

void treeGraph(Node* root, int idx){
    graphCnt++;
	if(root){
        printf("\t%d [label=\"%s\"];\n", idx, root->attr);
		if(root->son){
            printf("\t%d -> %d\n", idx, graphCnt);
            // printf("\t%s -> %s\n", root->attr, root->son->attr);
			treeGraph(root->son, graphCnt);
		}
		if (root->sib){
			// printf("\t{rank = same; %s; %s;}\n", root->attr, root->sib->attr);
            // printf("\t%s -> %s\n", root->attr, root->sib->attr);
			printf("\t{rank = same; %d; %d;}\n", idx, graphCnt);
            printf("\t%d -> %d\n", idx, graphCnt);
			treeGraph(root->sib, graphCnt);
		}
	}
}

void getGraph(Node* root){
    printf("digraph G{\n");
    printf("\trankdir = TB;\n");
    printf("\tsubgraph {\n");
    treeGraph(root, 0);
    printf("\t}\n");
    printf("}\n");
    return;
}

void libFuncInit(){
    #ifdef DEBUG
        printf("_________FuncInit_begin\n");
    #endif
    Func** newFunc = (Func**)malloc(sizeof(Func*) * 8);
    char tmp_name[64] = {'\0'};
    sprintf(tmp_name, "getint");
    newFunc[0] = (Func*)malloc(sizeof(Func));
    newFunc[0]->funcName = strdup(tmp_name);
    newFunc[0]->paramNum = 0;
    newFunc[0]->returnInt = 1;
    newFunc[0]->paramIdx = GblParamCnt;

    sprintf(tmp_name, "getch");
    newFunc[1] = (Func*)malloc(sizeof(Func));
    newFunc[1]->funcName = strdup(tmp_name);
    newFunc[1]->paramNum = 0;
    newFunc[1]->returnInt = 1;
    newFunc[1]->paramIdx = GblParamCnt;

    sprintf(tmp_name, "getint");
    newFunc[2] = (Func*)malloc(sizeof(Func));
    newFunc[2]->funcName = strdup(tmp_name);
    newFunc[2]->paramNum = 0;
    newFunc[2]->returnInt = 1;
    newFunc[2]->paramIdx = GblParamCnt;

    sprintf(tmp_name, "getarray");
    newFunc[3] = (Func*)malloc(sizeof(Func));
    newFunc[3]->funcName = strdup(tmp_name);
    newFunc[3]->paramNum = 1;
    newFunc[3]->returnInt = 1;
    Para* newPara = (Para*)malloc(sizeof(Para));
    newPara->dimension = 1;
    newPara->funcName = strdup(tmp_name);
    ParamList[GblParamCnt] = newPara;
    newFunc[3]->paramIdx = GblParamCnt++;

    sprintf(tmp_name, "putint");
    newFunc[4] = (Func*)malloc(sizeof(Func));
    newFunc[4]->funcName = strdup(tmp_name);
    newFunc[4]->paramNum = 1;
    newFunc[4]->returnInt = 0;
    newPara = (Para*)malloc(sizeof(Para));
    newPara->dimension = 0;
    newPara->funcName = strdup(tmp_name);
    ParamList[GblParamCnt] = newPara;
    newFunc[4]->paramIdx = GblParamCnt++;

    sprintf(tmp_name, "putch");
    newFunc[5] = (Func*)malloc(sizeof(Func));
    newFunc[5]->funcName = strdup(tmp_name);
    newFunc[5]->paramNum = 1;
    newFunc[5]->returnInt = 0;
    newPara = (Para*)malloc(sizeof(Para));
    newPara->funcName = strdup(tmp_name);
    newPara->dimension = 0;
    ParamList[GblParamCnt] = newPara;
    newFunc[5]->paramIdx = GblParamCnt++;

    sprintf(tmp_name, "putarray");
    newFunc[6] = (Func*)malloc(sizeof(Func));
    newFunc[6]->funcName = strdup(tmp_name);
    newFunc[6]->paramNum = 2;
    newFunc[6]->returnInt = 0;
    newPara = (Para*)malloc(sizeof(Para));
    newPara->funcName = strdup(tmp_name);
    newPara->dimension = 0;
    ParamList[GblParamCnt] = newPara;
    Para* newPara1 = (Para*)malloc(sizeof(Para));
    newPara1->dimension = 1;
    newPara1->funcName = strdup(tmp_name);

    ParamList[GblParamCnt+1] = newPara1;
    newFunc[6]->paramIdx = GblParamCnt;
    GblParamCnt +=2;

    sprintf(tmp_name, "starttime");
    newFunc[7] = (Func*)malloc(sizeof(Func));
    newFunc[7]->funcName = strdup(tmp_name);
    newFunc[7]->paramNum = 0;
    newFunc[7]->returnInt = 0;

    sprintf(tmp_name, "stoptime");
    newFunc[8] = (Func*)malloc(sizeof(Func));
    newFunc[8]->funcName = strdup(tmp_name);
    newFunc[8]->paramNum = 0;
    newFunc[8]->returnInt = 0;
    for(int i = 0; i < 9; ++i){
        FuncList[FuncCnt++] = newFunc[i];
    }
    #ifdef DEBUG
        printf("_________FuncInit_finished\n");
    #endif
}

Node* rightMost(Node* node){
	while(node->sib) node = node->sib;
	return node;
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

Node* makeNode(const char* attr, Node* son, enum _node_type type)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->type = type;
	newNode->son = son;
	newNode->sib = NULL;
	newNode->attr = attr;
	return newNode;
}

char* simpleCal(Node* node){
    #ifdef DEBUG
        printf("_________simpleCal:[%s, %u]\n",node->attr,node->type);
    #endif
    char* first_res = (char*)malloc(sizeof(char)*64);
    char* second_res = (char*)malloc(sizeof(char)*64);
    char* op = (char*)malloc(sizeof(char)*2);
    char* res = (char*)malloc(sizeof(char)*64);
    Node* tmp_node = node;
    int label;
    int jmplabel = condLabel;
    switch (node->type)
    {
    case 32: //lval
    case 33: //lval[]
        return simpleCal(node->son);
        break;
    case 52: //imm
        sprintf(res, "%s", node->attr);
        return res;
        break;
    case 11: // InitVal_Exp
        return simpleCal(node->son);
        break;
    case 53: // Indentifier
        if(inFunc){
            for(int i = FuncParamCnt + FuncParamIdx - 1; i >= FuncParamIdx; --i){
                if(EQ(ParamList[i]->oriName, node->attr)){
                    #ifdef DEBUG
                        printf("__________simpleCal_param:[oriName:%s, yoreName:%s, dimension:%d]\n",node->attr,ParamList[i]->yoreName, ParamList[i]->dimension);
                    #endif
                    if(ParamList[i]->dimension == 0 || needPara){
                        sprintf(res, "%s", ParamList[i]->yoreName);
                        return res;                        
                    }
                    else{
                        sprintf(first_res, "t%d", TmpVarCnt++);
                        if(inFunc) printf("\t");
                        printf("var %s\n", first_res);
                        if(inFunc){
                            sprintf(tmpBuffer, "\t%s = 0\n", first_res);
                            strcat(outBuffer, tmpBuffer);
                        }
                        else{
                            printf("%s = 0\n", first_res);
                        }
                        for(int j = 0; j < ParamList[i]->dimension; ++j){
                            #ifdef DEBUG
                                printf("___________simpleCal_param[]:[j:%d, dimension:%d]\n", j, ParamList[i]->dimension);
                            #endif
                            tmp_node = tmp_node->sib;
                            sprintf(second_res, "%s", simpleCal(tmp_node->son));
                            if(inFunc){
                                printf("\t");
                            }
                            printf("var t%d\n", TmpVarCnt);
                            if(inFunc){
                                sprintf(tmpBuffer, "\tt%d = %d * %s\n", TmpVarCnt, ParamList[i]->dimesize[j], second_res);
                                strcat(outBuffer, tmpBuffer);
                            }
                            else{
                                printf("t%d = %d * %s\n", TmpVarCnt, ParamList[i]->dimesize[j], second_res);
                            }
                            if(inFunc){
                                sprintf(tmpBuffer, "\t%s = %s + t%d\n", first_res, first_res, TmpVarCnt);
                                strcat(outBuffer, tmpBuffer);
                            }
                            else{
                                sprintf(tmpBuffer, "%s = %s + t%d\n", first_res, first_res, TmpVarCnt);
                                strcat(outBuffer, tmpBuffer);
                            }
                            TmpVarCnt++;
                        }
                        if(inFunc){
                            sprintf(tmpBuffer, "\t%s = %s * 4\n", first_res, first_res);
                            strcat(outBuffer, tmpBuffer);
                        }
                        else{
                            printf("%s = %s * 4\n", first_res, first_res);
                        }
                        sprintf(res, "t%d", TmpVarCnt++);
                        if(inFunc){
                            printf("\tvar %s\n", res);
                            sprintf(tmpBuffer, "\t%s = %s [ %s ]\n", res, ParamList[i]->yoreName, first_res);
                            strcat(outBuffer, tmpBuffer);
                        }
                        else{
                            printf("var %s\n", res);
                            printf("%s = %s [ %s ]\n", res, ParamList[i]->yoreName, first_res);
                        }
                        return res;
                    }
                }
            }
        }
        for(int i = VarCnt - 1; i >= 0; --i){
            if(EQ(VarList[i]->oriName, node->attr) && VarList[i]->valid != -1){
                if(needPara){
                    sprintf(res, "%s", VarList[i]->yoreName);
                    return res;
                }
                // only a int value
                if(VarList[i]->dimension == 0){
                    sprintf(res, "t%d", TmpVarCnt++);
                    if(inFunc) printf("\t");
                    printf("var %s\n", res);
                    if(inFunc){
                        sprintf(tmpBuffer, "\t%s = %s\n", res, VarList[i]->yoreName);
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        printf("%s = %s\n", res, VarList[i]->yoreName);
                    }
                    return res;
                }
                else{
                    #ifdef DEBUG
                        printf("__________simpleCal_ind[]:[%s, %s]\n",VarList[i]->oriName, VarList[i]->yoreName);
                    #endif
        			Node* tmp_node = node;
					int var_idx = 0;
					int sib_cnt = 0;
					int* acm_sz = (int*)malloc(sizeof(int) * VarList[i]->dimension);
					int dim_1 = (VarList[i]->dimension)-1;
					acm_sz[dim_1] = 1;
					for(int j = dim_1 - 1; j >= 0; --j){
						acm_sz[j] = acm_sz[j+1]*VarList[i]->dimensize[j+1];
					}
                    char printtmp[64] = {'\0'};
                    sprintf(printtmp, "t%d", TmpVarCnt++);
                    if(inFunc){
                        printf("\t");
                    }
                    printf("var %s\n", printtmp);
                    if(inFunc){
                        sprintf(tmpBuffer, "\t%s = 0\n", printtmp);
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        printf("%s = 0\n", printtmp);
                    }
					while(tmp_node->sib){
						tmp_node = tmp_node->sib;
						// var_idx += acm_sz[sib_cnt] * calAlg(tmp_node->son);
                        sprintf(first_res, "%s", simpleCal(tmp_node->son));
                        if(inFunc){
                            printf("\t");
                        }
                        printf("var t%d\n", TmpVarCnt);
                        if(inFunc){
                            sprintf(tmpBuffer, "\tt%d = %d * %s\n", TmpVarCnt, acm_sz[sib_cnt], first_res);
                            strcat(outBuffer, tmpBuffer);
                        }
                        else{
                            printf("t%d = %d * %s\n", TmpVarCnt, acm_sz[sib_cnt], first_res);
                        }
                        if(inFunc){
                            sprintf(tmpBuffer, "\t%s = %s + t%d\n", printtmp, printtmp, TmpVarCnt);
                            strcat(outBuffer, tmpBuffer);
                        }
                        else{
                            sprintf(tmpBuffer, "%s = %s + t%d\n", printtmp, printtmp, TmpVarCnt);
                            strcat(outBuffer, tmpBuffer);
                        }
                        TmpVarCnt++;
						sib_cnt++;
					}
                    // return VarList[i]->val[var_idx];
                    sprintf(res, "t%d", TmpVarCnt++);
                    if(inFunc) printf("\t");
                    printf("var %s\n", res);
                    if(inFunc){
                        sprintf(tmpBuffer, "\t%s = %s * 4\n", printtmp, printtmp);
                        strcat(outBuffer, tmpBuffer);
                        sprintf(tmpBuffer, "\t%s = %s [ %s ]\n", res, VarList[i]->yoreName, printtmp);
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        printf("%s = %s * 4\n", printtmp, printtmp);
                        printf("%s = %s [ %s ]\n", res, VarList[i]->yoreName, printtmp);
                    }
                    return res;
                }
            }
        }
        break;  
    case 34: 
    case 35: //Unary_func
        for(int i = FuncCnt - 1; i >= 0; --i){
            if(EQ(FuncList[i]->funcName, node->son->attr)){
                #ifdef DEBUG
                    printf("__________simpleCal_uf:[funcName:%s, paramNum:%d, returnInt:%d]\n",FuncList[i]->funcName,FuncList[i]->paramNum, FuncList[i]->returnInt);
                #endif
                Node* tmp_node = node->son;
                for(int j = 0; j < FuncList[i]->paramNum; ++j){
                    if(ParamList[j+FuncList[i]->paramIdx]->dimension != 0){
                        needPara = 1;
                    }
                    // needPara = 1;
                    if(inFunc){
                        first_res = simpleCal(tmp_node->sib);
                        sprintf(tmpBuffer, "\t");
                        strcat(outBuffer, tmpBuffer);
                        sprintf(tmpBuffer, "param %s\n", first_res);
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        printf("param %s\n", simpleCal(tmp_node->sib));
                    }
                    tmp_node = tmp_node->sib;
                }
                needPara = 0;
                if(FuncList[i]->returnInt){
                    sprintf(res, "t%d", TmpVarCnt++);
                    if(inFunc){
                        printf("\tvar %s\n", res);
                        sprintf(tmpBuffer, "\t%s = call f_%s\n", res, FuncList[i]->funcName);
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        printf("var %s", res);
                        printf("%s = call f_%s\n", res, FuncList[i]->funcName);
                    }
                }
                else{
                    sprintf(res, "call f_%s", FuncList[i]->funcName);
                }
                return res;
            }
        }
        break;
    case 36: // +
        sprintf(res, "t%d", TmpVarCnt++);
        if(inFunc) printf("\t");
        printf("var %s\n", res);
        first_res = simpleCal(node->son);
        if(inFunc){
            sprintf(tmpBuffer, "\t%s = %s\n", res, first_res);
            strcat(outBuffer, tmpBuffer);
        }
        else{
            printf("%s = %s\n", res, first_res);
        }
        break;
    case 37: // -
    case 38: // !
        sprintf(op, "%s", node->attr);
        sprintf(res, "t%d", TmpVarCnt++);
        if(inFunc) printf("\t");
        printf("var %s\n", res);
        first_res = simpleCal(node->son);
        if(inFunc){
            sprintf(tmpBuffer, "\t%s = %s %s\n", res, op, first_res);
            strcat(outBuffer, tmpBuffer);
        }
        else{
            printf("%s = %s %s\n", res, op, first_res);
        }
        break;
    case 39: //*
    case 40: ///
    case 41: //% 
    case 42: //+
    case 43: //-
    case 44: //<
    case 45: //>
    case 46: //<=
    case 47: //>=
    case 48: //==
    case 49: //!=
        sprintf(op, "%s", node->attr);
        sprintf(res, "t%d", TmpVarCnt++);
        if(inFunc) printf("\t");
        printf("var %s\n", res);
        first_res = simpleCal(node->son);
        second_res = simpleCal(node->son->sib);
        if(inFunc){
            sprintf(tmpBuffer, "\t%s = %s %s %s\n", res, first_res, op, second_res);
            strcat(outBuffer, tmpBuffer);
        }
        else{
            printf("%s = %s %s %s\n", res, first_res, op, second_res);
        }
        break;
    case 50: //&&
        if(inCond){
            label = LabelCnt++;
            condLabel = label;
            first_res = simpleCal(node->son);
            if(inFunc){
                sprintf(tmpBuffer, "\tif %s == 0 goto l%d\n", first_res, label);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("if %s == 0 goto l%d\n", first_res, label);
            }
            second_res = simpleCal(node->son->sib);
            if(inFunc){
                sprintf(tmpBuffer, "\tif %s == 0 goto l%d\n", second_res, label);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("if %s == 0 goto l%d\n", second_res, label);
            }
            sprintf(res, "t%d", TmpVarCnt++);
            if(inFunc) printf("\t");
            printf("var %s\n", res);
            if(inFunc){
                sprintf(tmpBuffer, "\t%s = 1\n", res);
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "\tgoto l%d\n", LabelCnt);
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "l%d:\n", label);
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "\t%s = 0\n", res);
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "l%d:\n", LabelCnt++);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("%s = 1\n", res);
                printf("goto l%d\n", label);
                printf("l%d:\n", label);
                printf("%s = 1\n", res);
            }
            condLabel = jmplabel;
        }
        else{
            sprintf(op, "%s", node->attr);
            sprintf(res, "t%d", TmpVarCnt++);
            if(inFunc) printf("\t");
            printf("var %s\n", res);
            first_res = simpleCal(node->son);
            second_res = simpleCal(node->son->sib);
            if(inFunc){
                sprintf(tmpBuffer, "\t%s = %s %s %s\n", res, first_res, op, second_res);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("%s = %s %s %s\n", res, first_res, op, second_res);
            }
        }
        break;
    case 51: //||
        if(inCond){
            label = LabelCnt++;
            condLabel = label;
            first_res = simpleCal(node->son);
            if(inFunc){
                sprintf(tmpBuffer, "\tif %s != 0 goto l%d\n", first_res, label);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("if %s != 0 goto l%d\n", first_res, label);
            }
            second_res = simpleCal(node->son->sib);
            if(inFunc){
                sprintf(tmpBuffer, "\tif %s != 0 goto l%d\n", second_res, label);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("if %s != 0 goto l%d\n", second_res, label);
            }
            sprintf(res, "t%d", TmpVarCnt++);
            if(inFunc) printf("\t");
            printf("var %s\n", res);
            if(inFunc){
                sprintf(tmpBuffer, "\t%s = 0\n", res);
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "\tgoto l%d\n", jmplabel);
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "l%d:\n", label);
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "\t%s = 1\n", res);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("%s = 0\n", res);
                printf("goto l%d\n", jmplabel);
                printf("l%d:\n", label);
                printf("%s = 1\n", res);
            }
            condLabel = jmplabel;
        }
        else{
            sprintf(op, "%s", node->attr);
            sprintf(res, "t%d", TmpVarCnt++);
            if(inFunc) printf("\t");
            printf("var %s\n", res);
            first_res = simpleCal(node->son);
            second_res = simpleCal(node->son->sib);
            if(inFunc){
                sprintf(tmpBuffer, "\t%s = %s %s %s\n", res, first_res, op, second_res);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("%s = %s %s %s\n", res, first_res, op, second_res);
            }
        }
        break;
    default:
        break;
    }
    return res;
}

int calAlg(Node* node){
    #ifdef DEBUG
        printf("_________calAlg:[%s, %u]\n",node->attr,node->type);
    #endif
    int divider = 0;
    switch (node->type)
    {
    case 52:
        return atoi(node->attr);
        break;
    case 53: // Indentifier
        for(int i = VarCnt - 1; i >= 0; --i){
            if(EQ(VarList[i]->oriName, node->attr) && VarList[i]->valid != -1){
                // only a int value
                if(VarList[i]->dimension == 0){
                    #ifdef DEBUG
                        printf("__________calAlg_return:[%s, %u]\n",VarList[i]->oriName, VarList[i]->val[0]);
                    #endif
                    return VarList[i]->val[0];
                }
                else{
        			Node* tmp_node = node;
					int var_idx = 0;
					int sib_cnt = 0;
					int* acm_sz = (int*)malloc(sizeof(int) * VarList[i]->dimension);
					int dim_1 = (VarList[i]->dimension)-1;
					acm_sz[dim_1] = 1;
					for(int j = dim_1 - 1; j >= 0; --j){
						acm_sz[j] = acm_sz[j+1]*VarList[i]->dimensize[j+1];
					}
					while(tmp_node->sib){
						tmp_node = tmp_node->sib;
						var_idx += acm_sz[sib_cnt] * calAlg(tmp_node->son);
						sib_cnt++;
					}
                    return VarList[i]->val[var_idx];
                }
            }
        }
        break;
    case 32: //lval
    case 33: //lval[]
        return calAlg(node->son);
        break;
    case 37:// -
        return calAlg(node->son) * (-1);
        break;
    case 39: //*
        return calAlg(node->son) * calAlg(node->son->sib);
    case 40: ///
        divider = calAlg(node->son->sib);
        if(divider == 0)
            return 0;
        return calAlg(node->son) / divider;
    case 41: //% 
        divider = calAlg(node->son->sib);
        if(divider == 0)
            return 0;
        return calAlg(node->son) % divider;
    case 42: //+
        return calAlg(node->son) + calAlg(node->son->sib);
    case 43: //-
        return calAlg(node->son) - calAlg(node->son->sib);
    case 44: //<
        return calAlg(node->son) < calAlg(node->son->sib);
    case 45: //>
        return calAlg(node->son) > calAlg(node->son->sib);
    case 46: //<=
        return calAlg(node->son) <= calAlg(node->son->sib);
    case 47: //>=
        return calAlg(node->son) >= calAlg(node->son->sib);
    case 48: //==
        return calAlg(node->son) == calAlg(node->son->sib);
    case 49: //!=
        return calAlg(node->son) != calAlg(node->son->sib);
    case 50: //&&
        return calAlg(node->son) && calAlg(node->son->sib);
    case 51: //||
        return calAlg(node->son) || calAlg(node->son->sib);
    default:
        break;
    }
    return 0;
}

int arrayVal(Node* node, Variable* var,int idx, int layer, int sz){
	node = node->son;
	int tmp_idx = 0;
	while(tmp_idx < sz){
		if(node->type == 11){//EQ(node->attr, "InitVal_Exp")
			var->val[idx+tmp_idx] = calAlg(node->son);
            if(inFunc){
                sprintf(tmpBuffer, "\t%s [ %d ] = %s\n", var->yoreName, (idx+tmp_idx)<<2, simpleCal(node->son));
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("%s [ %d ] = %s\n", var->yoreName, (idx+tmp_idx)<<2, simpleCal(node->son));
            }
			tmp_idx++;
			if(node->sib){
				node = node->sib;
			}
			else{
				for(int i = tmp_idx; i < sz; ++i)
					var->val[idx + i] = 0;
				tmp_idx = sz;
			}
		}
		else if (node->type == 12){//EQ(node->attr, "InitVal_{}")
			int tmp_sz = sz/var->dimensize[layer];
			for(int i = tmp_idx; i < tmp_sz; ++i){
				var->val[idx + i] = 0;
			}
			tmp_idx += tmp_sz;
			if(node->sib){
				node = node->sib;
			}
			else{
				for(int i = tmp_idx; i < sz; ++i)
					var->val[idx + i] = 0;
				tmp_idx = sz;
			}
		}
		else if (node->type == 13){//EQ(node->attr, "InitVal_{v}")
			int tmp_sz = sz/var->dimensize[layer];
			// compensate for 0
			if(tmp_idx % tmp_sz){
				for(int i = tmp_idx; i < tmp_sz; ++i){
					var->val[idx+i] = 0;
				}
				tmp_idx += tmp_sz - tmp_idx;
			}
			// recursive ArrayVal
			tmp_idx = arrayVal(node, var, idx + tmp_idx, layer+1, tmp_sz);
			if(node->sib){
				node = node->sib;
			}
			else{
				for(int i = tmp_idx; i < sz; ++i){
					var->val[i] = 0;
				}
				tmp_idx = sz;
			}
		}
	}
	return idx+sz;
}

void makeVar(Node* node){
    #ifdef DEBUG
        printf("_________makeVar:%s, %u\n",node->attr,node->type);
    #endif
	Variable* newVar = (Variable*)malloc(sizeof(Variable));
    int tmp_dim = 0;
    int whole_size = 1;
    Node* tmp_node = node->son;
    newVar->valid = blkLayer;
	newVar->oriName = (strdup(node->son->attr));
    char temp_name[32] = {'\0'};
    sprintf(temp_name, "T%d", VarCnt);
    newVar->yoreName = strdup(temp_name);
    switch (node->type)
    {
    case 7: // int
        newVar->dimension = 0;
		newVar->dimensize = (int*)malloc(sizeof(int));
		newVar->dimensize[0] = 1;
		newVar->val = (int*)malloc(sizeof(int));
		newVar->size = 1;
        // NAMEPRINT
        if(inFunc) printf("\t");
        printf("var %s\n", newVar->yoreName);
        break;
    case 3: // const int=
    case 8: // int=
        newVar->dimension = 0;
		newVar->dimensize = (int*)malloc(sizeof(int));
		newVar->dimensize[0] = 1;
		newVar->val = (int*)malloc(sizeof(int));
		newVar->size = 1;
        newVar->val[0] = calAlg(node->son->sib->son);
        // NAMEPRINT
        if(inFunc) printf("\t");
        printf("var %s\n", newVar->yoreName);
        if(inFunc){
            sprintf(tmpBuffer, "\t%s = %s\n", newVar->yoreName, simpleCal(node->son->sib));  
            strcat(outBuffer, tmpBuffer);
        }
        else{
            printf("%s = %s\n", newVar->yoreName, simpleCal(node->son->sib));
        }
        #ifdef DEBUG
            printf("__________makeVar_int=:[%s %s val:%d]\n", newVar->oriName, newVar->yoreName, newVar->val[0]);
        #endif
        break;
    case 9: // int[]
        node = node->son;
        while(tmp_node->sib && EQ(tmp_node->sib->attr, "[]")){
            tmp_dim++;
            tmp_node = tmp_node->sib;
        }
        newVar->dimension = tmp_dim;
        newVar->dimensize = (int*)malloc(sizeof(int)*tmp_dim);
		// get each dimension size
		tmp_node = node->sib;
		for(int i = 0; i < tmp_dim; ++i){
			newVar->dimensize[i] = calAlg(tmp_node->son);
			tmp_node = tmp_node->sib;
			whole_size *= newVar->dimensize[i];
		}
		newVar->size = whole_size;
		newVar->val = (int*)malloc(sizeof(int*) * whole_size);
        // print
        if(inFunc) printf("\t");
        printf("var %d %s\n", newVar->size * 4, newVar->yoreName);
        #ifdef DEBUG
            printf("__________makeVar_int[]:[%s %s size:%d]\n", newVar->oriName, newVar->yoreName, newVar->size);
        #endif
        break;
    case 2: // const int[]=
    case 10: // int[]=
        node = node->son;
		// scan dimension
		while(tmp_node->sib && EQ(tmp_node->sib->attr, "[]")){
			tmp_dim++;
			tmp_node = tmp_node->sib;
		}
		newVar->dimension = tmp_dim;
		newVar->dimensize = (int*)malloc(sizeof(int) * tmp_dim);
		// get each dimension size
		tmp_node = node->sib;
		for(int i = 0; i < tmp_dim; ++i){
			newVar->dimensize[i] = calAlg(tmp_node->son);
			// printf("_________MakeVar_int[]=_dim%d %d\n", i, newVar->dimensize[i]);
			tmp_node = tmp_node->sib;
			whole_size *= newVar->dimensize[i];
		}
		newVar->size = whole_size;
		newVar->val = (int*)malloc(sizeof(int*) * whole_size);
        // print
        if(inFunc) printf("\t");
        printf("var %d %s\n", newVar->size * 4, newVar->yoreName);
        // put value
        tmp_node = tmp_node->son;
        if(tmp_node == NULL) break;
        int cur_idx = 0;
        int cur_sz = whole_size/newVar->dimensize[0];
		while(cur_idx < whole_size){
			// just put values
			if(tmp_node->type == 11){ //EQ(tmp_node->attr,"InitVal_Exp")
				newVar->val[cur_idx] = calAlg(tmp_node->son);
                if(inFunc){
                    sprintf(tmpBuffer, "\t%s [ %d ] = %s\n", newVar->yoreName, cur_idx << 2, simpleCal(tmp_node->son));
                    strcat(outBuffer, tmpBuffer);
                }
                else{
                    printf("%s [ %d ] = %s\n", newVar->yoreName, cur_idx << 2, simpleCal(tmp_node->son));
                }
				cur_idx++;
				if(tmp_node->sib){
					tmp_node = tmp_node->sib;
				}
				else{
					for(int i = cur_idx; i < whole_size; ++i)
						newVar->val[i] = 0;
					cur_idx = whole_size;
				}
			}
            else if (tmp_node->type == 12){//EQ(tmp_node->attr, "InitVal_{}")
				if(cur_idx % cur_sz){
					for(int i = cur_idx; i < cur_sz; ++i){
						newVar->val[i] = 0;
					}
					cur_idx += cur_sz - cur_idx;
				}
                cur_idx += cur_sz;
                if(tmp_node->sib){
                    tmp_node = tmp_node->sib;
                }
            }
			// meet '{'
			else if (tmp_node->type == 13){//EQ(tmp_node->attr, "InitVal_{v}")
				// put cur_sz values as 0
				if(cur_idx % cur_sz){
					for(int i = cur_idx; i < cur_sz; ++i){
						newVar->val[i] = 0;
					}
					cur_idx += cur_sz - cur_idx;
				}
				// jump into the next layer 
				cur_idx = arrayVal(tmp_node, newVar, cur_idx, 1, cur_sz);
				if(tmp_node->sib){
					tmp_node = tmp_node->sib;
				}
				else{
					for(int i = cur_idx; i < whole_size; ++i)
						newVar->val[i] = 0;
					cur_idx = whole_size;
				}
			}
		}
        #ifdef DEBUG
            printf("__________makeVar_int[]=:[%s %s size:%d]\n[", newVar->oriName, newVar->yoreName, newVar->size);
            for(int i = 0; i < newVar->size; ++i){
                printf("%d ", newVar->val[i]);
            }
            printf("]\n");
        #endif
        break;
    default:
        break;
    }
    VarList[VarCnt++] = newVar;
}

void scanFDEF(Node* node){
	#ifdef DEBUG
		printf("_________scanFDEF %s %u\n", node->attr, node->type);
	#endif
    Node* p_node = node->son;
    inFunc = 1;
    outBuffer = (char*)malloc(sizeof(char)*(7<<16));
    memset(outBuffer, 0, sizeof(sizeof(char)*(7<<16)));
    // new a Function
    Func* newFunc = (Func*)malloc(sizeof(Func));
    newFunc->funcName = strdup(node->son->attr);
    newFunc->paramNum = 0;
    newFunc->returnInt = 0;
    FuncParamIdx = GblParamCnt;
    newFunc->paramIdx = FuncParamIdx;
    // parameters
    switch (node->type)
    {
        case 14://int()
            newFunc->returnInt = 1;
        case 15://void()
            node = node->son;
            break;
        case 16://int(p)
            newFunc->returnInt = 1;
        case 17://void(p)
            node = node->son;
            while(node->sib && node->sib->type != 21){
                newFunc->paramNum++;
                // make parameters
                node = node->sib; // p
                #ifdef DEBUG
                    printf("__________scanFDEF_scanPara %s %u\n", node->attr, node->type);
                #endif
                Para* newPara = (Para*)malloc(sizeof(Para));
                char temp_pN[32] = {'\0'};
                sprintf(temp_pN, "%s", node->son->attr);
                newPara->oriName = strdup(temp_pN);
                sprintf(temp_pN, "p%d", FuncParamCnt++);
                newPara->yoreName = strdup(temp_pN);
                p_node = node;
                switch (node->type)
                {
                case 20: // p
                    newPara->dimension = 0;
                    newPara->dimesize = (int*)malloc(sizeof(int));
                    newPara->dimesize[0] = 1;
                    break;
                case 19: // p[]
                    newPara->dimension = 1;
                    newPara->dimesize = (int*)malloc(sizeof(int));
                    newPara->dimesize[0] = 1;
                    break;
                case 18:// p[][]
                    newPara->dimension = 1;
                    int whole_size = 1;
                    p_node = node->son;
                    while(p_node->sib){
                        newPara->dimension++;
                        p_node = p_node->sib;
                        whole_size *= calAlg(p_node->son);
                    }
                    newPara->dimesize = (int*)malloc(sizeof(int)*newPara->dimension);
                    p_node = node->son;
                    int tmp_sz = whole_size;
                    for(int i = 0; i < newPara->dimension - 1; ++i){
                        p_node = p_node->sib;
                        newPara->dimesize[i] = tmp_sz;
                        tmp_sz /= calAlg(p_node->son);
                    }
                    break;
                default:
                    break;
                }
                #ifdef DEBUG
                    printf("__________scanFDEF_makePara [%s %s]\n", newPara->oriName, newPara->yoreName);
                #endif
                ParamList[GblParamCnt++] = newPara;
            }
            break;
        default:
            break;
    }
	#ifdef DEBUG
		printf("_________scanFDEF_finished %s %d\n", newFunc->funcName, newFunc->paramNum);
	#endif
    printf("f_%s [%d]\n", newFunc->funcName, FuncParamCnt);
    FuncList[FuncCnt++] = newFunc;
    while(node->sib){
        node = node->sib;
        scanNode(node);
    }
    printf("%s", outBuffer);
    memset(outBuffer, 0, sizeof(sizeof(char)*(7<<16)));
    if(newFunc->returnInt == 0){
        printf("\treturn\n");
    }
    printf("end f_%s\n", newFunc->funcName);
    inFunc = 0;
    FuncParamCnt = 0;
    free(outBuffer);
}

// = 
void scanASG(Node* node){
	#ifdef DEBUG
		printf("_________scanASG:[%s %u]\n", node->attr, node->type);
	#endif
    node = node->son;
    if(node->type == 32){//lval
        if(inFunc){
            for(int i = FuncParamCnt + FuncParamIdx - 1; i >= FuncParamIdx; --i){
                if(EQ(ParamList[i]->oriName, node->son->attr)){
                    sprintf(tmpBuffer, "\t%s = %s\n", ParamList[i]->yoreName, simpleCal(node->sib));
                    strcat(outBuffer, tmpBuffer);
                    return;
                }
            }
        }
        for(int i = VarCnt - 1; i >= 0; --i){
            if(EQ(VarList[i]->oriName, node->son->attr) && VarList[i]->valid != -1){
                if(inFunc){
                    VarList[i]->val[0] = calAlg(node->sib);
                    sprintf(tmpBuffer, "\t%s = %s\n", VarList[i]->yoreName, simpleCal(node->sib));
                    strcat(outBuffer, tmpBuffer);
                }
                else{
                    printf("%s = %s\n", VarList[i]->yoreName, simpleCal(node->sib));
                }
                return;
            }
        }
    }
    else if(node->type == 33){//lval[]
        Node* tmp_node = node->son;
        if(inFunc){
            for(int i = FuncParamCnt + FuncParamIdx - 1; i >= FuncParamIdx; --i){
                if(EQ(ParamList[i]->oriName, node->son->attr)){
                    if(ParamList[i]->dimension == 0){
                        sprintf(tmpBuffer, "\t%s = %s\n", ParamList[i]->yoreName, simpleCal(node->sib));
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        int print_tvc = TmpVarCnt++;
                        printf("\tvar t%d\n", print_tvc);
                        sprintf(tmpBuffer, "\tt%d = 0\n", print_tvc);
                        strcat(outBuffer, tmpBuffer);
                        char tmp_res[64] = {'\0'};
                        for(int j = 0; j < ParamList[i]->dimension; ++j){
                            tmp_node = tmp_node->sib;
                            sprintf(tmp_res, "%s", simpleCal(tmp_node->son));
                            sprintf(tmpBuffer, "\tt%d = t%d + %s\n", print_tvc, print_tvc, tmp_res);
                            strcat(outBuffer, tmpBuffer);
                        }
                        sprintf(tmpBuffer, "\tt%d = t%d * 4\n", print_tvc, print_tvc);
                        strcat(outBuffer, tmpBuffer);
                        sprintf(tmpBuffer, "\t%s [ t%d ] = %s\n", ParamList[i]->yoreName, print_tvc, simpleCal(node->sib));
                        strcat(outBuffer, tmpBuffer);
                    }
                    return;
                }
            }
        }
        tmp_node = node->son;
        for(int i = VarCnt - 1; i >= 0; --i){
            if(EQ(VarList[i]->oriName, node->son->attr) && VarList[i]->valid != -1){
                int var_idx = 0;
                int sib_cnt = 0;
                int* acm_sz = (int*)malloc(sizeof(int) * VarList[i]->dimension);
                int dim_1 = (VarList[i]->dimension)-1;
                acm_sz[dim_1] = 1;
                for(int j = dim_1 - 1; j >= 0; --j){
                    acm_sz[j] = acm_sz[j+1]*VarList[i]->dimensize[j+1];
                }
                char printtmp[64] = {'\0'};
                sprintf(printtmp, "t%d", TmpVarCnt++);
                if(inFunc){
                    printf("\t");
                }
                printf("var %s\n", printtmp);
                if(inFunc){
                    sprintf(tmpBuffer, "\t%s = 0\n", printtmp);
                    strcat(outBuffer, tmpBuffer);
                }
                else{
                    printf("%s = 0\n", printtmp);
                }
                while(tmp_node->sib){
                    tmp_node = tmp_node->sib;
                    var_idx += acm_sz[sib_cnt] * calAlg(tmp_node->son);
                    char simple_res[64] = {'\0'};
                    sprintf(simple_res, "%s", simpleCal(tmp_node->son));
                    if(inFunc){
                        printf("\t");
                    }
                    printf("var t%d\n", TmpVarCnt);
                    if(inFunc){
                        sprintf(tmpBuffer, "\tt%d = %d * %s\n", TmpVarCnt, acm_sz[sib_cnt], simple_res);
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        printf("t%d = %d * %s\n", TmpVarCnt, acm_sz[sib_cnt], simple_res);
                    }
                    if(inFunc){
                        sprintf(tmpBuffer, "\t%s = %s + t%d\n", printtmp, printtmp, TmpVarCnt);
                        strcat(outBuffer, tmpBuffer);
                    }
                    else{
                        sprintf(tmpBuffer, "%s = %s + t%d\n", printtmp, printtmp, TmpVarCnt);
                        strcat(outBuffer, tmpBuffer);
                    }
                    TmpVarCnt++;
                    sib_cnt++;
                }
                if(var_idx >= 0){
                    VarList[i]->val[var_idx] = calAlg(node->sib);
                }
                #ifdef DEBUG
                    printf("__________scanASG_ind[]:[%s, %d]\n",VarList[i]->yoreName, var_idx << 2);
                #endif
                if(inFunc){
                    sprintf(tmpBuffer, "\t%s = %s * 4\n", printtmp, printtmp);
                    strcat(outBuffer, tmpBuffer);
                    sprintf(tmpBuffer, "\t%s [ %s ] = %s\n", VarList[i]->yoreName, printtmp, simpleCal(node->sib));
                    strcat(outBuffer, tmpBuffer);
                }
                else{
                    printf("%s = %s * 4\n", printtmp, printtmp);
                    printf("%s [ %s ] = %s\n", VarList[i]->yoreName, printtmp, simpleCal(node->sib));
                }
            }
        }
    }
}

void scanBreak(Node* node){
    int label = LabelCnt++;
    if(inFunc){ 
        sprintf(tmpBuffer, "\t");
        strcat(outBuffer, tmpBuffer);
    }
    sprintf(tmpBuffer, "goto l%d\n", label);
    strcat(outBuffer, tmpBuffer);
    isBreak = label;
    return;
}

void scanIf(Node* node){
    inCond = 1;
    int label = LabelCnt++;
    condLabel = label;
    char temp_res[64] = {'\0'};
    sprintf(temp_res, "%s", simpleCal(node->son));
    if(inFunc){ 
        sprintf(tmpBuffer, "\t");
        strcat(outBuffer, tmpBuffer);
    }
    sprintf(tmpBuffer, "if %s == 0 goto l%d\n", temp_res, label);
    strcat(outBuffer, tmpBuffer);
    scanNode(node->son->sib);
    sprintf(tmpBuffer, "l%d:\n", label);
    strcat(outBuffer, tmpBuffer);
    inCond = 0;
    condLabel = 0;
    return;
}

void scanIfElse(Node* node){
    inCond = 1;
    int label1 = LabelCnt++;
    condLabel = label1;
    char temp_res[64] = {'\0'};
    sprintf(temp_res, "%s", simpleCal(node->son));
    if(inFunc){ 
        sprintf(tmpBuffer, "\t");
        strcat(outBuffer, tmpBuffer);
    }
    sprintf(tmpBuffer, "if %s == 0 goto l%d\n", temp_res, label1);
    strcat(outBuffer, tmpBuffer);
    scanNode(node->son->sib);
    
    int label2 = LabelCnt++;
    condLabel = label2;
    if(inFunc){ 
        sprintf(tmpBuffer, "\t");
        strcat(outBuffer, tmpBuffer);
    }
    sprintf(tmpBuffer, "goto l%d\n", label2);
    strcat(outBuffer, tmpBuffer);

    sprintf(tmpBuffer, "l%d:\n", label1);
    strcat(outBuffer, tmpBuffer);
    scanNode(node->son->sib->sib);

    sprintf(tmpBuffer, "l%d:\n", label2);
    strcat(outBuffer, tmpBuffer);
    inCond = 0;
    condLabel = 0;
    return;
}

void scanWhile(Node* node){
    inCond = 1;
    int label1 = LabelCnt++;
    int label2 = LabelCnt++;
    condLabel = label2;
    whileLabel = label1;
    sprintf(tmpBuffer, "l%d:\n", label1);
    strcat(outBuffer, tmpBuffer);
    char temp_res[64] = {'\0'};
    sprintf(temp_res, "%s", simpleCal(node->son));
    if(inFunc){ 
        sprintf(tmpBuffer, "\t");
        strcat(outBuffer, tmpBuffer);
    }
    sprintf(tmpBuffer, "if %s == 0 goto l%d\n", temp_res, label2);
    strcat(outBuffer, tmpBuffer);
    scanNode(node->son->sib);
    if(inFunc){ 
        sprintf(tmpBuffer, "\t");
        strcat(outBuffer, tmpBuffer);
    }
    sprintf(tmpBuffer, "goto l%d\n", label1);
    strcat(outBuffer, tmpBuffer);
    sprintf(tmpBuffer, "l%d:\n", label2);
    strcat(outBuffer, tmpBuffer);
    if(isBreak){
        sprintf(tmpBuffer, "l%d:\n", isBreak);
        strcat(outBuffer, tmpBuffer);
        isBreak = 0;
    }
    inCond = 0;
    condLabel = 0;
    return;
}

void scanNode(Node* node){
	#ifdef DEBUG
		printf("________scanNode:%s, %u\n",node->attr,node->type);
	#endif
    char* return_var = (char*)malloc(sizeof(char)*64);
	switch (node->type){
        // fucntion definition
		case 14:
        case 15:
        case 16:
        case 17:
            scanFDEF(node);
            break;
        // (const) variable decalring
        case 1:
        case 6:
            node = node->son;
            while(node){
                #ifdef DEBUG
                    printf("_________scanNode_Decl:%s, %u\n",node->attr,node->type);
                #endif
                makeVar(node);
                node = node->sib;
            }
            break;
        // block
        case 21:
            #ifdef DEBUG
                printf("________scanNode_block:%s, %u\n",node->attr,node->type);
            #endif
            blkLayer++;
            node = node->son;
            while(node){
                scanNode(node);
                node = node->sib;
            }
            for(int i = VarCnt - 1; i >= 0; --i){
                if(VarList[i]->valid >= blkLayer){
                    VarList[i]->valid = -1;
                }
            }
            blkLayer--;
            break;
        // = LVal ASG Exp SEM
        case 22:
            scanASG(node);
            break;
        // exp
        case 24:
            scanNode(node->son);
            break;
        // if
        case 25:
            scanIf(node);
            break;
        // if else
        case 26:
            scanIfElse(node);
            break;
        // while
        case 27:
            scanWhile(node);
            whileLabel = 0;
            break;
        // break
        case 28:
            scanBreak(node);
            break;
        //continue
        case 29:
            if(inFunc){
                sprintf(tmpBuffer, "\t");
                strcat(outBuffer, tmpBuffer);
                sprintf(tmpBuffer, "goto l%d\n", whileLabel);
                strcat(outBuffer, tmpBuffer);
            }
            else{
                printf("goto l%d\n", whileLabel);
            }
            break;
        // return;
        case 30:
            #ifdef DEBUG
                printf("________scanNode_return:%s, %u\n",node->attr,node->type);
            #endif
            sprintf(tmpBuffer, "\treturn\n");
            strcat(outBuffer, tmpBuffer);
            break;
        // return x;
        case 31:
            #ifdef DEBUG
                printf("________scanNode_return_x:%s, %u\n",node->attr,node->type);
            #endif
            sprintf(return_var, "%s", simpleCal(node->son));
            sprintf(tmpBuffer, "\treturn %s\n", return_var);
            strcat(outBuffer, tmpBuffer);
            break;
        // unary void function
        case 34:
        // unary function
        case 35:
            return_var = simpleCal(node);
            if(inFunc){
                sprintf(tmpBuffer, "\t");
                strcat(outBuffer, tmpBuffer);
            }
            sprintf(tmpBuffer, "%s\n", return_var);
            strcat(outBuffer, tmpBuffer);
            break;
		default:
			break;
	}
	// printf("_________scanEnd:%s, %u\n",node->attr,node->type);
}

void scanTree(Node* root){
	initEnv();
    #ifdef GRAPH
        getGraph(root);
    #endif
	#ifdef DEBUG
		printf("==================Print Tree===============\n");
		printTree(root, 0);
		printf("===================Scan Tree===============\n");
	#endif
	root = root->son;
	while(root){
		scanNode(root);
		// printf("_________scanRootEnd:%s, %u\n",root->attr,root->type);
		root = root->sib;
	}
	printf("\n");
}

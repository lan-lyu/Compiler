#include "parse.h"
#define EQ(a,b) (strcmp(a,b)==0)
// #define DEBUG

Expr* ExprList[80000];
int stk = 0;
void makeEXPR(int type, const char* attr, const char* op, const char* src1, const char* src2, const char* dst)
{
	int i = exprCnt;
	++exprCnt;
	ExprList[i] = (Expr*)malloc(sizeof(Expr));
    ExprList[i]->attr = attr;
	ExprList[i]->index = i;
	ExprList[i]->type = type;
	ExprList[i]->op = op;
	ExprList[i]->src1 = src1;
	ExprList[i]->src2 = src2;
	ExprList[i]->dst = dst;
	return;
}

void printExpr(){
    for(int i = 0; i < exprCnt; ++i){
        Expr* tmpE = ExprList[i];
        printf("%d:[attr:%s, type:%d, op:%s, src1:%s, src2:%s, dst:%s]\n", i, tmpE->attr, tmpE->type, tmpE->op, tmpE->src1, tmpE->src2, tmpE->dst);
    }
}

void scanROI(int idx){
    Expr* exp = ExprList[idx];
    int imm = atoi(exp->src2);
    if(imm <= 2047 && imm >= -2048 && (EQ(exp->op, "+")||EQ(exp->op, "<"))){
        if(EQ(exp->op, "+")){
            printf("\taddi    %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
        }
        else if(EQ(exp->op, "<")){
            printf("\tslti    %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
        }
    }
    else{
        printf("\tli      s0,%s", exp->src2);
        if(EQ(exp->op, "+")){
            printf("\tadd     %s,%s,s0\n", exp->dst, exp->src1);
        }
        else if(EQ(exp->op, "-")){
            printf("\tsub     %s,%s,s0\n", exp->dst, exp->src1);
        }
        else if(EQ(exp->op, "*")){
            printf("\tmul     %s,%s,s0\n", exp->dst, exp->src1);
        }
        else if(EQ(exp->op, "/")){
            printf("\tdiv     %s,%s,s0\n", exp->dst, exp->src1);
        }
        else if(EQ(exp->op, "%")){
            printf("\trem     %s,%s,s0\n", exp->dst, exp->src1);
        }
        else if(EQ(exp->op, "<")){
            printf("\tslt     %s,%s,s0\n", exp->dst, exp->src1);
        }
        else if(EQ(exp->op, ">")){
            printf("\tsgt     %s,%s,s0\n", exp->dst, exp->src1);
        }
        else if(EQ(exp->op, "<=")){
            printf("\tsgt     %s,%s,s0\n", exp->dst, exp->src1);
            printf("\tseqz    %s,%s", exp->dst, exp->dst);
        }
        else if(EQ(exp->op, ">=")){
            printf("\tslt     %s,%s,s0\n", exp->dst, exp->src1);
            printf("\tseqz    %s,%s", exp->dst, exp->dst);
        }
        // TODO: ADD %s conflict
        // else if(EQ(exp->op, "&&"))
        // {
        //     printf("\tsnez    %s,%s\n", exp->dst, exp->src1);
        //     printf("\tsnez    s0,%s\n", exp->src2);
        //     printf("\tand     %s,%s,s0\n", exp->dst, exp->dst);
        // }
        else if(EQ(exp->op, "||"))
        {
            printf("\tor      %s,%s,s0\n", exp->dst, exp->src1);
            printf("\tsnez    %s,%s\n", exp->dst, exp->dst);
        }
        else if(EQ(exp->op, "!="))
        {
            printf("\txor     %s,%s,s0\n", exp->dst, exp->src1);
            printf("\tsnez    %s,%s\n", exp->dst, exp->dst);
        }
        else if(EQ(exp->op, "=="))
        {
            printf("\txor     %s,%s,s0\n", exp->dst, exp->src1);
            printf("\tseqz    %s,%s\n", exp->dst, exp->dst);
        }
        return;
    }
}

void scanROR(int idx){
    Expr* exp = ExprList[idx];
    if(EQ(exp->op, "+")){
        printf("\tadd     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
    }
    else if(EQ(exp->op, "-")){
        printf("\tsub     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
    }
    else if(EQ(exp->op, "*")){
        printf("\tmul     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
    }
    else if(EQ(exp->op, "/")){
        printf("\tdiv     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
    }
    else if(EQ(exp->op, "%")){
        printf("\trem     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
    }
    else if(EQ(exp->op, "<")){
        printf("\tslt     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
    }
    else if(EQ(exp->op, ">")){
        printf("\tsgt     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
    }
    else if(EQ(exp->op, "<=")){
        printf("\tsgt     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
        printf("\tseqz    %s,%s", exp->dst, exp->dst);
    }
    else if(EQ(exp->op, ">=")){
        printf("\tslt     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
        printf("\tseqz    %s,%s", exp->dst, exp->dst);
    }
	else if(EQ(exp->op, "&&"))
	{
		printf("\tsnez    %s,%s\n", exp->dst, exp->src1);
		printf("\tsnez    s0,%s\n", exp->src2);
		printf("\tand     %s,%s,s0\n", exp->dst, exp->dst);
	}
	else if(EQ(exp->op, "||"))
	{
		printf("\tor      %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
		printf("\tsnez    %s,%s\n", exp->dst, exp->dst);
	}
	else if(EQ(exp->op, "!="))
	{
		printf("\txor     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
		printf("\tsnez    %s,%s\n", exp->dst, exp->dst);
	}
	else if(EQ(exp->op, "=="))
	{
		printf("\txor     %s,%s,%s\n", exp->dst, exp->src1, exp->src2);
		printf("\tseqz    %s,%s\n", exp->dst, exp->dst);
	}
    return;
}

void scanRORgoto(int idx){
    Expr* exp = ExprList[idx];
    if(EQ(exp->op, "<")){
        printf("\tblt     %s,%s,.%s\n", exp->src1, exp->src2, exp->dst);
    }
    else if(EQ(exp->op, ">")){
        printf("\tbgt     %s,%s,.%s\n", exp->src1, exp->src2, exp->dst);
    }
    else if(EQ(exp->op, "<=")){
        printf("\tble     %s,%s,.%s\n", exp->src1, exp->src2, exp->dst);
    }
    else if(EQ(exp->op, ">=")){
        printf("\tbge     %s,%s,.%s\n", exp->src1, exp->src2, exp->dst);
    }
    else if(EQ(exp->op, "!=")){
        printf("\tbne     %s,%s,.%s\n", exp->src1, exp->src2, exp->dst);
    }
    else if(EQ(exp->op, "==")){
        printf("\tbeq     %s,%s,.%s\n", exp->src1, exp->src2, exp->dst);
    }
    return;
}

void scanExpr(){
	#ifdef DEBUG
		printf("==================Print Expr===============\n");
        printExpr();
		printf("===================Scan Expr===============\n");
	#endif
    for(int i = 0; i < exprCnt; ++i){
        Expr* exp = ExprList[i];
        switch (ExprList[i]->type)
        {
        // global var = int
        case 0:
            printf("\t.global  %s\n", exp->dst);
            printf("\t.section .sdata\n");
            printf("\t.align   2\n");
            printf("\t.type    %s,@object\n", exp->dst);
            printf("\t.size    %s,4\n", exp->dst);
            printf("%s:\n", exp->dst);
            printf("\t.word    %s\n", exp->src1);
            break;
        // global var = int[]
        case 1:
            //.comm global_var, int, 4
            printf("\t.comm    %s,%s,4\n", exp->dst, exp->src1);
            break;
        // function head
        case 2:
            stk = (atoi(exp->src2)/4+1)*16;
            printf("\t.text\n");
            printf("\t.align  2\n");
            printf("\t.global %s\n", exp->op);
            printf("\t.type   %s,@function\n", exp->op);
            printf("%s:\n", exp->op);
            if(stk > 2047 || stk < -2048){
                printf("\tsw      ra,-4(sp)\n");
                printf("\tli      s0,%d\n", stk*(-1));
                printf("\tadd     sp,sp,s0\n");
            }
            else{
                printf("\taddi    sp,sp,-%d\n", stk);
                printf("\tsw      ra,%d(sp)\n", stk-4);
            }
            break;
        // fucntion end
        case 3:
            printf("\t.size   %s,.-%s\n", exp->op, exp->op);
            break;
        // reg = reg op reg
        case 4:
            scanROR(i);
            break;
        // reg = reg op imm
        case 5:
            scanROI(i);
            break;
        // reg = op reg
        case 6:
            if(EQ(exp->op, "-")){
                printf("\tneg     %s,%s\n", exp->dst, exp->src1);
            }
            else if(EQ(exp->op, "!")){
                printf("\tseqz    %s,%s\n", exp->dst, exp->src1);
            }
            break;
        // reg = reg
        case 7:
            printf("\tmv      %s,%s\n", exp->dst, exp->src1);
            break;
        // reg = imm
        case 8:
            printf("\tli      %s,%s\n", exp->dst, exp->src1);
            break;
        // reg[] = reg
        case 9:
            if(atoi(exp->src1) > 2047 || atoi(exp->src1) < -2048){
                printf("\tli      s0,%s\n", exp->src1);
                printf("\tadd     s0,s0,%s\n", exp->dst);
                printf("\tsw      %s,0(s0)\n", exp->src2);
            }
            else{
                printf("\tsw      %s,%s(%s)\n", exp->src2, exp->src1, exp->dst);
            }
            break;
        // reg = reg[]
        case 10:
            if(atoi(exp->src2) > 2047 || atoi(exp->src2) < -2048){
                printf("\tli      s0,%s\n", exp->src2);
                printf("\tadd     s0,s0,%s\n", exp->src1);
                printf("\tlw      %s,0(s0)\n", exp->dst);
            }
            else{
                printf("\tlw      %s,%s(%s)\n", exp->dst, exp->src2, exp->src1);
            }
            break;
        // if ROR goto
        case 11:
            scanRORgoto(i);
            break;
        // goto
        case 12:
            printf("\tj       .%s\n", exp->op);
            break;
        // label:
        case 13:
            printf(".%s:\n", exp->op);
            break;
        // call function
        case 14:
            printf("\tcall    %s\n", exp->op);
            break;
        // return
        case 15:
            if(stk > 2047 || stk < -2048){
                printf("\tli      s0,%d\n", stk);
                printf("\tadd     sp,sp,s0\n");
                printf("\tlw      ra,-4(sp)\n");
            }
            else{
                printf("\tlw      ra,%d(sp)\n", stk-4);
                printf("\taddi    sp,sp,%d\n",stk);
            }
            printf("\tret\n");
            break;
        // store reg int10
        case 16:
            if(atoi(exp->src1) > 511 || atoi(exp->src1) < -512){
                printf("\tli      s0,%d\n", atoi(exp->src1)*4);
                printf("\tadd     s0,s0,sp\n");
                printf("\tsw      %s,0(s0)\n", exp->op);
            }
            else{
                printf("\tsw      %s,%d(sp)\n", exp->op, atoi(exp->src1)*4);
            }
            break;
        // load imm reg
        case 17:
            if(atoi(exp->op) > 511 || atoi(exp->op) < -512){
                printf("\tli      s0,%d\n", atoi(exp->op)*4);
                printf("\tadd     s0,s0,sp\n");
                printf("\tlw      %s,0(s0)\n", exp->src1);
            }
            else{
                printf("\tlw      %s,%d(sp)\n", exp->src1, atoi(exp->op)*4);
            }
            break;
        // load global_var reg
        case 18:
            printf("\tlui     %s,%%hi(%s)\n", exp->src1, exp->op);
            printf("\tlw      %s,%%lo(%s)(%s)\n", exp->src1, exp->op, exp->src1);
            break;
        //load addr imm reg
        case 19:
            if(atoi(exp->op) > 511 || atoi(exp->op) < -512){
                printf("\tli      s0,%d\n", atoi(exp->op)*4);
                printf("\tadd    %s,sp,s0\n", exp->src1);
            }
            else{
                printf("\taddi    %s,sp,%d\n", exp->src1, atoi(exp->op)*4);
            }
            break;
        //load addr var reg
        case 20:
            printf("\tla      %s,%s\n", exp->src1, exp->op);
            break;
        default:
            break;
        }
    }
}
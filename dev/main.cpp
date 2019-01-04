#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "getsym.h"
#include "syntaxAnalysis.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"
#include "mipsCode.h"

FILE* fp;

int main(){
	char path[1024] = "";
	printf("please input a file path:\n");
	scanf("%s",path);
	fp = fopen(path,"r");
//    fp = fopen("test.txt", "r");

	if (fp==NULL){

		return 0;
	} else {
		printf("successfully open the file\n");
	}

    getsym();
    programAnalysis();

    // 输出符号表
    //printSymbolTable();

    // 输出中间代码
    //printMidCode();

    MidCode2File();
    printf("\n>>> FINISH MID CODE.\n");

    // 根据中间代码生成目标mips指令
    genMips();
    printf("\n>>> FINISH MIPS CODE.\n");
    Mips2File();

    //printMipsCode();

    // 优化部分
    printf("\n>>> BEGIN OPTIMIZATION.\n");
    printf("\n>>> BEGIN OPTIMIZATION MID CODE.\n");
    optimize_const();

    printf("\n>>> BEGIN OPTIMIZATION MIPS CODE.\n");
    optimize_sp();

    Mips2File_optimize();

	return 0;
}

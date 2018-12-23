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
//    globalFuncField = "Global";

	char path[1024] = "";
	printf("please input a file path:\n");
	scanf("%s",path);
	fp = fopen(path,"r");
//    fp = fopen("testBUG.txt", "r");

	if (fp==NULL){
		err(5);
		return 0;
	} else {
		printf("successfully open the file\n");
	}

    getsym();
    programAnalysis();
    // 生成符号表
    //printSymbolTable();
    // 生成中间代码
    //printMidCode();
    MidCode2File();
    printf("\n>>> FINISH MID CODE.\n");
    // 根据中间代码生成目标mips指令
    genMips();
    printf("\n>>> FINISH MIPS CODE.\n");
    Mips2File();
    //printMipsCode();
//    printGlobalRecord();


	return 0;
}

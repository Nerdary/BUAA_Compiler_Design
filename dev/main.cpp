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
    // ���ɷ��ű�
    //printSymbolTable();
    // �����м����
    //printMidCode();
    MidCode2File();
    printf("\n>>> FINISH MID CODE.\n");
    // �����м��������Ŀ��mipsָ��
    genMips();
    printf("\n>>> FINISH MIPS CODE.\n");
    Mips2File();
    //printMipsCode();
//    printGlobalRecord();


	return 0;
}

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

    // ������ű�
    //printSymbolTable();

    // ����м����
    //printMidCode();

    MidCode2File();
    printf("\n>>> FINISH MID CODE.\n");

    // �����м��������Ŀ��mipsָ��
    genMips();
    printf("\n>>> FINISH MIPS CODE.\n");
    Mips2File();

    //printMipsCode();

    // �Ż�����
    printf("\n>>> BEGIN OPTIMIZATION.\n");
/*
    // �Ż��м����
    optimize_const();
    // �Ż����м�������Ҫ�������ֶ���ֵ���ٴε���genmips
    midCodeVec.clear();
    midCodeVec = midCodeVec_Optimize;
    genMips();
    Mips2File_optimize();
*/
    // �Ż�mips
    optimize_sp();
    Mips2File_optimize();

	return 0;
}

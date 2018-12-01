#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "getsym.h"
#include "syntaxAnalysis.h"
#include "error.h"
#include "symbolTable.h"
#include "midCode.h"

FILE* fp;

int main(){
//    globalFuncField = "Global";

	char path[1024] = "";
//	printf("please input a file path:\n");
//	scanf("%s",path);
//	fp = fopen(path,"r");
    fp = fopen("test.txt", "r");
	if (fp==NULL){
		err(5);
		return 0;
	} else {
		printf("successfully open the file\n");
	}

    getsym();
    programAnalysis();

    printSymbolTable();

    printMidCode();

	return 0;
}

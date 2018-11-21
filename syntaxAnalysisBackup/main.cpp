#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "getsym.cpp"
#include "syntaxAnalysis.cpp"

int result;
FILE* fp;

int main(){
	char path[1024] = "";
	printf("please input a file path:\n");
	scanf("%s",path);
	fp = fopen(path,"r");
	if (fp==NULL){
		err(5);
		return 0;
	} else {
		printf("successfully open the file\n");
	}

    char list[44][10] = {"",	"CONSTSY",		"INTSY",		"CHARSY",		"VOIDSY",		"MAINSY",	// 0~5
                                "IFSY",			"ELSESY",		"WHILESY",		"FORSY",		"SCANFSY",	// 6~10
                                "PRINTSY",		"RETSY",		"",				"",				"",			//11~15
                                "",				"",				"USINTSY",		"ACHARSY",		"IDSY",		//16~20
                                "STRINGSY",		"PLUSSY",		"MINUSSY",		"STARSY",		"DIVISY",	//21~25
                                "LPARSY",		"RPARSY",		"COMMASY",		"SEMISY",		"COLONSY",	//26~30
                                "ASSIFNSY",		"EQUSY",		"LESSSY",		"LOESY",		"MORESY",	//31~35
                                "MOESY",		"LOMSY",		"AEQUSY",		"DBQUOSY",		"LBRACSY",	//36~40
                                "RBRACSY",		"LBRACESY",		"RBRACESY"	};

    char symbols[44][10] = {"",     "",     "",     "",     "",     // 0~4
                            "",     "",     "",     "",     "",     // 5~9
                            "",     "",     "",     "",     "",     //10~14
                            "",     "",     "",     "",     "",     //15~19
                            "",     "",     "+",    "-",    "*",    //20~24
                            "/",    "(",    ")",    ",",    ";",    //25~29
                            ":",    "",     "=",    "<",    "<=",   //30~34
                            ">",    ">=",   "!=",   "==",   "",     //35~39
                            "[",    "]",    "{",    "}",            //40~43
                            };


    getsym();
//    constState();
//    varState();
//    factor();
//    term();
//    expr();
//    condSentence();
//    loopSentence();
//    retValueFuncCall();
/*
    scanSentence();
    scanSentence();
    printSentence();
    printSentence();
    printSentence();
    retSentence();
    retSentence();
*/
/*
    retValueFuncDefine();
    retValueFuncDefine();
    unretValueFuncDefine();
    unretValueFuncDefine();
*/
    complexSentence();



	return 0;
}

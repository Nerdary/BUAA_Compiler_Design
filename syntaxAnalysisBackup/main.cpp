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


/*
	while(1){
		int result = getsym(fp);
	//	printf("test info : result = %d\n", result);
		if(result == -1)	continue;
		else{
			// IDSY
			if(result == 20){
				printf("%d\tIDSY\t%s\n",lc, token);
			}
			// INTSY
			else if (result == 18){
				printf("%d\tUSINTSY\t%s\n",lc, token);
			}
			// STRING
			else if (result == 21){
				printf("%d\tSTRINGSY\t\"%s\"\n",lc, string);
			}
			// a char
			else if (result == 19){
				printf("%d\tACHARSY\t%c\n",lc, string[0]);
			}
			// others
				// reserved
			else if (result >= 1 && result <= 12){
				printf("%d\t%s\t%s\n",lc, list[result],token);
			//	printf("DEBUG-MODE:	result=%d\n", result);
			}
				// opreator
			else if (result >= 22 && result <= 43){
				printf("%d\t%s\t%s\n",lc, list[result],symbols[result]);
			}
			else if (result == -2){
                printf("successfully reach the end of program.\n");
                break;
			}
			else{
			//	printf("unknown error in main\n");
			//	err(0);
                err(7);
			}

		}

	}
*/


	return 0;
}

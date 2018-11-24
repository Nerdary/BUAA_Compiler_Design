#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifndef __getsym__
#define __getsym__
int getsym();


void clearToken();
int isSpace(char a);
int isNewline(char a);
int isTab(char a);
int isLetter(char a);
int isDigit(char a);
void catToken(char a);
int isReserved();
int transNum(char* token);
void err(int index);

char token[200] = "";
char string[200] = "";
char symbol[10] = "";
int lc = 1;
FILE* backupFile;
int backupResult;

int charCount = 0;
int backupCharCount = 0;

// 预读功能的实现
void recordRead(){

    extern int result;
    extern FILE* fp;
    *backupFile = *fp;
    backupResult = result;
    printf(">>> recorded.\tbk=%ld\n", *backupFile);

    backupCharCount = charCount;
}

void resetRead(){
    extern FILE* fp;
    extern int result;

    printf(">>> reset...\tfp=%ld ", *fp);
    result = backupResult;
//    *p = backupFile;
    *fp = *backupFile;
//    fseek(fp,(backupCharCount - charCount),1);
    printf("fp=%ld\n", *fp);
    charCount = backupCharCount;

}

// 词法分析子程序
int getsym(){
//    printf("getsym used.\n");
    extern FILE* fp;
    extern int result;
	char a = fgetc(fp);
	printf("in getsym fp=%ld\n", *fp);								// 读入一个字符
	charCount += 1;

	if(a==EOF){
        result = -2;
		return 0;
	}

	clearToken();										// 清空Token
	while(isSpace(a)||isNewline(a)||isTab(a)){
		if(isNewline(a))
			lc += 1;
		a = fgetc(fp);
		charCount += 1;
	}													// 跳过空格、换行符、制表符
														// 读到第一个有意义的字符

	if(isLetter(a)){
		while(isLetter(a) || isDigit(a)){				// 如果是字母数字，拼接起来
			catToken(a);
		//	a = getchar();
			a = fgetc(fp);
			charCount += 1;
		}
		ungetc(a,fp);									// 指针后退一个字符
		charCount -= 1;

		// 判断保留字
		int resultValue = isReserved();
		if(resultValue == 0){
			strcpy(symbol, "IDSY");
			result = 20;
			return 20;
		}
		else{
			strcpy(symbol, token);
			result = resultValue;
			return resultValue;
		}



	}

	else if(isDigit(a)){								// 如果是数字，拼接起来
		while(isDigit(a)){
			catToken(a);
			//a = getchar();
			a = fgetc(fp);
			charCount += 1;
		}
		ungetc(a,fp);									// 指针回退
		charCount -= 1;
		int num = transNum(token);
		if (num==-1){
			err(1);										// transNum 出错
			result = 0;
			return 0;
		} else {
			strcpy(symbol, "USINTSY");
			result = 18;
			return 18;
		}
	}

	else if(a=='<'){
		a = fgetc(fp);
		charCount += 1;
		if(a=='='){
			strcpy(symbol, "LOESY");
			result = 34;
			return 34;
		}
		else{
			ungetc(a,fp);
			charCount -= 1;
			strcpy(symbol, "LESSSY");
			result = 33;
			return 33;
		}
	}

	else if(a=='>'){
		a = fgetc(fp);
		charCount += 1;
		if(a=='='){
			strcpy(symbol, "MOESY");
			result = 36;
			return 36;
		}
		else{
			ungetc(a,fp);
			charCount -= 1;
			strcpy(symbol, "MORESY");
			result = 35;
			return 35;
		}
	}

	else if(a=='!'){
		a = fgetc(fp);
		charCount += 1;
		if(a=='='){
			strcpy(symbol, "LOMSY");
			result = 36;
			return 36;
		}
		else{
			ungetc(a,fp);
			charCount -= 1;
		//	printf("illegal '!=' \n");
		err(6);
            result = -1;
			return -1;
		}
	}

	else if(a==34){
		a = fgetc(fp);
		charCount += 1;
		int index = 0;
		while(a!=34){
			if(a==32 || a==33 || (a>=35 && a<=126)){
				string[index++] = a;
				a = fgetc(fp);
				charCount += 1;
			} else{
				err(2);
			//	printf("illegal charactor while dealing string\n");
                result = -1;
				return -1;
			}
		}
		strcpy(symbol, "STRINGSY");
		result = 21;
		return 21;
	}

	else if(a==39){
		a = fgetc(fp);
		charCount += 1;
		if(a=='+'||a=='-'||a=='*'||a=='/'||a=='_'||(a>='a'&&a<='z')||(a>='A'&&a<='Z')||(a>='0'&&a<='9')){
			char b = fgetc(fp);
			charCount += 1;
			if(b==39){
				string[0] = a;
				strcpy(symbol, "ACHARSY");
				result = 19;
				return 19;
			} else {
				ungetc(b,fp);
				ungetc(a,fp);
				charCount -= 2;
				err(3);
			//	printf("more than one charactor\n");
                result = -1;
				return -1;
			}
		} else{
			err(4);
		//	printf("illegal charactor in single char\n");
            result = -1;
			return -1;
		}
	}

	else if(a=='+'){
		strcpy(symbol,"PLUSSY" );
		result = 22;
		return 22;
	}
	else if(a=='-'){
		strcpy(symbol, "MINUSSY");
		result = 23;
		return 23;
	}
	else if(a=='*'){
		strcpy(symbol, "STARSY");
		result = 24;
		return 24;
	}
	else if(a=='('){
		strcpy(symbol, "LPARSY");
		result = 26;
		return 26;
	}
	else if(a==')'){
		strcpy(symbol, "RPARSY");
		result = 27;
		return 27;
	}
	else if(a==','){
		strcpy(symbol, "COMMASY");
		result = 28;
		return 28;
	}
	else if(a==';'){
		strcpy(symbol, "SEMISY");
		result = 29;
		return 29;
	}
	else if(a=='='){
		a = fgetc(fp);
		charCount += 1;
		if(a=='='){
			strcpy(symbol, "DBEQUSY");
			result = 38;
			return 38;
		}
		else{
			ungetc(a,fp);
			charCount -= 1;
			strcpy(symbol, "EQUSY");
			result = 32;
			return 32;
		}
	}
	else if(a=='/'){
		strcpy(symbol, "DIVISY");
		result = 25;
		return 25;
	}
	else if(a=='{'){
		strcpy(symbol, "LBRACESY");
		result = 42;
		return 42;
	}
	else if(a=='}'){
		strcpy(symbol, "RBRACESY");
		result = 43;
		return 43;
	}
	else if(a=='['){
		strcpy(symbol, "LBRACSY");
		result = 40;
		return 40;
	}
	else if(a==']'){
		strcpy(symbol, "RBRACSY");
		result = 41;
		return 41;
	}

	else{
        result = 0;
        return 0;
	}


    return result;
}

// 错误处理
void err(int index){
	switch(index){
		case(0):	printf("an unknown error occurred.\n");								break;
		case(1):	printf("error while transforming string into integer.(USINTSY)\n");	break;
		case(2):	printf("illegal character or without end_mark while dealing string.(STRINGSY)\n");		break;
		case(3):	printf("more than one character in ''.(ACHARSY)\n");				break;
		case(4):	printf("illegal character or without end_mark while dealing single char.(ACHARSY)\n");	break;
		case(5):	printf("unable to open the file.(FILE IO)\n");						break;
		case(6):	printf("illegal '!=' \n");											break;
		case(7):    printf("illegal or unexpected character in code.\n");               break;
		default:	printf("an illegal index in err.\n");								break;
	}
}

// 清空TOKEN
void clearToken(){
	memset(token,0,200);
}

//判断是否为空格
int isSpace(char a){
	if(a==' ')
		return 1;
	else
		return 0;
}

//判断是否是换行符
int isNewline(char a){
	if(a=='\n')
		return 1;
	else
		return 0;
}

// 判断是否为制表符
int isTab(char a){
	if(a=='\t')
		return 1;
	else
		return 0;
}

// 判断是否为字母
int isLetter(char a){
	if((a>='a' && a<='z') || (a>='A' && a<='Z') || (a=='_'))
		return 1;
	else
		return 0;
}

// 判断是否为数字
int isDigit(char a){
	if(a>='0' && a<='9')
		return 1;
	else
		return 0;
}

//将读到的字符和已有的拼接上
void catToken(char a){
	int i = strlen(token);
	token[i] = a;
}

//判断是否是保留字
// const	int		char	void	main	if	else	while	for	scanf	printf	return
int isReserved(){

	int length = strlen(token),i;
	char tmp[200] = "";
	for(i=0;i<length;i++){
		if(token[i]>='A' && token[i]<='Z')
			tmp[i] = token[i] - 'A' + 'a';
		else
			tmp[i] = token[i];
	}
//	printf("DEBUG_MODE:	token:%s tmp:%s\n", token, tmp);

	if(strcmp(tmp,"const")==0)
		return 1;
	else if(strcmp(tmp,"int")==0)
		return 2;
	else if(strcmp(tmp,"char")==0)
		return 3;
	else if(strcmp(tmp,"void")==0)
		return 4;
	else if(strcmp(tmp,"main")==0)
		return 5;
	else if(strcmp(tmp,"if")==0)
		return 6;
	else if(strcmp(tmp,"else")==0)
		return 7;
	else if(strcmp(tmp,"while")==0)
		return 8;
	else if(strcmp(tmp,"for")==0)
		return 9;
	else if(strcmp(tmp,"scanf")==0)
		return 10;
	else if(strcmp(tmp,"printf")==0)
		return 11;
	else if(strcmp(tmp,"return")==0)
		return 12;
	else
		return 0;
}

// 将字符串数字转为int
int transNum(char* token){
	int len = strlen(token);
	int num = 0,i;
	for(i=0;i<len;i++){
		if(token[i]>='0' && token[i]<='9'){
			num *= 10;
			num += (token[i] - '0');
		} else{
		//	printf("wrong character in transNum\n");
			return -1;
		}
	}
	return num;
}






/*
	reserved names:
		const			1		CONSTSY
		int				2		INTSY
		char			3		CHARSY
		void			4		VOIDSY
		main			5		MAINSY
		if				6		IFSY
		else			7		ELSESY
		while			8		WHILESY
		for				9		FORSY
		scanf			10		SCANFSY
		printf			11		PRINTSY
		return			12		RETSY

	identifier:
		<your name>		20		IDSY

	string:
		<string name>	21		STRINGSY			string itself

	char:
						19		ACHARSY				ascii of char

	unsigned integer	18		USINTSY				value of interger

	others:
		+				22		PLUSSY
		-				23		MINUSSY
		*				24		STARSY
		/				25		DIVISY
		(				26		LPARSY
		)				27		RPARSY
		,				28		COMMASY
		;				29		SEMISY
		:				30		COLONSY
		:=				31		ASSIGNSY
		= 				32		EQUSY
		<				33		LESSSY
		<=				34		LOESY
		>				35		MORESY
		>=				36		MOESY
		!=				37		LOMSY
		==				38		AEQUSY
		"				39		DBQUOSY
		[				40		LBRACSY
		]				41		RBRACSY
		{				42		LBRACESY
		}				43		RBRACESY

*/
#endif

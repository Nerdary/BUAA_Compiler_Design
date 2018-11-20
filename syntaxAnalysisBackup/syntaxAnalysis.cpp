#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "error.cpp"
#include "getsym.cpp"

#define CONSTSY     1
#define INTSY       2
#define CHARSY      3
#define VOIDSY      4
#define MAINSY      5
#define IFSY        6
#define ELSESY      7
#define WHILESY     8
#define FORSY       9
#define SCANFSY     10
#define PRINTSY     11
#define RETTSY      12

#define USINTSY     18
#define ACHARSY     19
#define IDSY        20
#define STRINGSY    21

#define PLUSSY      22
#define MINUSSY     23
#define STARSY      24
#define DIVISY      25
#define LPARSY      26
#define RPARSY      27
#define COMMASY     28
#define SEMISY      29
#define COLONSY     30
#define ASSIGNSY    31
#define EQUSY       32
#define LESSSY      33
#define LOESY       34
#define MORESY      35
#define MOESY       36
#define LOMSY       37
#define AEQUSY      38
#define DBQUOSY     39
#define LBRACSY     40
#define RBRACSY     41
#define LBRACESY    42
#define RBRACESY    43


extern int result;


int unsignedInt();
int signedInt();
int constDefine();
int constState();
int factor();
int item();
int expr();
int varDefine();
int varState();
int paraList();
int complexSentence();
int retValueFuncDefine();

int unsignedInt(){                  // 文法中的无符号整数
    if(result==USINTSY){
        while(true){
            getsym();
            if(result==USINTSY){
                continue;
            }else   break;
        }
    }else{
        error();
        return -1;
    }
    printf("an unsigned integer.\n");
    return 0;
}

int signedInt(){                    // 对应文法中的整数
    if(result==PLUSSY||result==MINUSSY){
        getsym();
    }

    unsignedInt();
    printf("a signed integer.\n");
    return 0;
}

int constDefine(){
    if(result==INTSY){              // INTSY
        int tmp_count = 0;
                    while(true){
                        getsym();
                        if(result==IDSY){
                            getsym();
                            if(result==EQUSY){
                                getsym();
                                signedInt();
                                // 这里signedInt已经预读了下一个symbol
                                if(result==COMMASY){
                                    tmp_count += 1;
                                    continue;
                                }else{
                                    break;
                                }
                            }else{
                                error();
                                return -1;
                            }
                        }else{
                            error();
                            return -1;
                        }
                    }

    }else if(result==CHARSY){       // CHARSY
        int tmp_count = 0;
                        while(true){
                            getsym();
                            if(result==IDSY){
                                getsym();
                                if(result==EQUSY){
                                    getsym();
                                    if(result==ACHARSY){
                                        getsym();
                                        if(result==COMMASY){
                                            tmp_count += 1;
                                            continue;
                                        }
                                        else    break;
                                    }else {
                                        error();
                                        return -1;
                                    }
                                }else{
                                    error();
                                    return -1;
                                }
                            }else{
                                error();
                                return -1;
                            }
                        }
    }else{
        error();
        return -1;
    }
    printf("a const definition.\n");
    return 0;
}

int constState(){
    int constStateCount = 0;
    while(true){
    //    printf("in constState result = %d\n", result);
        if(result!=CONSTSY) break;
        else{
            getsym();
            constDefine();          // 变量定义中内置了getsym，不用再读入一个
            if(result==SEMISY){
                constStateCount += 1;
                getsym();
                continue;
         else{
            getsym();
        }   }else{
                error();
                return -1;
            }
        }
    }
    printf("constStateCount = %d\n", constStateCount);
    printf("a const statement.\n");
    return 0;
}

int declareHead(){
    if(result==INTSY){
        getsym();
        if(result!=IDSY){
            error();
            retrun -1;
        }else{
            getsym();
        }
    }else if(result==CHARSY){
        getsym();
        if(result!=IDSY){
            error();
            retrun -1;
        }else{
            getsym();
        }
    }else{
        error();
        return -1;
    }
    return 0;
}

int varDefine(){
    if(result!=INTSY&&result!=CHARSY){
        error();
        return -1;
    }
    // 开始识别标识符部分
    getsym();
    while(true){
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();
        if(result==LBRACSY){
            getsym();
            unsignedInt();
            if(result!=RBRACSY){
                error();
                return -1;
            }else{
                getsym();
            }
        }
        if(result==COMMASY){
            continue;
        }else   break;
    }
}

int varState(){

}

int paraList(){
    while(true){
        if(result==INTSY||result==CHARSY){
            getsym();
            if(result==IDSY){
                getsym();
                if(result==COMMASY){
                    // 说明值参数表还没有结束
                    getsym();
                    continue;
                }else{
                    break;
                }
            }else{
                error();
                return -1;
            }
        }else{
            error();
            return -1;
        }
    }
    return 0;
}

int retValueFuncDefine(){
    declareHead();
    // 处理值参数表部分（如果有）
    if(result==LPARSY){
        getsym();
        paraList();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();
    }

    if(result!=LBRACESY){   // "{"
        error();
        return -1;
    }

    complexSentence();
    if(result!=RBRACESY){   // "}"
        error();
        return -1;
    }
    return 0;
}

int unretValueFuncDefine(){

}

int complexSentence(){

}

/*
int factor(){
    if(result==LPARSY){             // result = "("
        getsym();
        expr();                     // 表达式
        if(result==RPARSY){         // result = ")"
            getsym();
        }else error();
    }else if(result==IDSY){         // result = IDSY
        getsym();
        if(result==LBRACSY){        // result = "["
            getsym();
            expr();                 // 表达式
            if(result==RBRACSY){
                getsym();
            }else error();
        }else if(result==LPARSY){   // result = "("
            getsym();
            paralist();             // 值参数表
            if(result==RPARSY)      // result = ")"
                getsym();
            else    error();
        }else if(result==PLUSSY || result==MINUSSY){
            getsym();
            if(result!=USINTSY)     // 有符号整数
                error();
        }else if(result==USINTSY){
            getsym();               // 无符号整数
        }else if(result==ACHARSY){
            getsym();               // 字符 ACHARSY
        }
    }
}
*/

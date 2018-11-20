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
#define RETSY      12

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
int sentenceSequence();
int retValueFuncDefine();
int condSentence();
int condition();
int loopSentence();
int retValueFuncCall();
int unretValueFuncCall();
int assignSentence();
int scanSentence();
int printSentence();
int nullSentence();
int retSentence();
int programAnalysis();
int mainAnalysis();


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
        if(result!=CONSTSY){
            error();
            return -1;
        }
        else{
            getsym();
            constDefine();          // 变量定义中内置了getsym，不用再读入一个
            if(result==SEMISY){
                constStateCount += 1;
                getsym();
                continue;
            }else{
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
            return -1;
        }else{
            getsym();
        }
    }else if(result==CHARSY){
        getsym();
        if(result!=IDSY){
            error();
            return -1;
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
//    varDefine();              // 注意这样改动后是从0个开始的，可能需要一个计数变量
    while(true){
        int judgeTag = -1;      /*  judgeTag==1     下一项还是变量声明
                                    judgeTag==2     下一项是有返回值函数定义
                                    judgeTag==-1    初始状态/两者都不是
                                */
        recordRead();           // 保存读指针
        if(varDefine()==0){
            judgeTag = 1;
            // 预读一个符号，这里不用getsym
            if(result==LPARSY){
                judgeTag = 2;
                return judgeTag;
            }
        }
        resetRead();            // 恢复读指针

        switch(judgeTag){
            case(-1):
                error();
                return -1;
            case(0):
                break;
            case(1):
                varDefine();
                if(result==SEMISY){
                    getsym();
                    continue;
                }else{
                    error();
                    return -1;
                }
            default:    error();return -1;
        }


    }
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
    if(result!=VOIDSY){
        error();
        return -1;
    }
    getsym();
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
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

int sentence(){
    switch(result){
        case(IFSY):             // 条件语句
            condSentence();
            break;
        case(WHILESY):          // 循环语句
            loopSentence();
            break;
        case(FORSY):            // 循环语句
            loopSentence();
            break;
        case(LBRACESY):         // 语句列
            sentenceSequence();
            break;
        case(SEMISY):           // 空语句，直接分号
            nullSentence();
            break;
        case(PRINTSY):          // 写语句
            printSentence();
            break;
        case(SCANFSY):          // 读语句
            scanSentence();
            break;
        case(RETSY):            // 返回语句
            retSentence();
            break;
        case(IDSY):             // 两种情况，函数调用或赋值语句
            recordRead();
            getsym();
            if(result==LPARSY){         // 函数调用
                resetRead();
                retValueFuncCall();
                break;
            }
            else if(result==EQUSY){     // 赋值语句
                resetRead();
                assignSentence();
                break;
            }
            else{
                resetRead();
                return -1;
            }
        default:    return -1;
    }
}

int sentenceSequence(){
    while(true){
        switch(result){
            case(IFSY):     break;  // 条件语句
            case(WHILESY):  break;  // 循环语句
            case(FORSY):    break;  // 循环语句
            case(LBRACESY): break;  // 语句列
            case(SEMISY):   break;  // 空语句，直接分号
            case(PRINTSY):  break;  // 写语句
            case(SCANFSY):  break;  // 读语句
            case(RETSY):    break;  // 返回语句
            case(IDSY):             // 两种情况，函数调用或赋值语句
                recordRead();
                getsym();
                if(result==LPARSY){
                    resetRead();
                    break;
                }
                else if(result==EQUSY){
                    resetRead();
                    break;
                }
                else{
                    resetRead();
                    return 0;
                }
            default:    return 0;
        }
        // 进行到这里说明接下来是语句成分
        sentence();
    }
}

int complexSentence(){
    constState();
    varState();
    // 修改后的两个函数都可以直接调用，如果不符合不会破坏指针的位置
    // 语句列部分
    sentenceSequence();
}

int condSentence(){
    if(result!=IFSY){
        error();
        return -1;
    }

    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
    getsym();
    condition();
    if(result!=RPARSY){
        error();
        return -1;
    }
    getsym();

    sentence();
    // 可选项 else分支
    if(result==ELSESY){
        getsym();
        sentence();
    }
}

int condition(){
    expr();
    if(result==LESSSY||result==LOESY||result==MORESY||
       result==MOESY||result==LOMSY||result==AEQUSY){
        expr();
        return 0;
    }else   return 0;
}

int stepLength(){
    unsignedInt();
}

int loopSentence(){
    if(result==WHILESY){
        getsym();
        if(result!=LPARSY){
            error();
            return -1;
        }
        getsym();
        condition();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();

        sentence();
    }else if(result==FORSY){
        // ENDRIGHTGERE!!!!
        getsym();
        if(result!=LPARSY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();
        if(result!=EQUSY){
            error();
            return -1;
        }
        getsym();
        expr();
        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();
        condition();
        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();
        if(result!=EQUSY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();
        if(result!=PLUSSY&&result!=MINUSSY){
            error();
            return -1;
        }
        getsym();
        stepLength();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();
        sentence();

    }else{
        error();
        return -1;
    }

    return 0;
}

int retValueFuncCall(){
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
    if(result==LPARSY){
        getsym();
        paraList();
        if(result!=RPARSY){
            error();
            return -1;
        }
    }
    return 0;
}

int assignSentence(){
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
    if(result==LBRACSY){
        getsym();
        expr();
        if(result!=RBRACSY){
            error();
            return -1;
        }else   getsym();
    }

    if(result!=EQUSY){
        error();
        return -1;
    }
    getsym();

    expr();
    return 0;
}

int scanSentence(){
    if(result!=SCANFSY){
        error();
        return -1;
    }
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
    getsym();
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
    while(true){
        if(result!=COMMASY) break;
        else{
            getsym();
            if(result!=IDSY){
                error();
                return -1;
            }else{
                getsym();
                continue;
            }
        }
    }

    if(result!=RPARSY){
        error();
        return -1;
    }
    getsym();
    return 0;
}

int printSentence(){
    if(result!=PRINTSY){
        error();
        return -1;
    }
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
    getsym();
    // 接下来判断是一个表达式还是一个字符串
    if(result==STRINGSY){
        getsym();
        if(result==COMMASY){
            getsym();
            expr();
            if(result!=RPARSY){
                error();
                return -1;
            }else{
                getsym();
                return 0;
            }
        }else if(result==RPARSY){
            // end of print
            getsym();
            return 0;
        }else{
            error();
            return -1;
        }
    }else{
        expr();
        if(result!=RPARSY){
            error();
            return -1;
        }else{
            getsym();
            return 0;
        }
    }
    //如果以上分支都不满足进行到了这里，返回未知错误
    return -2;
}

int nullSentence(){
    if(result==SEMISY){
        getsym();
        return 0;
    }else{
        error();
        return -1;
    }
}

int retSentence(){
    if(result!=RETSY){
        error();
        return -1;
    }
    getsym();

    if(result==LPARSY){
        getsym();
        expr();
        if(result!=RPARSY){
            error();
            return -1;
        }else{
            getsym();
            return 0;
        }
    }else{
        return 0;
    }
}

int programAnalysis(){
    int mainTag = 0;
    constState();
    varState();
    // 修改后的两个函数都可以直接调用，如果不符合不会破坏指针的位置
    while(true){
        if(result==INTSY||result==CHARSY){
            retValueFuncDefine();
        }else if(result==VOIDSY){
            recordRead();
            getsym();
            if(result==MAINSY){
                mainTag = 1;
            }
            resetRead();
            // 恢复读之前的初始状态
            if(mainTag==0){
                unretValueFuncDefine();
            }else{  // 下一个就是主函数 void main
                break;
            }

        }else{
            error();
            return -1;
        }
    }

    mainAnalysis();
}

int mainAnalysis(){
    if(result!=VOIDSY){
        error();
        return -1;
    }
    getsym();
    if(result!=MAINSY){
        error();
        return -1;
    }
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
    getsym();
    if(result!=RPARSY){
        error();
        return -1;
    }
    getsym();
    if(result!=LBRACESY){
        error();
        return -1;
    }
    getsym();
    complexSentence();

    if(result!=RBRACESY){
        error();
        return -1;
    }
    getsym();
}

int factor(){
    if(result==LPARSY){             // result = "("
        getsym();
        expr();                     // 表达式
        if(result==RPARSY){         // result = ")"
            getsym();
        }else{
            error();
            return -1;
        }
    }else if(result==IDSY){         // result = IDSY
        getsym();
        if(result==LBRACSY){        // result = "["
            getsym();
            expr();                 // 表达式
            if(result==RBRACSY){
                getsym();
            }else {
                error();
                return -1;
            }
        }else if(result==LPARSY){   // result = "("
            getsym();
            paraList();             // 值参数表
            if(result==RPARSY)      // result = ")"
                getsym();
            else {
                error();
                return -1;
            }
        }else if(result==PLUSSY || result==MINUSSY){
            getsym();
            if(result!=USINTSY){    // 有符号整数
                error();
                return -1;
            }

        }else if(result==USINTSY){
            getsym();               // 无符号整数
        }else if(result==ACHARSY){
            getsym();               // 字符 ACHARSY
        }
    }
    return 0;
}

int term(){
    factor();
    while(true){
        if(result==STARSY||result==DIVISY){
            getsym();
            factor();
        }else   break;

    }
    return 0;
}

int expr(){
    if(result==PLUSSY||result==MINUSSY)
        getsym();
    term();
    while(true){
        if(result==PLUSSY||result==MINUSSY){
            term();
        }else   break;
    }
    return 0;
}


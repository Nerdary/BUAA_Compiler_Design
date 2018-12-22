#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <vector>

#include "error.h"
#include "getsym.h"
#include "symbolTable.h"
#include "syntaxAnalysis.h"
#include "error.h"
#include "midCode.h"


extern FILE* fp;                // 用于跳读


vector<symbolTableItem> symbolTable;
//vector<arrayTableItem> arrayTable;
//vector<funcTableItem> funcTable;

// 栈区
vector<char> stackBrace;        // 大括号栈，用于函数定义判断
vector<int> stackCalc;          // 计算表达式用的运算栈，用来存储将要进行运算的$t的值

vector<int> stackIfElse;        // 用来处理ifelse嵌套的标签问题
vector<funcInfoItem> AllFuncInfo;
                                // 用于在函数定义时存储所有（一部分）信息
vector<int> res;


// 局部变量区
    // 无符号整数值
int unsignValue = 0;
    // 常量
int constValue = 0;
    // 偏移量
int globalOffset = 1;

// 语义分析
    //
int retExist = 0;

// 值计算
int tCount = 0;
//int factorValue = 0;
//int termValue = 0;
//int exprValue = 0;
// 其实只需要类型就可以进行语义分析了
int factorType = 0;
int factorType2 = 0;
int termType = 0;
int exprType = 0;   // 0:default | 1:int | 2:char | 3:else

// 数组越界检查，用两个变量来记一下值和tag
int checkArrayValue = 0;
//int checkArrayTag = 0;
//
int exprCountTerm = 0;
int termCountFactor = 0;


//
//int factorType = 0; // default:0 int:1 char:2 else:3

string currentFuncID;
string callFuncID;
vector<int> funcParaType;

// 生成标签计数器
int labelCount = 1;

// 防止寄存器冲突
int factorFirstTCount = 0;
int exprFirstTCount = 0;

vector<int> factorFirstTCountVector;
vector<int> exprFirstTCountVector;

int nestedExpr = 0;     // 用来记录因子与表达式的嵌套是否发生



// 函数声明区
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
        // 将值取出来
        unsignValue = IntValue;

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

    printf("This is an unsigned integer.\n");
    return 0;
}

int signedInt(){                    // 对应文法中的整数
    int symTag = 1; // 默认为正值
    if(result==PLUSSY){
        getsym();
    } else if(result==MINUSSY){
        symTag = -1;
        getsym();
    }

    unsignedInt();

    constValue = unsignValue * symTag;

    printf("This is a signed integer.\n");
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
                                //printf("2\n");
                                getsym();
                                signedInt();
                                //printf("check: result=%d\n", result);
                                // 这里signedInt已经预读了下一个symbol

                                int pushResult = 0;
                                pushResult = pushConstantTable(IDname, 1, constValue);
                                if(pushResult==-1){
                                    // 说明语义分析没过，变量名重复了
                                }else{
                                    pushMidCodeConst(1, IDname, constValue);
                                }

                                if(result==COMMASY){
                                    //printf("3\n");
                                    tmp_count += 1;

                                    // 这里可以填符号表了
                                //    pushConstantTable(constName, 1, constValue);


                                    continue;
                                }else if(result==SEMISY){
                                    // 这种情况下也要填符号表
                                //    pushConstantTable(constName, 1, constValue);
//                                    int pushResult = 0;
//                                    pushResult = pushConstantTable(IDname, 1, constValue);
//                                    if(pushResult==-1){
//                                        // 说明语义分析没过，变量名重复了
//
//                                    }else{
//                                        pushMidCodeConst(1, IDname, constValue);
//                                    }


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
                            //    constName = token;

                                getsym();
                                if(result==EQUSY){
                                    getsym();
                                    if(result==ACHARSY){
                                        char tmpc = globalChar;
                                        //printf(">>> check tmpc:%c\n", tmpc);
                                        constValue = tmpc;
                                        //printf(">>> check value:%d\n", constValue);

                                        int pushResult = 0;
                                        pushResult = pushConstantTable(IDname, 2, constValue);
                                        if(pushResult==-1){
                                            // 说明语义分析没过，变量名重复了
                                        }else{
                                            pushMidCodeConst(2, IDname, constValue);
                                        }

                                        getsym();
                                        if(result==COMMASY){
                                            tmp_count += 1;

                                        //    pushConstantTable(IDname, 2, constValue);
                                            continue;
                                        }
                                        else if(result==SEMISY){
                                        //    pushConstantTable(IDname, 2, constValue);
                                            break;
                                        }else    break;
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

    printf("This is a const definition.\n");
    return 0;
}

int constState(){
    int constStateCount = 0;
    if(result!=CONSTSY){
        error();
        return -1;
    }
    getsym();
    constDefine();

    if(result!=SEMISY){
        error();
        return -1;
    }
    getsym();
    constStateCount += 1;

    while(true){
    //    printf("in constState result = %d\n", result);
        if(result!=CONSTSY){
            // should continue for next syntax part
            break;
        }
        else{
            getsym();
            constDefine();          // 变量定义中内置了getsym，不用再读入一个
        }

        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();

    }
//    printf("constStateCount = %d\n", constStateCount);
    printf("This is a const statement.\n");
    return 0;
}

int declareHead(){
    int retType = 0;    //
                        // 0: default | 1:int | 2:char

    if(result==INTSY){
        retType = 1;
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }else{
            getsym();
        }
    }else if(result==CHARSY){
        retType = 2;
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

    // 在这里将有返回值函数定义的函数写入符号栈
    globalOffset = 0;
    globalFuncField = IDname;
    globalFuncLevel = 1;
    //
    pushFuncTable(IDname, retType);

    pushMidCodeFuncHead(retType, IDname);
    currentFuncID = IDname;

    printf("This is a head declaration.\n");
    return 0;
}

int varDefine(){
//    printf("in varDefine 1\n");
//    printf(">>>result=%d.\n", result);
    int typeTag = 0;    // 0:default | 1:int | 2:char
    if(result!=INTSY&&result!=CHARSY){
        error();
        return -1;
    }

    if(result==INTSY)   typeTag = 1;
    else                typeTag = 2;
    // 开始识别标识符部分
//    printf("in varDefine 2\n");
    getsym();
//    printf(">>>result=%d.\n", result);
    while(true){
        if(result!=IDSY){
            error();
            return -1;
        }
//        printf("in varDefine 3\n");

        getsym();
        if(result==LBRACSY){
            // 数组变量声明
            getsym();
            unsignedInt();
//            printf("in varDefine 4\n");
            if(result!=RBRACSY){
//                printf("in varDefine 5\n");
                error();
                return -1;
            }else{
                //
            //    printf("CHECK: unsignedValue:%d\n", unsignValue);
                if(pushArrayTable(IDname, typeTag, unsignValue, globalOffset)!=-1){
                    // 成功登记入符号表
                    pushMidCodeArray(typeTag, IDname, unsignValue);
                }

                getsym();
            }
        }else{
            // 普通变量声明
            int pushResult = 0;
            pushResult = pushVarTable(IDname, typeTag, globalOffset, 0);
            if(pushResult!=-1){
                pushMidCodeVar(typeTag, IDname);
            }
        }
        if(result==COMMASY){
//            printf("in varDefine 6\n");
            getsym();
            continue;
        }else   break;
    }


    printf("This is a variable definition.\n");
    return 0;
}

int varState(){
//    printf("in var state.\n");

    if(varDefine()!=0){
    //    printf("something went wrong in the first variable definition.\n");
        error();
        return -1;
    }

    if(result==SEMISY){
        getsym();
    }else{
        error();
        return -1;
    }

    while(true){
//        printf("result = %d\n", result);

        int varStateTag = 0;
        recordRead();
        if(result==INTSY||result==CHARSY){
            getsym(1);
            if(result==IDSY){
                getsym(1);
                if(result==LPARSY || result==LBRACESY) varStateTag = 1;
            }else   varStateTag = 1;
        }else   varStateTag = 1;

        resetRead();

//        printf("varStateTag=%d\n", varStateTag);
        if(varStateTag==1){
            // 说明下一个成分不是变量定义

            break;
        }else{
            varDefine();
            if(result!=SEMISY){
                error();
                return -1;
            }
            getsym();

        }

    }

    printf("This is a variable statement.\n");
    return 0;
}

int paraValueList(){
    int paraSeqCnt = 0;

    while(true){
        //
        expr();
        // exprType
        if(exprType != res.at(paraSeqCnt)){
//            printf("para type not fit in:%s at %d\n", callFuncID.c_str(), paraSeqCnt);
            SyntaxAnalysisError(errUnfitParaType, lc);

            return -1;
        }


        // 最后有一个操作数应该取出来
        stackCalc.pop_back();

        pushMidCodePara(tCount-1);
        tCount--;

        if(result==COMMASY){
            getsym();
            paraSeqCnt++;
            continue;
        }else{
            break;
        }
    }
    //tCount++;

    printf("This is a parameter value list.\n");
    return 0;
}

int paraList(){
//    int paralistItemCount = 0;
//    paralistItem tmpParalistItem;

    while(true){
        int paraType = 0;   // 0:default | 1:int | 2;char
        if(result!=INTSY&&result!=CHARSY){
            error();
            return -1;
        }

        if(result==INTSY)   paraType = 1;
        else                paraType = 2;

        //
        funcParaType.push_back(paraType);


        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }

        string thisParaName = IDname;
        getsym();

        //
        pushVarTable(IDname, paraType, globalOffset, 1);
        pushMidCodePara(paraType, IDname);

//        tmpParalistItem.paraTypes[paralistItemCount] = paraType;
//        tmpParalistItem.paraNames[paralistItemCount] = thisParaName;
//        paralistItemCount++;
//        tmpParalistItem.funcName = currentFuncID;

        if(result==COMMASY){
            getsym();
            continue;
        }else{
            break;
        }
    }


    printf("This is a parameter list.\n");
    return 0;
}

int retValueFuncDefine(){
    declareHead();

    // 处理参数表部分（如果有）
//    printf("pass declare head\n");
    if(result==LPARSY){
        getsym();
        paraList();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();
    }
//    printf("pass tag 1\n");
    if(result!=LBRACESY){   // "{"
        error();
        return -1;
    }

    // 记录函数信息
    funcInfoItem tmpFuncInfo = {currentFuncID, funcParaType};
    AllFuncInfo.push_back(tmpFuncInfo);
    // 清空funcParaType
    funcParaType.clear();

    globalMidCodeInFunc = 1;
    // 符号栈的操作 stack brace
    stackBrace.push_back('{');

    getsym();
    retExist = 0;
    complexSentence();
//    printf(">>>>>>>>>>>result = %d\n", result);
    if(result!=RBRACESY){   // "}"
        error();
        return -1;
    }
    getsym();

    //
    globalFuncField = "Global";
    globalFuncLevel = 0;
//    globalOffset = 0;

    // 语义分析，一定要有返回语句
    if(retExist==0){
        error();
        printf("No return sentence in retFuncDefinition.\n");
        return -1;
    }

    pushMidCodeFuncTail(currentFuncID);
    printf("This is function declaration with returned value.\n");
    return 0;
}

int unretValueFuncDefine(){
//    printf("in unretValueFuncDefine.\n");
    if(result!=VOIDSY){
        error();
        return -1;
    }
    getsym();
    if(result!=IDSY){
        error();
        return -1;
    }

    // 设置全局变量
    globalOffset = 0;
    globalFuncField = IDname;
    globalFuncLevel = 1;

    // 填入符号表
    pushFuncTable(IDname, 3);
    pushMidCodeFuncHead(3, IDname);
    currentFuncID = IDname;

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


    // 记录函数信息
    funcInfoItem tmpFuncInfo = {currentFuncID, funcParaType};
    AllFuncInfo.push_back(tmpFuncInfo);
    // 清空funcParaType
    funcParaType.clear();



    globalMidCodeInFunc = 1;
    //
    stackBrace.push_back('{');

    getsym();
 //   printf("check void complex 1.\n");
    retExist = 0;
    complexSentence();
//    printf("check void complex 2.\n");
//    printf("________________result = %d\n", result);
    if(result!=RBRACESY){   // "}"
        error();
        return -1;
    }

    if(stackBrace.back()=='{'){
        stackBrace.pop_back();
    }else{
        printf("STACK BRACE ERROR: not match.\n");
    }

    if(stackBrace.back()=='{'){
        stackBrace.pop_back();
    }else{
        printf("STACK BRACE ERROR: not match.\n");
    }

    getsym();

    // 恢复全局变量
    globalFuncField = "Global";
    globalFuncLevel = 0;

    if(retExist!=0){        // 这里可能有问题，重新查返回值限制条件
        error();
        printf("Return sentence in unretFunc Definition.\n");
    //    return -1;
    }

    pushMidCodeFuncTail(currentFuncID);
    printf("This is a function declaration without return value.\n");
    return 0;
}

int sentence(){

//    printf(">>>> in sentence\n");
//    printf("result=%d\n", result);
    switch(result){
        case(IFSY):             // 条件语句
            if(condSentence()==-1){
                SyntaxAnalysisError(errCondSentence, lc);
            }
            break;
        case(WHILESY):          // 循环语句
            loopSentence();
            break;
        case(FORSY):            // 循环语句
            loopSentence();
            break;
        case(LBRACESY):         // 语句列
            // 对符号栈操作
            stackBrace.push_back('{');

//            printf("this is where the bug might be for a while.\n");
            getsym();
            sentenceSequence();
            if(result!=RBRACESY){
                error();
                return -1;
            }else{
                if(stackBrace.back()=='{'){
                    stackBrace.pop_back();
                }else{
                    printf("STACK BRACE ERROR: not match.\n");
                }

                getsym();
            }
            break;
        case(SEMISY):           // 空语句，直接分号
            nullSentence();
            break;
        case(PRINTSY):          // 写语句
            printSentence();
            if(result!=SEMISY){
                error();
                return -1;
            }else{
                getsym();
            }
            break;
        case(SCANFSY):          // 读语句
            scanSentence();
            if(result!=SEMISY){
                error();
                return -1;
            }else{
                getsym();
            }
            break;
        case(RETSY):            // 返回语句
            retSentence();

            if(result!=SEMISY){
                error();
                return -1;
            }else{
                getsym();
            }
            break;
        case(IDSY):             // 两种情况，函数调用或赋值语句
            {
                string tmpFuncName = IDname; // 可能不是函数ID
            //    printf(">>>>>tmpFuncName:%s\n", tmpFuncName.c_str());

                int senTag = 0;

                recordRead();
                getsym(1);
                if(result==LPARSY || result==SEMISY)  senTag = 1;
                else if(result==EQUSY)    senTag = 2;
                else if(result==LBRACSY)    senTag = 3;
                resetRead();

                if(senTag==1){              // 函数调用
                    callFuncID = tmpFuncName;
                    if(retValueFuncCall()==-1){
                        SyntaxAnalysisError(errFuncCallNotComplete, lc);
                        jump2SEMISY2();
                        getsym();
                    }
                    if(result!=SEMISY){
                        error();
                        return -1;
                    }else{
                        getsym();
                    }
                    break;
                }else if(senTag==2){        // 赋值语句
                    if(assignSentence()==-1){
                        SyntaxAnalysisError(errAssignNotComplete, lc);
                    }
                    if(result!=SEMISY){
                        SyntaxAnalysisError(errLackSemiSymbol, lc);
                        return -1;
                    }else{
                        getsym();
                    }
                    break;
                }else if(senTag==3){        // 数组元素赋值语句
                    assignSentence();
                    if(result!=SEMISY){
                        error();
                        return -1;
                    }else{
                        getsym();
                    }
                    break;

                }else{
                    error();
                    return -1;
                }
            }
            break;

        default:    return -1;
    }

    printf("This is a sentence.\n");
    return 0;
}

int sentenceSequence(){
//    printf(">>>> in sequence sentence\n");
    while(true){
        if(result==IFSY||result==WHILESY||result==FORSY||result==LBRACESY||result==SEMISY||
           result==PRINTSY||result==SCANFSY||result==RETSY||result==IDSY){
        //    printf("send to sentence.\n");
            sentence();
        }else{
            break;
        }

    }

    printf("This is a sequence of sentences.\n");
    return 0;
}

int complexSentence(){
    // to be modified 需要预读处理
//    printf("complex sentence 1\n");
    if(result==CONSTSY){
        constState();
    }
//    printf("complex sentence 2\n");

//    int complexTag = 0;
//    recordRead();
//    if(result!=INTSY&&result!=CHARSY){
//        complexTag = 1;
//    }
//    getsym();
//    if(result!=IDSY){
//        complexTag = 1;
//    }
//    getsym();
//    if(result==LBRACSY){
//        complexTag = 1;
//    }
//
//    resetRead();
//
//    if(complexTag==1){
//        varState();
//    }
    // 变量声明部分
    int complexTag = 0;
    recordRead();
    if(result!=INTSY&&result!=CHARSY){
        complexTag = 1;
    }
    getsym(1);
    if(result!=IDSY){
        complexTag = 1;
    }
    getsym(1);
    if(result==LPARSY){     // 这里做过一次修改，如果产生连带问题可以回来看看
        complexTag = 1;
    }

    resetRead();
//    printf("complex-tag = %d\n", complexTag);
    if(complexTag==0){
        varState();
    }
//    printf("complex sentence 3\n");
    // 修改后的两个函数都可以直接调用，如果不符合不会破坏指针的位置
    // 语句列部分
    sentenceSequence();

    printf("This is a complex sentences.\n");
    return 0;
}

int condSentence(){

    labelCount++;

    // 入栈
    stackIfElse.push_back(labelCount);

    if(result!=IFSY){
        error();
        return -1;
    }
//    printf("in cond 1\n");
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
//    printf("in cond 2\n");
    getsym();
    if(condition()==-1){
        SyntaxAnalysisError(errCondNotComplete, lc);
        return -1;
    }
//    printf("in cond 3\n");
//    printf("result = %d\n", result);
    if(result!=RPARSY){
        error();
        return -1;
    }
    // 开始生成条件语句的四元式
    //pushMidCodeBZ(labelCount, 1);
    pushMidCodeBZ(stackIfElse.back(), 1);

    getsym();
//    printf("in cond 4\n");
    sentence();
    // 四元式部分
    //pushMidCodeGOTO(labelCount, 2);
    pushMidCodeGOTO(stackIfElse.back(), 2);
    //pushMidCodeLabel(labelCount, 1);
    pushMidCodeLabel(stackIfElse.back(), 1);


    // 可选项 else分支
    if(result==ELSESY){
//        printf("in cond 5\n");
        getsym();
        sentence();
    }
    //pushMidCodeLabel(labelCount, 2);
    pushMidCodeLabel(stackIfElse.back(), 2);
    stackIfElse.pop_back();

    //labelCount++;

    printf("This is a conditional sentence.\n");
    return 0;
}

int condition(){
    if(expr()==-1){
        SyntaxAnalysisError(errExprNotComplete, lc);
        //printf("Line:%d\tERROR: This is an ILLEGAL assignment sentence.\n", lc);

        // getsym, 将下一个符号SEMISY读入
        getsym();

        return -1;
    }

    // 最后有一个操作数应该取出来
    stackCalc.pop_back();


    int expr1Count = tCount;

    if(result==LESSSY||result==LOESY||result==MORESY||
       result==MOESY||result==LOMSY||result==AEQUSY){
        int op = result;
        getsym();
        expr();

        // 最后有一个操作数应该取出来
        stackCalc.pop_back();

        // 生成四元式
        pushMidCodeCondition(expr1Count, op, tCount);
    }else{
        // 不应该报错，存在条件是一个表达式的情况
        pushMidCodeCondition(tCount);
    }

    printf("This is a condition.\n");
    return 0;
}

int stepLength(){
    unsignedInt();

    printf("This is a step length.\n");
    return unsignValue;
}

int loopSentence(){
    if(result==WHILESY){
        labelCount++;

        stackIfElse.push_back(labelCount);

        //pushMidCodeLabel(labelCount, 1);
        pushMidCodeLabel(stackIfElse.back(), 1);

        getsym();
        if(result!=LPARSY){
            error();
            return -1;
        }
        getsym();
        condition();

        pushMidCodeBZ(stackIfElse.back(), 2);

        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();

        sentence();
        pushMidCodeGOTO(stackIfElse.back(), 1);
        pushMidCodeLabel(stackIfElse.back(), 2);

        stackIfElse.pop_back();

        //labelCount++;

    }else if(result==FORSY){
        // ENDRIGHTGERE!!!!
        labelCount++;
        stackIfElse.push_back(labelCount);

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
        string saveID = IDname;

        getsym();
        if(result!=EQUSY){
            error();
            return -1;
        }
        getsym();
        expr();

        // 最后有一个操作数应该取出来
        stackCalc.pop_back();

        // 把表达式的值取出来
        pushMidCodeAssign(saveID, 0, 0, tCount);
        pushMidCodeGOTO(stackIfElse.back(), 1);
        pushMidCodeLabel(stackIfElse.back(), 2);


        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();
        condition();

        pushMidCodeBZ(stackIfElse.back(), 3);
        pushMidCodeLabel(stackIfElse.back(), 1);

        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }

        string tmpRecordName = IDname;

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
        int op = 1;
        if(result!=PLUSSY&&result!=MINUSSY){
            error();
            return -1;
        }
        if(result==MINUSSY) op = 2;

        getsym();
        int length = stepLength();



        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();


        sentence();     // content

        // 生成四元式部分
        // saveID
        //pushMIdCodeCalStep(tCount, saveID, op, length);
        //tCount++;
        pushMidCodeGetValue(tCount, tmpRecordName);
        tCount++;

        pushMidCodeGetValue(tCount, length);
        tCount++;

        pushMidCodeCalc(tCount, tCount-2, op, tCount-1);
        tCount++;

        pushMidCodeAssign(tmpRecordName, 0, 0, tCount);

        pushMidCodeGOTO(stackIfElse.back(), 2);
        pushMidCodeLabel(stackIfElse.back(), 3);

        stackIfElse.pop_back();
        //labelCount++;

    }else{
        error();
        return -1;
    }

    printf("This is a loop sentence.\n");
    return 0;
}

int retValueFuncCall(){
    //printf("RETVFC: result=%d\n", result);
    if(result!=IDSY){
        error();
        return -1;
    }
//    string FuncID = IDname;

    getsym();

    res = searchFuncPara(callFuncID);
    int paraNum = res.size();

    if(paraNum==0){
        // 应该是没有参数的
        if(result==LPARSY){
            error();
            return -1;
        }
        // 否则一切正常
    }else{
        if(result!=LPARSY){
            error();
            return -1;
        }
        //
        getsym();

        if(paraValueList()==-1){
            SyntaxAnalysisError(errParaValueList, lc);
            return -1;
        }

        if(result!=RPARSY){
            error();
            return -1;
        }else{
            getsym();
        }
    }


    pushMidCodeFuncCall(callFuncID);
    printf("This is a function call with returned value.\n");
    return 0;
}

int unretValueFuncCall(){
    if(result!=IDSY){
        error();
        return -1;
    }
//    string FuncID = IDname;

    getsym();
    if(result==LPARSY){
        getsym();
        paraValueList();
        if(result!=RPARSY){
            error();
            return -1;
        }else{
            getsym();
        }
    }

    pushMidCodeFuncCall(callFuncID);
    printf("This is a function call without returned value.\n");
    return 0;
}

int assignSentence(){
//    printf("assign tag 1\n");
//    printf("result = %d\n", result);
    if(result!=IDSY){
        error();
        return -1;
    }
    string assignID = IDname;
    // 记录变量类型
    int recordFisrtType = searchName2Type(assignID, 0);

    getsym();
    int isArray = 0;
    int recTCount = 0;
//    printf("assign tag 2\n");
    if(result==LBRACSY){
        isArray = 1;
        getsym();
        expr();
        recTCount = tCount;

        // 最后有一个操作数应该取出来
        stackCalc.pop_back();


        // 检查数组越界
        printf(">>> check number1:%d\n", factorType);
        if(termCountFactor==1 && exprCountTerm==1 && factorType==1){
//            printf(">>> check number1:%d\n", checkArrayValue);
            int tableLength = getArrayLength(assignID);
//            printf(">>> check number2:%d\n", tableLength);
            if(checkArrayValue<0 || checkArrayValue>=tableLength){
                symbolTableError(errArrayOutOfRange);
                return -1;
            }
        }




        if(result!=RBRACSY){
            error();retExist += 1;
            return -1;
        }else   getsym();
    }
//    printf("assign tag 3\n");
    if(result!=EQUSY){
        error();
        return -1;
    }
    getsym();
//    printf("assign tag 4\n");

    nestedExpr = 0;
    if(expr()==-1){
        SyntaxAnalysisError(errExprNotComplete, lc);
        // printf("Line:%d\tERROR: This is an ILLEGAL assignment sentence.\n", lc);

        // getsym, 将下一个符号SEMISY读入
        getsym();

        return -1;
    }

    // 最后有一个操作数应该取出来
    stackCalc.pop_back();

    // 判断两边赋值的类型是否合法
    // illegal: int = char; char = int; char = (char); ......
    // legal:   int = (char);
    // 只判断是否违法就行
    int recordSecondType;
    printf(">>> termCountFactor:%d exprCountTerm:%d\n", termCountFactor, exprCountTerm);
    if(termCountFactor==1 && exprCountTerm==1){
        // 赋值语句右边是一个元素
        if(nestedExpr==1){
            // 发生嵌套也是int
            recordSecondType = 1;
        }else{
            recordSecondType = factorType2;
        }
    }else{
        // 多个元素一定是int
        recordSecondType = 1;
    }

    // 检查是否违法

    if(recordFisrtType!=recordSecondType){
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> test: %d=%d\n", recordFisrtType, recordSecondType);
        //printf(">>> test: %d=%d\n", recordFisrtType, recordSecondType);
        SyntaxAnalysisError(errAssignDifferType, lc);
        return -1;
    }


    nestedExpr = 0;
//    printf("assign tag 5\n");
    // 生成四元式
    pushMidCodeAssign(assignID, isArray, recTCount, tCount);

    printf("This is an assignment sentence.\n");
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
    //

    int type = searchName2Type(IDname, 0);
    pushMidCodeScan(type, IDname);

    getsym();
    while(true){
        if(result!=COMMASY) break;
        else{
            getsym();
            if(result!=IDSY){
                error();
                return -1;
            }else{
                int type = searchName2Type(IDname, 0);
                pushMidCodeScan(type, IDname);
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

    printf("This is a scan sentence.\n");
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
        pushMidCodePrint(1, globalString, tCount);
        globalString = "";
        getsym();
        if(result==COMMASY){
            getsym();
            expr();

            // 最后有一个操作数应该取出来
            stackCalc.pop_back();

            if(result!=RPARSY){
                error();
                return -1;
            }
            getsym();

            // 判断是否是单个标识符
            if(termCountFactor==1 && exprCountTerm==1 && factorType==3){
                // 是单个标识符
                pushMidCodePrint(IDname, tCount);
                printf("This is an ID print sentence.\n");
                return 0;

            }else{

                // 四元式操作
                pushMidCodePrint(2, "", tCount);
                printf("This is a print sentence.\n");
                return 0;
            }
        }else if(result==RPARSY){
            // end of print
            getsym();
            printf("This is a print sentence.\n");
            return 0;
        }else{
            error();
            return -1;
        }
    }else{
        expr();

        // 最后有一个操作数应该取出来
        stackCalc.pop_back();

        int getType = exprType;
        printf(">>> check get out expr type:%d\n", getType);

        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();

        if(termCountFactor==1 && exprCountTerm==1 && factorType==3){
            // 是单个标识符
            pushMidCodePrint(IDname, tCount);
            printf("This is an ID print sentence.\n");
            return 0;

        }else if(getType==2){
            //     有返回值函数调用，应为单个标识符的情况已经在上一个分支解决了，
            // 这里只可能是返回值类型为char的函数调用
            pushMidCodePrintFuncCall(tCount);
            printf("This is a func print sentence.\n");
            return 0;

        }else{
            pushMidCodePrint(2, "", tCount);
            printf("This is a print sentence.\n");
            return 0;
        }
    }

    //如果以上分支都不满足进行到了这里，返回未知错误
    return -2;
}

int nullSentence(){
    if(result==SEMISY){
        getsym();

        printf("This is an empty sentence.\n");
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

        // 最后有一个操作数应该取出来
        stackCalc.pop_back();

        if(result!=RPARSY){
            error();
            return -1;
        }
        pushMidCodeRet(tCount);

    }else{
        pushMidCodeRet();
    }
    getsym();

    retExist += 1;
    printf("This is a return sentence.\n");
    return 0;
}

int programAnalysis(){


    // 常量声明部分
    if(result==CONSTSY){
        constState();
    }

    // 变量声明部分
    int complexTag = 0;
    recordRead();
    if(result!=INTSY&&result!=CHARSY){
        complexTag = 1;
    }
    getsym(1);
    if(result!=IDSY){
        complexTag = 1;
    }
    getsym(1);
    if(result==LBRACESY || result==LPARSY){
        complexTag = 1;
    }

    resetRead();
//    printf("tag = %d\n", complexTag);
    if(complexTag==0){
        varState();
    }

    while(true){
        if(result==INTSY||result==CHARSY){
            retValueFuncDefine();
        }else if(result==VOIDSY){

            int mainTag = 0;
            recordRead();
            getsym();
            if(result==MAINSY){
                mainTag = 1;
            }
            resetRead();


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

    printf("This is a program analysis.\n");
    return 0;
}

int mainAnalysis(){
//    printf("in main anal\n");
    if(result!=VOIDSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
    if(result!=MAINSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    pushMidCodeMain();

    // 填入符号表
    globalFuncField = "Main";
    globalFuncLevel = 1;
    globalOffset = 0;

    pushFuncTable("Main", 4);   // 4: main

    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
    if(result!=RPARSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
    if(result!=LBRACESY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    stackBrace.push_back('{');

    getsym();
//    printf("check main complex 1.\n");
    complexSentence();
//    printf("in main anal\n");
//    printf("check main complex 2.\n");
    if(result!=RBRACESY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    if(stackBrace.back()=='{'){
        stackBrace.pop_back();
    }else{
        printf("STACK BRACE ERROR: not match.\n");
    }

    getsym();

    printf("This is a main function.\n");
    return 0;
}

int factor(){
    //printf("DEBUG: in factor, result=%d\n", result);

    if(result==LPARSY){             // result = "("
    //    printf("factor-debug branch-1\n");
        getsym();

        // 用一个标志记录这里发生了嵌套
        nestedExpr = 1;

        expr();                     // 表达式

        //语义分析
        //factorType = exprType;
        factorType = 1; // 参与运算了type就是1:int
        factorType2 = 1;

        if(result==RPARSY){         // result = ")"
            getsym();
        }else{
            error();
            return -1;
        }

        // 嵌套表达式，返回值也应该入计算栈
//        stackCalc.push_back(tCount);
//        printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
        // 修改过后，没有立即清空，这里不用


    }else if(result==IDSY){         // result = IDSY
        string recordFactorID = token;

//        pushMidCodeGetValue(tCount, IDname);
//        stackCalc.push_back(tCount);
//        tCount++;

        getsym();
        if(result==LBRACSY){        // result = "["
        //    printf("factor-debug branch-2\n");

            // 调用expr前先记录termCountFactor和exprCountTerm
            // 调用后恢复
            int tmpsave1 = termCountFactor;
            int tmpsave2 = exprCountTerm;

            getsym();
            expr();                 // 表达式

            stackCalc.pop_back();

            // 检查数组越界
            if(termCountFactor==1 && exprCountTerm==1 && factorType==1){
                //printf(">>> check number1:%d\n", checkArrayValue);
                printf(">>> check recorded ID:%s\n", recordFactorID.c_str());
                int tableLength = getArrayLength(recordFactorID);
                //printf(">>> check number2:%d\n", tableLength);
                if(checkArrayValue<0 || checkArrayValue>=tableLength){
                    symbolTableError(errArrayOutOfRange);
                    return -1;
                }
            }

            // 生成相应党的四元式
            // $ti = a[j]
            // $ti a [] j
            // named as "getArrayValue"
            pushMidCodeGetArrayValue(tCount, recordFactorID, tCount-1);
            stackCalc.push_back(tCount);
            printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
            tCount++;
            checkConflict();

            if(result==RBRACSY){
        //        printf("factor-debug branch-2-2\n");
                getsym();

                termCountFactor = tmpsave1;
                exprCountTerm = tmpsave2;


                factorType = searchName2Type(recordFactorID, 0);
                factorType2 = factorType;
                //factorType = 4;


            }else {
                error();
                return -1;
            }


        }else if(result==LPARSY){   // result = "(" 有返回值函数调用
        //    printf("factor-debug branch-3\n");

            // 有返回值函数调用
//            retValueFuncCall();

            int tmpsave1 = termCountFactor;
            int tmpsave2 = exprCountTerm;

            getsym();
            paraValueList();             // 值参数表

            termCountFactor = tmpsave1;
            exprCountTerm = tmpsave2;

            if(result==RPARSY){      // result = ")"
                getsym();

                factorType = searchName2Type(recordFactorID, 1);
                factorType2 = factorType;
                //factorType = 4;
            }else {
                error();
                return -1;
            }

            //
            pushMidCodeFuncCall(recordFactorID);

            // 将返回值压入运算栈
            ///////////////////////////////////////////////////////////////////////////////////////
            // 可能这里需要大改
            ///////////////////////////////////////////////////////////////////////////////////////
            pushMidCodeRET(tCount);
            stackCalc.push_back(tCount);
            printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
            tCount++;
            checkConflict();

            printf("This is a function call with returned value.\n");

        }else{
            // 需要查表判断是标识符还是无参数的函数调用
            int thisTag = 0;

            if(searchIsFunc(recordFactorID)==-1){
                // 普通的标识符，是个变量
                pushMidCodeGetValue(tCount, IDname);
                stackCalc.push_back(tCount);
                printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
                tCount++;
                checkConflict();

                //factorType = searchName2Type(recordFactorID, 0);
                factorType = 3;
                factorType2 = searchName2Type(recordFactorID, 0);

            }else{
                // 是函数调用
                printf("This is a function call without parameter.\n");

                pushMidCodeFuncCall(recordFactorID);

                pushMidCodeRET(tCount);
                stackCalc.push_back(tCount);
                printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
                tCount++;
                checkConflict();
                // ...
                //factorType = 0;
                factorType = searchName2Type(recordFactorID, 1);
                factorType2 = factorType;
            }

        }
    }else if(result==PLUSSY || result==MINUSSY){
        printf(">>>>>> in this branch: factor: MINUSSY/PLUSSY\n");
        // 判断一下符号
        int resultTag = 1;
        if(result==MINUSSY) resultTag = -1;

        getsym();
        // 这里才是普通的“整数”分支
        if(result!=USINTSY){    // 有符号整数
            error();
            return -1;
        }else{


            checkArrayValue = IntValue;
            checkArrayValue *= resultTag;

            pushMidCodeFactorValue(tCount, 1, checkArrayValue);
            stackCalc.push_back(tCount);
            printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
            tCount++;
            checkConflict();

            getsym();

            factorType = 1;
            factorType2 = 1;
        }
    }else if(result==USINTSY){
        if(getsym()==-1){
            return -1;
        }
        checkArrayValue = IntValue;

        pushMidCodeFactorValue(tCount, 1, checkArrayValue);
        stackCalc.push_back(tCount);
        printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
        tCount++;
        checkConflict();

        factorType = 1;
        factorType2 = 1;
    }else if(result==ACHARSY){
        pushMidCodeFactorValue(tCount, 2, globalChar);
        stackCalc.push_back(tCount);
        printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());
        tCount++;
        checkConflict();

        getsym();               // 字符 ACHARSY
        factorType = 2;
        factorType2 = 2;
    }else{
        SyntaxAnalysisError(errSomethingElse, lc);
        return -1;
    }

    // termCountFactor只是用来判断数组越界的，所以expr嵌套定义没有关系
    termCountFactor += 1;
    printf("This is a factor.\n");
    return 0;
}

int term(){
    termCountFactor = 0;

    if(factor()==-1){
        SyntaxAnalysisError(errFactorNotComplete, lc);
        return -1;
    }
    termType = factorType2;

    factorFirstTCount = transTCount2Register();
    printf("\t\t====== CHECK FACTOR:%d\n", factorFirstTCount);

    while(true){
        if(result==STARSY||result==DIVISY){
            int opTag = 3;
            if(result==DIVISY)  opTag = 4;

            getsym();
            if(factor()==-1){
                SyntaxAnalysisError(errFactorNotComplete, lc);
                return -1;
            }
            // 此处可以进行值计算、栈操作、生成四元式

            printf(">>>>>>>>>>>>>>>>>>>>>>>> check the length of stack:%d\n", stackCalc.size());

            int n2 = stackCalc.back();
            stackCalc.pop_back();
            printf("<<<<<< pop out :%d\n", n2);
            int n1 = stackCalc.back();
            stackCalc.pop_back();
            printf("<<<<<< pop out :%d\n", n1);
            pushMidCodeCalc(tCount, n1, opTag, n2);
            stackCalc.push_back(tCount);
            printf("<<<<<< push in:%d\n", tCount);

            printf(">>>>>>>>>>>>>>>>>>>>>>>> check the length of stack:%d\n", stackCalc.size());

            tCount++;
            checkConflict();
            factorFirstTCount = transTCount2Register();
            printf("\t\t====== CHECK FACTOR:%d\n", factorFirstTCount);

            termType = 1;   // 只要参与运算了就自动转化为int型

        }else   break;

    }

    exprCountTerm += 1;
    factorFirstTCount = 0;
    printf("This is a term.\n");
    return 0;
}

int expr(){
    exprCountTerm = 0;

    // 在这里把tCount清零
    // tCount = 0;

//    checkArrayTag = 0;  // 用来检查数组越界，
                        // 只有表达式递归下去是一个整数才需要检查
    // 跳过加法运算符，如果有
    int opTag = 0;

    if(result==PLUSSY||result==MINUSSY){
        opTag = 1;
        if(result==MINUSSY)
            opTag = -1;
        getsym();
    }


    if(term()==-1){
        SyntaxAnalysisError(errTermNotComplete, lc);
        return -1;
    }

    exprFirstTCount = transTCount2Register();
    printf("\t\t====== CHECK EXPR:%d\n", exprFirstTCount);
    exprFirstTCountVector.push_back(exprFirstTCount);

    if(opTag==-1){
        pushMidCodeGetMinusExpr(tCount);

        // 将运算栈中那个数替换掉
        int n = stackCalc.back();
        stackCalc.pop_back();
        printf("<<<<<< pop out :%d\n", n);
        stackCalc.push_back(tCount);
        printf("<<<<<< push in:%d\tsize:%d\n", tCount, stackCalc.size());


        tCount++;
        checkConflict();
    }



    // 如果有正负号，则第一个项不能是字符型
//    if(termType==2 && opTag==1){
//        printf("TMP: +|- char not valid in expr.\n");
//        error();
//        return -1;
//    }

    exprType = termType;

    while(true){
//        printf("expr result = %d\n", result);
        if(result==PLUSSY||result==MINUSSY){
            int opFlag = 1;
            if(result==MINUSSY) opFlag = 2;

            getsym();
            if(term()==-1){
                SyntaxAnalysisError(errTermNotComplete, lc);
                return -1;
            }
            // 参与运算，则为int
            // 此处可以进行值计算、栈操作、生成四元式
//            printf(">>>> size:%d\n", stackCalc.size());

            printf(">>>>>>>>>>>>>>>>>>>>>>>> check the length of stack:%d\n", stackCalc.size());

            int n2 = stackCalc.back();
            stackCalc.pop_back();
            printf("<<<<<< pop out :%d\n", n2);
//            printf(">>>> size:%d\n", stackCalc.size());
            int n1 = stackCalc.back();
            stackCalc.pop_back();
            printf("<<<<<< pop out :%d\n", n1);
//            printf(">>>> size:%d\n", stackCalc.size());
            pushMidCodeCalc(tCount, n1, opFlag, n2);
            stackCalc.push_back(tCount);
            printf("<<<<<< push in:%d\n", tCount);

            printf(">>>>>>>>>>>>>>>>>>>>>>>> check the length of stack:%d\n", stackCalc.size());

//            printf(">>>> size:%d\n", stackCalc.size());
            tCount++;
            checkConflict();
            exprFirstTCount = transTCount2Register();
            printf("\t\t====== CHECK EXPR:%d\n", exprFirstTCount);
            exprFirstTCountVector.pop_back();
            exprFirstTCountVector.push_back(exprFirstTCount);

            exprType = 1;
        }else   break;
    }

//    // 最后有一个操作数应该取出来
//    stackCalc.pop_back();

    // 检测之前的MINUSSY
//    if(opTag==-1){
//        pushMidCodeGetMinusExpr(tCount);
//        tCount++;
//    }


    exprFirstTCountVector.pop_back();
    if(exprFirstTCountVector.size()==0){
        exprFirstTCount = 0;
    }else{
        exprFirstTCount = exprFirstTCountVector.back();
    }


    printf("This is an expression.\n");
//    printf("tCount:%d\n", tCount);
    return 0;
}

int transTCount2Register(){
    int dul = tCount;
    if(dul>9){
        if(dul%9==0) dul = 9;
        else    dul %= 9;
    }
    return dul;
}

int transTCount2Register(int offsetFp){
    int dul = tCount + offsetFp;
    if(dul>9){
        if(dul%9==0) dul = 9;
        else    dul %= 9;
    }
    return dul;
}

void checkConflict(){
    // return:  1:conflict
    //          0:safe

    // 放置在每一次tCount++后
    // 这里的 tCount 是即将被使用的tCount
    int i, tmpReg, tmpExpr;
    printf("\t\t====== IN CHECK CONFLICT ====== factor: %d expr:%d\n", factorFirstTCount, exprFirstTCount);
    if(factorFirstTCount!=0||exprFirstTCount!=0){
        while(true){
            tmpReg = transTCount2Register(1);

            printf("\t\t======target:%d factor:%d ", tmpReg, factorFirstTCount);
            for(i=0;i<exprFirstTCountVector.size();i++)
                printf("%d ", exprFirstTCountVector.at(i));
            printf("\n");


            if(factorFirstTCount==tmpReg){
                // 发生了寄存器冲突
                printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
                tCount++;
            }else{
                int conflictTag = 0;
                for(i=0;i<exprFirstTCountVector.size();i++){
                    tmpExpr = exprFirstTCountVector.at(i);
                    if(tmpExpr==tmpReg){
                        // 发生了寄存器冲突
                        conflictTag = 1;
                        printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
                        tCount++;
                        // 不能break
                    }
                }
                if(conflictTag==0)
                    break;
            }
        }

//        tmpReg = transTCount2Register();
//        if(factorFirstTCount==tmpReg){
//            // 发生了寄存器冲突
//            printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
//            tCount++;
//        }else{
//            for(i=0;i<exprFirstTCountVector.size();i++){
//                tmpExpr = exprFirstTCountVector.at(i);
//                if(tmpExpr==tmpReg){
//                    // 发生了寄存器冲突
//                    printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
//                    tCount++;
//                    // 不能break
//                }
//            }
//        }
    }

//    // 防止二次冲突，再检测一边
//    if(factorFirstTCount!=0||exprFirstTCount!=0){
//        tmpReg = transTCount2Register();
//        if(factorFirstTCount==tmpReg){
//            // 发生了寄存器冲突
//            printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
//            tCount++;
//        }else{
//            for(i=0;i<exprFirstTCountVector.size();i++){
//                tmpExpr = exprFirstTCountVector.at(i);
//                if(tmpExpr==tmpReg){
//                    // 发生了寄存器冲突
//                    printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
//                    tCount++;
//                    // 不能break
//                }
//            }
//        }
//    }
}

void checkConflict(int offset){
    // return:  1:conflict
    //          0:safe

    // 放置在每一次tCount++后
    // 这里的 tCount 是即将被使用的tCount
    int i, tmpReg, tmpExpr;
    printf("\t\t====== IN CHECK CONFLICT ====== factor: %d expr:%d\n", factorFirstTCount, exprFirstTCount);
    if(factorFirstTCount!=0||exprFirstTCount!=0){
        while(true){
            tmpReg = transTCount2Register(offset);

            printf("\t\t======target:%d factor:%d ", tmpReg, factorFirstTCount);
            for(i=0;i<exprFirstTCountVector.size();i++)
                printf("%d ", exprFirstTCountVector.at(i));
            printf("\n");


            if(factorFirstTCount==tmpReg){
                // 发生了寄存器冲突
                printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
                tCount++;
            }else{
                int conflictTag = 0;
                for(i=0;i<exprFirstTCountVector.size();i++){
                    tmpExpr = exprFirstTCountVector.at(i);
                    if(tmpExpr==tmpReg){
                        // 发生了寄存器冲突
                        conflictTag = 1;
                        printf("\t\t====== CONFLICT: $t%d, factor:%d, expr:%d\n", tmpReg, factorFirstTCount, exprFirstTCount);
                        tCount++;
                        // 不能break
                    }
                }
                if(conflictTag==0)
                    break;
            }
        }

    }

}


vector<int> searchFuncPara(string funcName){
    int i, length = AllFuncInfo.size();
    for(i=0;i<length;i++){
        funcInfoItem funcInfoItem2 = AllFuncInfo.at(i);
        if(funcInfoItem2.funcName == funcName){
            return funcInfoItem2.funcParaType;
        }
    }

    if(i==length){
        printf("in function 'searchFuncPara' no such name:%s\n", funcName.c_str());
    }

    vector<int> nonsence;

    return nonsence;
}

// 跳读到下一个分号
void jump2SEMISY2(){
    int jumpCount = 0;
    int a = fgetc(fp);
    while(a!=';'){
        printf("%d\n", a);
        a = fgetc(fp);
        jumpCount++;

        if(a=='\n')
            lc++;
    }

    // 将分号退回
    ungetc(a,fp);

    printf(">>> Jump pass %d lexical characters.\n", jumpCount);

}























#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <iomanip>

#include "error.h"
#include "getsym.h"
#include "symbolTable.h"
#include "syntaxAnalysis.h"
#include "error.h"
#include "midCode.h"

vector<midCodeItem> midCodeVec;

int globalMidCodeInFunc = 0;

void pushMidCodeConst(int type, string ID, int value){
    // const int a 10
    // const char c 'c'
    string tmpType;
    string tmpValue;
    if(type==1){
        tmpType="int";
    //    itoa(value, tmpValue);

        tmpValue = to_string(value);
    }else{
        tmpType="char";
    //    tmpValue[0] = value;
    //    tmpValue[1] = '\0';

    //    sprintf(tmpValue, "%c", value);
//        char ctmp = value;
//        string stmp(&ctmp, 1);
//        tmpValue = stmp;

        tmpValue = to_string(value);

    //    printf(">>>> %s\n", tmpValue.c_str());
    }

    midCodeItem tmp = {
        "const",
        tmpType,
        ID,
        tmpValue,
    };

    midCodeVec.push_back(tmp);
//    printf(">>> Push mid code: %s, %s, %s, %s\n", tmp.one.c_str(), tmp.two.c_str(), tmp.three.c_str(), tmp.four.c_str());
}

void pushMidCodeVar(int type, string ID){
    string tmpType;
    if(type==1){
        tmpType="int";
    }else{
        tmpType="char";
    }
    midCodeItem tmp = {
        "var",
        tmpType,
        ID,
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeArray(int type, string ID, int length){
    string tmpType;
    string tmpValue;
    if(type==1){
        tmpType="int";
    //    itoa(value, tmpValue);

        tmpValue = to_string(length);
    }else{
        tmpType="char";
//        char ctmp = value;
//        string stmp(&ctmp, 1);
//        tmpValue = stmp;
        tmpValue = to_string(length);
    }
    midCodeItem tmp = {
        "array",
        tmpType,
        ID,
        tmpValue,
    };

    midCodeVec.push_back(tmp);
}

void pushMidCodeFuncHead(int funcType, string ID){
    // 先生成一个label
//    int i, len = strlen(ID.c_str());
    string labelName = ID + "_1";
//    for(i=0;i<len;i++)
//        labelName[i] = ID[i];
//    labelName[len] = '_';
//    labelName[len+1] = '1';
//    labelName[len+2] = 0;
    midCodeItem tmp1 = {
        "label",
        labelName,
        "func",
        "",
    };
    midCodeVec.push_back(tmp1);

    // 再生成一个函数信息的midcode
    string tmpType;
    switch(funcType){
        case(1):
            tmpType = "int";break;
        case(2):
            tmpType = "char";break;
        case(3):
            tmpType = "void";break;
        default:
            break;
    }
    midCodeItem tmp = {
        "func",
        tmpType,
        ID,
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeFuncTail(string ID){
    string labelName = ID + "_2";
    midCodeItem tmp = {
        "label",
        labelName,
        "func",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodePara(int type, string ID){
    string tmpType;
    if(type==1){
        tmpType="int";
    }else{
        tmpType="char";
    }
    midCodeItem tmp = {
        "para",
        tmpType,
        ID,
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeGetValue(int tCount, string ID){
    string t;
    int newCount = tCount + 1;
    if(newCount>9){
        if(newCount%9==0) newCount = 9;
        else    newCount %= 9;
    }

    t = "$t" + to_string(newCount);
    midCodeItem tmp = {
        t,
        ID,
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeGetValue(int tCount, int value){
    int newCount = tCount + 1;
    if(newCount>9){
        if(newCount%9==0) newCount = 9;
        else    newCount %= 9;
    }

    string t;
    t = "$t" + to_string(newCount);
    midCodeItem tmp = {
        t,
        to_string(value),
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeGetValue(int tCount){
    // 用于有返回执函数调用
    // 从v0 读数据
    string t;
    int newCount = tCount + 1;
    if(newCount>9){
        if(newCount%9==0) newCount = 9;
        else    newCount %= 9;
    }

    t = "$t" + to_string(newCount);
    midCodeItem tmp = {
        t,
        "$v0",
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeGetArrayValue(int tCount, string arrayID, int tCount2){
    // get tCount(new) ready
    int newCount = tCount + 1;
    if(newCount>9){
        if(newCount%9==0) newCount = 9;
        else    newCount %= 9;
    }

    int newCount2 = tCount2 + 1;
    if(newCount2>9){
        if(newCount2%9==0) newCount2 = 9;
        else    newCount2 %= 9;
    }

    // gen and push mid code
    string t, t2;
    t = "$t" + to_string(newCount);
    t2 = "$t" + to_string(newCount2);

    // 源代码：     x = arrayID[y]
    //              y(expr) -> $t1
    // 生成形式:    $t2, arrayID, [], $t1
    midCodeItem tmp = {
        t,
        arrayID,
        "[]",
        t2,
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeCalc(int tCount, int n1, int op, int n2){
    int dul = tCount + 1;
    if(dul>9){
        if(dul%9==0) dul = 9;
        else    dul %= 9;
    }

    int d1 = n1 + 1, d2 = n2 + 1;
    if(d1>9){
        if(d1%9==0) d1 = 9;
        else    d1 %= 9;
    }
    if(d2>9){
        if(d2%9==0) d2 = 9;
        else    d2 %= 9;
    }


    // op: 0:default | 1:+ | 2:- | 3:* | 4:/
    string t1 = "$t" + to_string(dul);
    string t2 = "$t" + to_string(d1);
    string t3;
    string t4 = "$t" + to_string(d2);

    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }

    switch(op){
        case(1):
            t3 = "+";break;
        case(2):
            t3 = "-";break;
        case(3):
            t3 = "*";break;
        case(4):
            t3 = "/";break;
        default: break;
    }
    midCodeItem tmp = {
        t1,
        t2,
        t3,
        t4,
    };
    midCodeVec.push_back(tmp);

}

void pushMidCodePara(int tCount){
    int newCount = tCount + 1;
    if(newCount>9){
        if(newCount%9==0) newCount = 9;
        else    newCount %= 9;
    }

    string t1 = "$t" + to_string(newCount);
    midCodeItem tmp = {
        "push",
        t1,
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeFuncCall(string FuncID){

    midCodeItem tmp = {
        "call",
        FuncID,
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeRet(int tCount){
    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }

    string t1 = "$t" + to_string(tCount);
    midCodeItem tmp = {
        "ret",
        t1,
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeRet(){
    midCodeItem tmp = {
        "ret",
        "",
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeRET(int tCount){
    int newCount = tCount + 1;
    if(newCount>9){
        if(newCount%9==0) newCount = 9;
        else    newCount %= 9;
    }
    string t = "$t" + to_string(newCount);
    midCodeItem tmp = {
        t,
        "RET",
        "",
        "",
    };
    midCodeVec.push_back(tmp);


}

void pushMidCodeAssign(string ID, int isArray, int index, int tCount){
    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }

    if(index>9){
        if(index%9==0) index = 9;
        else    index %= 9;
    }

    midCodeItem tmp;
    if(isArray==1){
        // 数组赋值
        string t1 = "$t" + to_string(tCount);
        string t3 = "$t" + to_string(index);
        tmp = {
            ID,
            "[]",
            //to_string(index),
            t3,
            t1,
        };
    }else{
        string t1 = "$t" + to_string(tCount);
        tmp = {
            ID,
            t1,
            "",
            "",
        };
    }

    midCodeVec.push_back(tmp);
}

void pushMidCodeCondition(int tCount){
    // 条件是一个表达式的情况
    // 结果存在寄存器tCount里
    // 解决方案：将tCount的值存到s1中

    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }
    string t = "$t" + to_string(tCount);

    midCodeItem tmp = {
        "$s1",
        t,
        "",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeCondition(int expr1, int op, int expr2){
    if(expr1>9){
        if(expr1%9==0) expr1 = 9;
        else    expr1 %= 9;
    }
    if(expr2>9){
        if(expr2%9==0) expr2 = 9;
        else    expr2 %= 9;
    }

    string t1 = "$t" + to_string(expr1);//
    string t2 = "$t" + to_string(expr2);//
    string opt;

    switch(op){
        case(33):   opt="<";break;
        case(34):   opt="<=";break;
        case(35):   opt=">";break;
        case(36):   opt=">=";break;
        case(37):   opt="!=";break;
        case(38):   opt="==";break;
        default:    break;
    }

    midCodeItem tmp = {
        t1,
        opt,
        t2,
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeFactorValue(int tCount, int type, int value){
    int newCount = tCount + 1;
    if(newCount>9){
        if(newCount%9==0) newCount = 9;
        else    newCount %= 9;
    }

    midCodeItem tmp;
    if(type==1){
        string t1 = "$t" + to_string(newCount);
        tmp = {
            t1,
            to_string(value),
            "",
            "",
        };
    }else{
        string t1 = "$t" + to_string(newCount);
//        char ctmp = value;
//        string stmp(&ctmp, 1);
//        string tmpValue = stmp;
        tmp = {
            t1,
            //tmpValue,
            to_string(value),
            "",
            "",
        };
    }
    midCodeVec.push_back(tmp);
}

void pushMidCodeLabel(int labelCount, int index){
    string content = to_string(labelCount) + "_" + to_string(index);
    midCodeItem tmp = {
        "label",
        content,
        "norm",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeBZ(int labelCount, int index){
    string content = to_string(labelCount) + "_" + to_string(index);
    midCodeItem tmp = {
        "BZ",
        "label",
        content,
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeBNZ(int labelCount, int index){
    string content = to_string(labelCount) + "_" + to_string(index);
    midCodeItem tmp = {
        "BNZ",
        "label",
        content,
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeGOTO(int labelCount, int index){
    string content = to_string(labelCount) + "_" + to_string(index);
    midCodeItem tmp = {
        "GOTO",
        "label",
        content,
        "",
    };
    midCodeVec.push_back(tmp);
}
//
//void pushMIdCodeCalStep(int tCount, string var, int op, int length){
//    if(tCount>9){
//        if(tCount%9==0) tCount = 9;
//        else    tCount %= 9;
//    }
//
//    string t1 = "$t" + to_string(tCount+1);
//    string opt = "+";
//    if(op==2)   opt = "-";
//
//    midCodeItem tmp = {
//        t1,
//        var,
//        opt,
//        to_string(length),
//    };
//    midCodeVec.push_back(tmp);
//
//    midCodeItem tmp2 = {
//        var,
//        t1,
//        "",
//        "",
//    };
//    midCodeVec.push_back(tmp2);
//}

void pushMidCodeScan(int type, string ID){
    string tmpType;
    if(type==1){
        tmpType="int";
    }else if(type==2){
        tmpType="char";
    }else{
        tmpType="";
    }
    midCodeItem tmp = {
        "scan",
        tmpType,
        ID,
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodePrint(int mode, string content, int tCount){
    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }

    if(mode==1){
        // 打印字符串
        midCodeItem tmp = {
            "print",
            content,
            "",
            "",
        };
        midCodeVec.push_back(tmp);
    }else{
        // 打印表达式值
        string t1 = "$t" + to_string(tCount);
        midCodeItem tmp = {
            "print",
            t1,
            "",
            "",
        };
        midCodeVec.push_back(tmp);
    }
}

void pushMidCodePrint(string ID, int tCount){
    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }
    string st = "$t" + to_string(tCount);

    midCodeItem tmp = {
        "print",
        ID,
        "IDSY",
        st,
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodePrintFuncCall(int tCount){
    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }
    string st = "$t" + to_string(tCount);

    midCodeItem tmp = {
        "print",
        st,
        "func",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeMain(){
    midCodeItem tmp = {
        "main",
        "void",
        "main",
        "",
    };
    midCodeVec.push_back(tmp);
}

void pushMidCodeGetMinusExpr(int tCount){

    if(tCount>9){
        if(tCount%9==0) tCount = 9;
        else    tCount %= 9;
    }
    string st = "$t" + to_string(tCount);

    int dul = tCount + 1;
    if(dul>9){
        if(dul%9==0) dul = 9;
        else    dul %= 9;
    }
    string res = "$t" + to_string(dul);

    midCodeItem tmp = {
        res,
        "-",
        st,
        "",
    };
    midCodeVec.push_back(tmp);
}

void printMidCode(){
    printf("\nContents of mid codes\n1\t2\t3\t4\n");
    int i, cntMidCode = midCodeVec.size();
    for(i=0;i<cntMidCode;i++){
        midCodeItem tmp = midCodeVec.at(i);
        printf("%s\t", tmp.one.c_str());
        printf("%s\t", tmp.two.c_str());
        printf("%s\t", tmp.three.c_str());
        printf("%s\n", tmp.four.c_str());
    }
}

void MidCode2File(){
    int i, cntMidCode = midCodeVec.size();
    ofstream ofile;
    ofile.open("midCode.txt");
    // setw(6)<<i  1\t2\t3\t4\n
    ofile<<"Contents of mid codes"<<endl;
    ofile<<setw(12)<<"1"<<setw(12)<<"2"<<setw(12)<<"3"<<setw(12)<<"4"<<endl;
    for(i=0;i<cntMidCode;i++){
        midCodeItem tmp = midCodeVec.at(i);
        ofile<<setw(12)<<tmp.one.c_str()<<setw(12)<<tmp.two.c_str()<<setw(12)<<tmp.three.c_str()<<setw(12)<<tmp.four.c_str()<<endl;
    }
    ofile.close();
}

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <stdlib.h>

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
        char ctmp = value;
        string stmp(&ctmp, 1);
        tmpValue = stmp;
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

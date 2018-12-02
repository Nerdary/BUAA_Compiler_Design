#ifndef MIDCODE_H_INCLUDED
#define MIDCODE_H_INCLUDED

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;
//
struct midCodeItem{
    string one;     // op
    string two;     // n1
    string three;   // n2
    string four;    // out
};

extern vector<midCodeItem> midCodeVec;
extern int globalMidCodeInFunc;

void pushMidCodeConst(int type, string ID, int value);
void pushMidCodeVar(int type, string ID);
void pushMidCodeArray(int type, string ID, int length);
void pushMidCodeFuncHead(int funcType, string ID);
void pushMidCodeFuncTail(string ID);
void pushMidCodePara(int type, string ID);
void pushMidCodeGetValue(int tCount, string ID);
void pushMidCodeCalc(int tCount, int n1, int op, int n2);
void pushMidCodePara(int tCount);    // жиди
void pushMidCodeFuncCall(string FuncID);
void pushMidCodeRet(int tCount);
void pushMidCodeRet();
void pushMidCodeAssign(string ID, int isArray, int index, int tCount);
void pushMidCodeCondition(int expr1, int op, int expr2);
void pushMidCodeFactorValue(int tCount, int type, int value);


void printMidCode();

#endif // MIDCODE_H_INCLUDED

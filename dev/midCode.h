#ifndef MIDCODE_H_INCLUDED
#define MIDCODE_H_INCLUDED

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

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
void pushMidCodeGetValue(int tCount, int value);
void pushMidCodeGetValue(int tCount); //
void pushMidCodeGetArrayValue(int tCount, string arrayID, int tCount2);
void pushMidCodeCalc(int tCount, int n1, int op, int n2);
void pushMidCodePara(int tCount);    // жиди
void pushMidCodeFuncCall(string FuncID);
void pushMidCodeRet(int tCount);
void pushMidCodeRet();
void pushMidCodeRET(int tCount);
void pushMidCodeAssign(string ID, int isArray, int index, int tCount);
void pushMidCodeCondition(int expr1, int op, int expr2);
void pushMidCodeFactorValue(int tCount, int type, int value);
void pushMidCodeLabel(int labelCount, int index);
void pushMidCodeBZ(int labelCount, int index);
void pushMidCodeBNZ(int labelCount, int index);
void pushMidCodeGOTO(int labelCount, int index);
void pushMIdCodeCalStep(int tCount, string var, int op, int length);
void pushMidCodeScan(int type, string ID);
void pushMidCodePrint(int mode, string content, int tCount);
void pushMidCodePrint(string ID, int tCount);
void pushMidCodePrintFuncCall(int tCount);
void pushMidCodeMain();
void pushMidCodeGetMinusExpr(int tCount);

void MidCode2File();
void printMidCode();

#endif // MIDCODE_H_INCLUDED

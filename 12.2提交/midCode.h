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

void printMidCode();

#endif // MIDCODE_H_INCLUDED

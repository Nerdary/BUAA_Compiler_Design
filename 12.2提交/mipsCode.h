#ifndef MIPSCODE_H_INCLUDED
#define MIPSCODE_H_INCLUDED

#include "midCode.h"

#define $0 $zero
#define $1 $at
#define $2 $v0
#define $3 $v1
#define $4 $a0
#define $5 $a1
#define $6 $a2
#define $7 $a3
#define $8 $t0
#define $9 $t1
#define $10 $t2
#define $11 $t3
#define $12 $t4
#define $13 $t5
#define $14 $t6
#define $15 $t7
#define $16 $s0
#define $17 $s1
#define $18 $s2
#define $19 $s3
#define $20 $s4
#define $21 $s5
#define $22 $s6
#define $23 $s7
#define $24 $t8
#define $25 $t9
// 26
// 27
#define $28 $gp
#define $29 $sp
#define $30 $fp
#define $31 $ra

using namespace std;

struct globalRecordItem{
    string ID;
    int offset;
};

struct mipsItem{
    string one;
    string two;
    string three;
    string four;
};

extern vector<mipsItem> mipsCodeVector;

struct functionInfo{
    // 形参声明
    vector<midCodeItem> paraStatement;
    // 常量定义
    vector<midCodeItem> constDefinition;
    // 变量定义
    vector<midCodeItem> variableDefinition;

    //


};

void genMips();
void printMipsCode();

void addi(string res, string in, int value);
void add(string res, string n1, string n2);
void sub(string res, string n1, string n2);
void sw(string value, int offset, string base);
void li(string res, string value);
void mul(string res, string m1, string m2);
void printGlobalRecord();

#endif // MIPSCODE_H_INCLUDED

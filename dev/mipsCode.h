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

extern int funcLevel;

struct transResult{
    int success;
    int value;
};

struct globalRecordItem{
    string ID;
    int offset;
    string type;
};

struct funcRecordItem{
    string ID;
    int offset;
    int isPara;
    string value;
    string type;
};

struct mipsItem{
    string one;
    string two;
    string three;
    string four;
};

extern vector<mipsItem> mipsCodeVector;

struct functionInfo{
    string funcName;
    int level;
    //int fp;
    //int sp;
    //int length;
    int isMain;
    vector<funcRecordItem> funcSymbolTable;
//    vector<midCodeItem> otherMidCodeVec;
};

struct searchResult{
//    int targetFp;
    int index;
    int isMain;
};

void handleMain();
void handleMidCode();

void genMips();
void printMipsCode();

searchResult searchStackID(string targetFuncName, string ID);
int searchGlobalID(string ID);
int searchIDType(string func, string ID);

void nextLine();

void addi(string res, string in, int value);
void add(string res, string n1, string n2);
void sub(string res, string n1, string n2);
void sw(string value, int offset, string base);
void lw(string res, int offset, string base);
void li(string res, string value);
void mul(string res, string m1, string m2);
void div(string res, string d1, string d2);
void label(string label);
void j(string label);
void jr();
void jal(string label);
void blez(string tag, string label);
void bltz(string tag, string label);
void bne(string s1, string s2, string label);
void beq(string s1, string s2, string label);
void mipsLabel(string label);
void mipsEndLabel();
void syscall();


void printGlobalRecord();
void Mips2File();

void SplitString(const string& s, vector<string>& v, const string& c);

#endif // MIPSCODE_H_INCLUDED

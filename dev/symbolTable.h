#ifndef SYMBOLTABLE_H_INCLUDED
#define SYMBOLTABLE_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

//struct paralistItem{
//    string funcName;
//    int paraTypes[1000];
//    string paraNames[1000];
//};

extern int globalFuncLevel;
extern string globalFuncField;
//extern vector<paralistItem> paralistVector;



struct symbolTableItem{
    string ID;          // 标识符名称
    int value;          // 存储常量的值， 默认为0
    int IDobject;       // 标识符种类
                        // 0: default | 1: constant | 2: variable | 3: function | 4: array
    int IDType;         // 标识符类型
                        // 0: default | 1: int | 2: char
    string field;       // 变量、常量等等的作用域
    int level;          // 当前分程序层级，默认为1

//    int addr;           // 对于变量、形参，存运行栈中的地址
                        // 对于函数，填入入口地址, default 0
                        // 对于常量，存个0
                        // 对于数组，填首元素地址
    int offset;         // 对于形参，填相对函数域的偏移量
                        // 对于变量，填相对函数域的偏移量
                        // 对于常量，填相对函数域的偏移量
                        // 对于数组，填相对函数域的偏移量
    int length;         // 数组长度,非数组默认值为0
    int funcType;       // 函数类型
                        // 0: default | 1: int | 2: char | 3: void | 4: main
    int isPara;         // 如果是形参则为1，其余情况为0
};

/*
struct arrayTableItem{
    int type;           // 数组元素类型
    int length;         // 数组长度

};

struct funcTableItem{
    int funcType;       // 返回值类型
                        // 0: default | 1: int | 2: char | 3: void | 4: main
//    int paraList[][2];  // 参数列表
                        // paraList[i][0] = 0: default | 1: int | 2: char
                        // paraList[i][1] = value: int | ascii: char

};

*/


// 常量 int char
int pushConstantTable(string ID, int type, int value);
// 数组
int pushArrayTable(string ID, int type, int length, int offset);
// 函数声明
int pushFuncTable(string ID, int retType);
// 变量、形参
int pushVarTable(string ID, int type, int offset, int isPara);

int checkDuplicate(string ID);

void printSymbolTable();

int searchName2Obj(string Name);
int searchName2Type(string Name, int mode);

int searchIsFunc(string Name);

int getArrayLength(string ID);

int judgeConst(string Name);
int getConstValue(string Name);

#endif // SYMBOLTABLE_H_INCLUDED

#ifndef SYMBOLTABLE_H_INCLUDED
#define SYMBOLTABLE_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

extern int globalFuncLevel;
extern string globalFuncField;


struct symbolTableItem{
    string ID;          // 标识符名称
    int value;          // 存储常量的值， 默认为0
    int IDobject;       // 标识符种类
                        // 0: default | 1: constant | 2: variable | 3: function | 4: array
    int IDType;         // 标识符类型
                        // 0: default | 1: int | 2: char
//    int link;           // 同一分程序中上一个标识符的位置
                        // 过程名函数名的 link 值为 0
    string field;       // 变量、常量等等的作用域
//    int refer;          // 指向数组、函数在分表中的索引，默认为0
    int level;          // 当前分程序层级，默认为1

    int addr;           // 对于变量、形参，存运行栈中的地址
                        // 对于函数，填入入口地址, default 0
                        // 对于常量，存个0
                        // 对于数组，填首元素地址
    int length;         // 数组长度,非数组默认值为0
    int funcType;       // 函数类型
                        // 0: default | 1: int | 2: char | 3: void | 4: main

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
void pushConstantTable(string ID, int type, int value);
// 数组
void pushArrayTable(string ID, int type, int length, int addr);
// 函数声明
void pushFuncTable(string ID, int retType);
// 变量、形参
void pushVarTable(string ID, int type, int addr);

#endif // SYMBOLTABLE_H_INCLUDED

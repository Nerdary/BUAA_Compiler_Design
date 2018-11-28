#include <stdio.h>
#include <string.h>

#include "symbolTable.h"
#include "syntaxAnalysis.h"
#include "error.h"

int globalFuncLevel = 1;
string globalFuncField = "";


// 常量 int char
void pushConstantTable(string ID, int type, int value){

    symbolTableItem tmp = {
        ID,                 // ID name
        value,              // value
        1,                  // constant
        type,               // ID type
        globalFuncField,    //
        globalFuncLevel,    //
        0,                  // address
        0,                  // length of array
        0,                  // function type
    };

    symbolTable.push_back(tmp);
    printf("Push constant:%s value:%d in symbolTable.\n", ID.c_str(), value);
}
// 数组
void pushArrayTable(string ID, int type, int length, int addr){
/*
    arrayTableItem atmp = {
        type,
        length,
    };
    arrayTable.push_back(atmp);
    int asize = arrayTable.size();
*/
    symbolTableItem tmp = {
        ID,                 // ID name
        0,                  // value
        4,                  // array
        type,               // ID type
        globalFuncField,    //
        globalFuncLevel,    //
        addr,               // address
        length,             // length of array
        0,                  // function type
    };

    symbolTable.push_back(tmp);
    printf("Push array:%s length:%d in symbolTable.\n", ID.c_str(), length);
}
// 函数声明
void pushFuncTable(string ID, int retType){
    symbolTableItem tmp = {
        ID,                 // ID name
        0,                  // value
        3,                  // function
        0,                  // ID type
        globalFuncField,    //
        globalFuncLevel,    //
        0,                  // address
        0,                  // length of array
        retType,            // function type
    };

    symbolTable.push_back(tmp);
    printf("Push function:%s type:%d in symbolTable.\n", ID.c_str(), retType);
}
// 变量、形参
void pushVarTable(string ID, int type, int addr){
    symbolTableItem tmp = {
        ID,                 // ID name
        0,                  // value
        2,                  // variable
        type,               // ID type
        globalFuncField,    //
        globalFuncLevel,    //
        addr,               // address
        0,                  // length of array
        0,                  // function type
    };

    symbolTable.push_back(tmp);
    printf("Push variable:%s type:%d in symbolTable.\n", ID.c_str(), type);
}

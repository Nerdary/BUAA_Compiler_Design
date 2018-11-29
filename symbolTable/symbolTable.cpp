#include <stdio.h>
#include <string.h>

#include "symbolTable.h"
#include "syntaxAnalysis.h"
#include "error.h"

int globalFuncLevel = 0;            // 全局为0 main 为1
string globalFuncField = "Global";  // 初始设置为全局


// 常量 int char
void pushConstantTable(string ID, int type, int value){

    symbolTableItem tmp = {
        ID,                 // ID name
        value,              // value
        1,                  // constant
        type,               // ID type
        globalFuncField,    //
        globalFuncLevel,    //
        globalOffset,       // offset
        0,                  // length of array
        0,                  // function type
        0,                  // is parameter
    };

    globalOffset += 1;

    symbolTable.push_back(tmp);
    printf("Push constant:%s value:%d in symbolTable.\n", ID.c_str(), value);
}
// 数组
void pushArrayTable(string ID, int type, int length, int offset){
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
        offset,             // offset
        length,             // length of array
        0,                  // function type
        0,                  // is parameter
    };

    globalOffset += 1;

    symbolTable.push_back(tmp);
    printf("Push array:%s length:%d in symbolTable.\n", ID.c_str(), length);
}
// 函数声明
void pushFuncTable(string ID, int retType){
    // level & field 不在这里设置

    symbolTableItem tmp = {
        ID,                 // ID name
        0,                  // value
        3,                  // function
        0,                  // ID type
        globalFuncField,    //
        globalFuncLevel,    //
        0,                  // offset
        0,                  // length of array
        retType,            // function type
        0,                  // is parameter
    };

    globalOffset += 1;

    symbolTable.push_back(tmp);
    printf("Push function:%s type:%d in symbolTable.\n", ID.c_str(), retType);
}
// 变量、形参
void pushVarTable(string ID, int type, int offset, int isPara){
    symbolTableItem tmp = {
        ID,                 // ID name
        0,                  // value
        2,                  // variable
        type,               // ID type
        globalFuncField,    //
        globalFuncLevel,    //
        offset,             // offset
        0,                  // length of array
        0,                  // function type
        isPara,             // is parameter
    };

    globalOffset += 1;

    symbolTable.push_back(tmp);
    printf("Push variable:%s type:%d isPara:%d in symbolTable.\n", ID.c_str(), type, isPara);
}

void printSymbolTable(){
    int i;
    int cntTable = symbolTable.size();
    printf("num\tID\tvalue\tobj\ttype\tfield\tlevel\toffset\tlen\tfunT\tispara\n");
    for(i=0;i<cntTable;i++){
        printf("%d\t", i);
        printf("%s\t", symbolTable.at(i).ID.c_str());
        printf("%d\t", symbolTable.at(i).value);
        printf("%d\t", symbolTable.at(i).IDobject);
        printf("%d\t", symbolTable.at(i).IDType);
        printf("%s\t", symbolTable.at(i).field.c_str());
        printf("%d\t", symbolTable.at(i).level);
        printf("%d\t", symbolTable.at(i).offset);
        printf("%d\t", symbolTable.at(i).length);
        printf("%d\t", symbolTable.at(i).funcType);
        printf("%d\n", symbolTable.at(i).isPara);
    }
}

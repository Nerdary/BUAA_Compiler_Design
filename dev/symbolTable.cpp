#include <stdio.h>
#include <string>

#include "symbolTable.h"
#include "syntaxAnalysis.h"
#include "error.h"
#include "getsym.h"

int globalFuncLevel = 0;            // 全局为0 main 为1
string globalFuncField = "Global";  // 初始设置为全局

using namespace std;

// 常量 int char
int pushConstantTable(string ID, int type, int value){

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

    // check
    if(checkDuplicate(ID)!=0){
        symbolTableError(errDuplicate, lc);
        return -1;
    }

    symbolTable.push_back(tmp);
    return 0;
}

// 数组
int pushArrayTable(string ID, int type, int length, int offset){

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

    // check
    if(checkDuplicate(ID)!=0){
        symbolTableError(errDuplicate, lc);
        return -1;
    }

    symbolTable.push_back(tmp);
    return 0;
}
// 函数声明
int pushFuncTable(string ID, int retType){
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

    // check
    if(checkDuplicate(ID)!=0){
        symbolTableError(errDuplicate, lc);
        return -1;
    }

    symbolTable.push_back(tmp);
    return 0;
}
// 变量、形参
int pushVarTable(string ID, int type, int offset, int isPara){
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

    // check
    if(checkDuplicate(ID)!=0){
        symbolTableError(errDuplicate, lc);
        return -1;
    }
    symbolTable.push_back(tmp);

    return 0;
}

int checkDuplicate(string ID){
    int i, cntTable = symbolTable.size();
    for(i=0;i<cntTable;i++){
        if(symbolTable.at(i).ID==ID
            && symbolTable.at(i).field==globalFuncField){

            // 标识符相同，作用域相同，直接报错
            return -1;
        }
    }
    return 0;
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

int searchName2Obj(string Name){
    // 输入标识符ID(name)返回标识符类型
    // update: 修改后也可以查函数的返回值类型了 mode: 0:others | 1:functions
    // 0:default | 1:int | 2:char | 3:else/unknown
    int i, cntTable = symbolTable.size();

    for(i=cntTable-1;i>=0;i--){
        if(symbolTable.at(i).ID==Name){
            // 名字相同
            if(symbolTable.at(i).field==globalFuncField){
                // 作用域相同
                return symbolTable.at(i).IDobject;
            }else if(symbolTable.at(i).level<globalFuncLevel){
                // 作用域不同，但有全局定义
                return symbolTable.at(i).IDobject;
            }else   continue;
        }
    }
    printf("No such ID in symbol table, fail to trans name to type.\n");

    return 0;
}

int searchName2Type(string Name, int mode){
    // 输入标识符ID(name)返回标识符类型
    // update: 修改后也可以查函数的返回值类型了 mode: 0:others | 1:functions
    // 0:default | 1:int | 2:char | 3:else/unknown
    int i, cntTable = symbolTable.size();
    switch(mode){
    case(0):
        for(i=cntTable-1;i>=0;i--){
            if(symbolTable.at(i).ID==Name){
                // 名字相同
                if(symbolTable.at(i).field==globalFuncField){
                    // 作用域相同
                    return symbolTable.at(i).IDType;
                }else if(symbolTable.at(i).level<globalFuncLevel){
                    // 作用域不同，但有全局定义
                    return symbolTable.at(i).IDType;
                }else   continue;
            }
        }
        printf("No such var in symbol table, fail to trans name to type.\n");
        break;
    case(1):
        for(i=0;i<cntTable;i++){
            if(symbolTable.at(i).ID==Name
               && symbolTable.at(i).IDobject==3)
               return symbolTable.at(i).funcType;
        }
        printf("No such func in symbol table, fail to trans name to type.\n");
        break;
    default:    break;
    }

    return 0;
}

int searchIsFunc(string Name){
    int i, cntTable = symbolTable.size();
    for(i=0;i<cntTable;i++){
        if(symbolTable.at(i).ID==Name && symbolTable.at(i).IDobject==3)
               return symbolTable.at(i).funcType;
    }

    return -1;
}

int getArrayLength(string ID){
    int i, cntTable = symbolTable.size();
    for(i=0;i<cntTable;i++){
        if(symbolTable.at(i).ID==ID){
            if(symbolTable.at(i).field==globalFuncField
               || symbolTable.at(i).level < globalFuncLevel){

                // 输出检查
//                printf("///////////////////////\n");
//                printf("// ID:\t%s\n", ID.c_str());
//                printf("// field:\t%s\n",symbolTable.at(i).field.c_str());
//                printf("// obj:\t%d\n", symbolTable.at(i).IDobject);
//                printf("///////////////////////\n");


                if(symbolTable.at(i).IDobject!=4){
                    // 不是array
                    symbolTableError(errArrayLength, lc);
                    return -1;
                }
                else{
                    return symbolTable.at(i).length;
                }

            }
        }
    }
    return -1;
}

int judgeConst(string Name){
    /*

    */
    int i, cntTable = symbolTable.size();
    for(i=cntTable-1;i>=0;i--){
        if(symbolTable.at(i).ID==Name){
            if(symbolTable.at(i).field==globalFuncField
               || symbolTable.at(i).level < globalFuncLevel){
                if(symbolTable.at(i).IDobject==1)
                    return 1;
                else
                    return 0;
            }
        }

    }

    return 0;
}

int getConstValue(string Name){
    // 搭配 judgeConst 使用

    int i, cntTable = symbolTable.size(), ret_value = 0;
    for(i=cntTable-1;i>=0;i--){
        if(symbolTable.at(i).ID==Name){
            if(symbolTable.at(i).field==globalFuncField
               || symbolTable.at(i).level < globalFuncLevel){
                if(symbolTable.at(i).IDobject==1)
                    ret_value = symbolTable.at(i).value;
            }
        }

    }

    return ret_value;
}



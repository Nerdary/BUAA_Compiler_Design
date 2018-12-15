#include <stdio.h>
#include <string>

#include "symbolTable.h"
#include "syntaxAnalysis.h"
#include "error.h"

int globalFuncLevel = 0;            // ȫ��Ϊ0 main Ϊ1
string globalFuncField = "Global";  // ��ʼ����Ϊȫ��

using namespace std;

// ���� int char
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
        symbolTableError(errDuplicate);
        return -1;
    }

    symbolTable.push_back(tmp);
    printf("Push constant:%s value:%d in symbolTable.\n", ID.c_str(), value);
    return 0;
}
// ����
int pushArrayTable(string ID, int type, int length, int offset){
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

    // check
    if(checkDuplicate(ID)!=0){
        symbolTableError(errDuplicate);
        return -1;
    }

    symbolTable.push_back(tmp);
    printf("Push array:%s length:%d in symbolTable.\n", ID.c_str(), length);
    return 0;
}
// ��������
int pushFuncTable(string ID, int retType){
    // level & field ������������

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
        symbolTableError(errDuplicate);
        return -1;
    }

    symbolTable.push_back(tmp);
    printf("Push function:%s type:%d in symbolTable.\n", ID.c_str(), retType);
    return 0;
}
// �������β�
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
        symbolTableError(errDuplicate);
        return -1;
    }

    symbolTable.push_back(tmp);
    printf("Push variable:%s type:%d isPara:%d in symbolTable.\n", ID.c_str(), type, isPara);
    return 0;
}

int checkDuplicate(string ID){
    int i, cntTable = symbolTable.size();
    for(i=0;i<cntTable;i++){
//        if(strcmp(symbolTable.at(i).ID.c_str(), ID)==0
        if(symbolTable.at(i).ID==ID
        //    && symbolTable.at(i).IDobject==object
//            && strcmp(symbolTable.at(i).field, globalFuncField)){
            && symbolTable.at(i).field==globalFuncField){
            // ��ʶ����ͬ����������ͬ��ֱ�ӱ���
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

int searchName2Type(string Name, int mode){
    // �����ʶ��ID(name)���ر�ʶ������
    // update: �޸ĺ�Ҳ���Բ麯���ķ���ֵ������ mode: 0:others | 1:functions
    // 0:default | 1:int | 2:char | 3:else/unknown
    int i, cntTable = symbolTable.size();
    switch(mode){
    case(0):
        for(i=0;i<cntTable;i++){
            if(symbolTable.at(i).ID==Name){
                // ������ͬ
                if(symbolTable.at(i).field==globalFuncField){
                    // ��������ͬ
                    return symbolTable.at(i).IDType;
                }else if(symbolTable.at(i).level<globalFuncLevel){
                    // ������ͬ������ȫ�ֶ���
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
    printf("No such func in symbol table, fail to trans name to type.\n");
    return -1;
}

int getArrayLength(string ID){
    int i, cntTable = symbolTable.size();
    for(i=0;i<cntTable;i++){
        if(symbolTable.at(i).ID==ID){
            if(symbolTable.at(i).field==globalFuncField
               || symbolTable.at(i).level < globalFuncLevel){
                if(symbolTable.at(i).IDobject!=4){
                    // ����array
                    symbolTableError(errArrayLength);
                    return -1;
                }
                return symbolTable.at(i).length;
            }
        }


    }

    return -1;
}

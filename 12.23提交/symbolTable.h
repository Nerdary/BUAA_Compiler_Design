#ifndef SYMBOLTABLE_H_INCLUDED
#define SYMBOLTABLE_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

extern int globalFuncLevel;
extern string globalFuncField;


struct symbolTableItem{
    string ID;          // ��ʶ������
    int value;          // �洢������ֵ�� Ĭ��Ϊ0
    int IDobject;       // ��ʶ������
                        // 0: default | 1: constant | 2: variable | 3: function | 4: array
    int IDType;         // ��ʶ������
                        // 0: default | 1: int | 2: char
    string field;       // �����������ȵȵ�������
    int level;          // ��ǰ�ֳ���㼶��Ĭ��Ϊ1

//    int addr;           // ���ڱ������βΣ�������ջ�еĵ�ַ
                        // ���ں�����������ڵ�ַ, default 0
                        // ���ڳ��������0
                        // �������飬����Ԫ�ص�ַ
    int offset;         // �����βΣ�����Ժ������ƫ����
                        // ���ڱ���������Ժ������ƫ����
                        // ���ڳ���������Ժ������ƫ����
                        // �������飬����Ժ������ƫ����
    int length;         // ���鳤��,������Ĭ��ֵΪ0
    int funcType;       // ��������
                        // 0: default | 1: int | 2: char | 3: void | 4: main
    int isPara;         // ������β���Ϊ1���������Ϊ0
};

/*
struct arrayTableItem{
    int type;           // ����Ԫ������
    int length;         // ���鳤��

};

struct funcTableItem{
    int funcType;       // ����ֵ����
                        // 0: default | 1: int | 2: char | 3: void | 4: main
//    int paraList[][2];  // �����б�
                        // paraList[i][0] = 0: default | 1: int | 2: char
                        // paraList[i][1] = value: int | ascii: char

};

*/


// ���� int char
int pushConstantTable(string ID, int type, int value);
// ����
int pushArrayTable(string ID, int type, int length, int offset);
// ��������
int pushFuncTable(string ID, int retType);
// �������β�
int pushVarTable(string ID, int type, int offset, int isPara);

int checkDuplicate(string ID);

void printSymbolTable();

int searchName2Type(string Name, int mode);

int searchIsFunc(string Name);

int getArrayLength(string ID);

#endif // SYMBOLTABLE_H_INCLUDED

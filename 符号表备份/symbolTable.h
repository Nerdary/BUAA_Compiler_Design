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
//    int link;           // ͬһ�ֳ�������һ����ʶ����λ��
                        // �������������� link ֵΪ 0
    string field;       // �����������ȵȵ�������
//    int refer;          // ָ�����顢�����ڷֱ��е�������Ĭ��Ϊ0
    int level;          // ��ǰ�ֳ���㼶��Ĭ��Ϊ1

    int addr;           // ���ڱ������βΣ�������ջ�еĵ�ַ
                        // ���ں�����������ڵ�ַ, default 0
                        // ���ڳ��������0
                        // �������飬����Ԫ�ص�ַ
    int length;         // ���鳤��,������Ĭ��ֵΪ0
    int funcType;       // ��������
                        // 0: default | 1: int | 2: char | 3: void | 4: main

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
void pushConstantTable(string ID, int type, int value);
// ����
void pushArrayTable(string ID, int type, int length, int addr);
// ��������
void pushFuncTable(string ID, int retType);
// �������β�
void pushVarTable(string ID, int type, int addr);

#endif // SYMBOLTABLE_H_INCLUDED

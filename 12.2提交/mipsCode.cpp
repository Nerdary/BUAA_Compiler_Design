#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <stdlib.h>

#include "mipsCode.h"
#include "error.h"


int midCodeIndex = 0;
midCodeItem tmp;
vector<mipsItem> mipsCodeVector;
vector<globalRecordItem> globalRecordVector;
vector<functionInfo> allFuncInfoVector;
int globalRecordCount = 0;

void printMidCodeTmp(midCodeItem tmp){
    printf("%s\t", tmp.one.c_str());
    printf("%s\t", tmp.two.c_str());
    printf("%s\t", tmp.three.c_str());
    printf("%s\n", tmp.four.c_str());
}

void getMid(){
    tmp = midCodeVec.at(midCodeIndex++);
//    printf("> %d\t", midCodeIndex);
//    printMidCodeTmp(tmp);
}

void pushGlobalRecord(string ID, int offset){
    globalRecordItem tmp={
        ID,
        offset,
    };
    globalRecordVector.push_back(tmp);
}

int transNum(string token){
	int len = token.length();
	int num = 0,i;
	for(i=0;i<len;i++){
		if(token[i]>='0' && token[i]<='9'){
			num *= 10;
			num += (token[i] - '0');
		} else{
		//	printf("wrong character in transNum\n");
			return -1;
		}
	}
	return num;
}

void genMips(){     // �е������� programAnalysis
    getMid();

    // ����ȫ�� ��������
    if(tmp.one=="const"){
        while(true){
            // ��¼������Ϣ
            pushGlobalRecord(tmp.three, globalRecordCount);
            globalRecordCount++;
            // ����mips
            addi("$gp", "$gp", -4);
            // ������Ҫ�ж�һ��int �� char
            if(tmp.two=="int"){
                li("$t1", tmp.four);
            }else{
                char ctmp = tmp.four[0];
                int itmp = ctmp;
                li("$t1", to_string(itmp));
            }
            sw("$t1", 0, "$gp");

            getMid();
            if(tmp.one!="const")    break;
        }
    }

    while(tmp.one!="func" && tmp.one!="main" && tmp.one!="label"){
        // ��������
        if(tmp.one=="var"){
            // ��¼������Ϣ
            pushGlobalRecord(tmp.three, globalRecordCount);
            globalRecordCount++;
            // ����mips
            addi("$gp", "$gp", -4);
        }else if(tmp.one=="array"){
            // ��¼������Ϣ
            pushGlobalRecord(tmp.three, globalRecordCount);
            globalRecordCount += transNum(tmp.four);
            // ����mips
            li("$t1", to_string(4));
            li("$t2", tmp.four);
            mul("$t3", "$t1", "$t2");
            sub("$gp", "$gp", "$t3");
        }
        getMid();
    }

    while(tmp.one=="label" && tmp.three=="func"){
        // ���������Ӻ�������
        // ����mips����¼������Ϣ������������ջ
        vector<midCodeItem> otherMidCodeVec;
        otherMidCodeVec.push_back(tmp);
        getMid();
        // ���� func ��Ԫʽ
        if(tmp.one=="func"){
            otherMidCodeVec.push_back(tmp);
            getMid();
        }else{
            error();
            return ;
        }
        // ��������
        vector<funcRecordItem> funcSymbolTable;
        int funcSymbolCount = 0;
        while(tmp.one=="para"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 1, ""};
            funcSymbolTable.push_back(tmp2);
            getMid();
        }
        // �������岿��
        while(tmp.one=="const"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, tmp.four};
            funcSymbolTable.push_back(tmp2);
            getMid();
        }
        // ���������鶨�岿��
        while(tmp.one=="var" || tmp.one=="array"){
            if(tmp.one=="var"){
                funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, ""};
                funcSymbolTable.push_back(tmp2);
                getMid();
            }else{
                funcRecordItem tmp2 = {tmp.three, funcSymbolCount, 0, ""};
                funcSymbolCount += transNum(tmp.four);
                funcSymbolTable.push_back(tmp2);
                getMid();
            }
        }
        // ������䲿��
        // �ȼ�¼��һ��vector������mipsʱ��չ��
        while(true){
            // tmp.three!="func"
            if(tmp.one=="label"){
            //    printf(">>> \t");
            //    printMidCodeTmp(tmp);
                if(tmp.three=="func"){
                    break;
                }
            }
            otherMidCodeVec.push_back(tmp);
            getMid();
        }
//        printf("> check 1\n");
        // ����label_func_2
        otherMidCodeVec.push_back(tmp);
        getMid();
    //    printf("> check 2\n");
        // ���ú���������Ϣpush��allFuncInfoVector
        functionInfo tmpfunc = {funcSymbolTable, otherMidCodeVec};
        allFuncInfoVector.push_back(tmpfunc);
    }
//    printf("> check 3\n");
    // ���������к�������
    // ����������
    if(tmp.one=="main"){
//        printf("> about to get in handle main.\n");
        handleMain();
    }
}

// ��������������������ջ������MIPS
void handleMain(){
    getMid();
//    printf("> about to get in handle mid code.\n");
    handleMidCode();
}

// ����������䣬�������е���Ԫʽ�����ɴ󲿷�MIPS
void handleMidCode(){

    while(midCodeIndex <= (midCodeVec.size()-1)){
        // �Ѿ�Ԥ����һ��MidCode
        if(tmp.one=="BZ"){
            // BZ����һ��һ�������������Զ���$ti<$tj����ʽ����ֵ������
            // ֱ�Ӵӱ�����ȡֵ�ж�
        }else if(tmp.one=="GOTO"){
            j(tmp.two);
        }else if(tmp.one=="ret"){

        }else if(tmp.one=="print"){

        }else if(tmp.one=="scan"){

        }else if(tmp.one=="para"){

        }else if(tmp.one=="push"){

        }else if(tmp.one=="call"){

        }else if(tmp.one=="GOTO"){

        }else{
            if(tmp.one[0]=='$' && tmp.one[1]=='t'){
                // "$ti"
            }else{
                // ID
            }
        }
        getMid();
    }
}

void addi(string res, string in, int value){
    mipsItem tmp1 = {
        "addi",
        res,
        in,
        to_string(value),
    };
    mipsCodeVector.push_back(tmp1);
}

void add(string res, string n1, string n2){
    mipsItem tmp = {
        "add",
        res,
        n1,
        n2,
    };
    mipsCodeVector.push_back(tmp);
}

void sub(string res, string n1, string n2){
    mipsItem tmp = {
        "sub",
        res,
        n1,
        n2,
    };
    mipsCodeVector.push_back(tmp);
}

void sw(string value, int offset, string base){
    mipsItem tmp1 = {
        "sw",
        value,
        to_string(offset)+"("+base+")",
        "",
    };
    mipsCodeVector.push_back(tmp1);
}

void li(string res, string value){
    mipsItem tmp1 = {
        "li",
        res,
        value,
        "",
    };
    mipsCodeVector.push_back(tmp1);
}

void mul(string res, string m1, string m2){
     mipsItem tmp = {
        "mul",
        res,
        m1,
        m2,
    };
    mipsCodeVector.push_back(tmp);
}

void label(string label){
    mipsItem tmp = {
        label + ":",
        "",
        "",
        "",
    };
    mipsCodeVector.push_back(tmp);
}

void j(string label){
    mipsItem tmp = {
        "j",
        label,
        "",
        "",
    };
    mipsCodeVector.push_back(tmp);
}

void printMipsCode(){
//    printf("\nContents of MIPS codes\n1\t2\t3\t4\n");
    printf("\nContents of MIPS codes\n");
    printf(".data\n");
    printf(".text\n");
    int i, cntMips = mipsCodeVector.size();
    for(i=0;i<cntMips;i++){
        mipsItem tmp = mipsCodeVector.at(i);
        printf("%s", tmp.one.c_str());
        if(tmp.two!="")
            printf(" %s", tmp.two.c_str());
        if(tmp.three!="")
            printf(", %s", tmp.three.c_str());
        if(tmp.four!="")
            printf(", %s", tmp.four.c_str());
        printf("\n");
    }
}

void printGlobalRecord(){
//    printf("\nContents of MIPS codes\n1\t2\t3\t4\n");
    printf("\nContents of global Record\n");
    int i, cntMips = globalRecordVector.size();
    for(i=0;i<cntMips;i++){
        globalRecordItem tmp = globalRecordVector.at(i);
        printf("%d\t%s\t%d\n", i, tmp.ID.c_str(), tmp.offset);
    }
}

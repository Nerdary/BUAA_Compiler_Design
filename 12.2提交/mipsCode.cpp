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

void genMips(){     // 有点类似于 programAnalysis
    getMid();

    // 处理全局 常量定义
    if(tmp.one=="const"){
        while(true){
            // 记录常量信息
            pushGlobalRecord(tmp.three, globalRecordCount);
            globalRecordCount++;
            // 生成mips
            addi("$gp", "$gp", -4);
            // 这里需要判断一下int 和 char
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
        // 变量声明
        if(tmp.one=="var"){
            // 记录变量信息
            pushGlobalRecord(tmp.three, globalRecordCount);
            globalRecordCount++;
            // 生成mips
            addi("$gp", "$gp", -4);
        }else if(tmp.one=="array"){
            // 记录变量信息
            pushGlobalRecord(tmp.three, globalRecordCount);
            globalRecordCount += transNum(tmp.four);
            // 生成mips
            li("$t1", to_string(4));
            li("$t2", tmp.four);
            mul("$t3", "$t1", "$t2");
            sub("$gp", "$gp", "$t3");
        }
        getMid();
    }

    while(tmp.one=="label" && tmp.three=="func"){
        // 处理所有子函数定义
        // 生成mips，记录所有信息用于生成运行栈
        vector<midCodeItem> otherMidCodeVec;
        otherMidCodeVec.push_back(tmp);
        getMid();
        // 读到 func 四元式
        if(tmp.one=="func"){
            otherMidCodeVec.push_back(tmp);
            getMid();
        }else{
            error();
            return ;
        }
        // 参数部分
        vector<funcRecordItem> funcSymbolTable;
        int funcSymbolCount = 0;
        while(tmp.one=="para"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 1, ""};
            funcSymbolTable.push_back(tmp2);
            getMid();
        }
        // 常量定义部分
        while(tmp.one=="const"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, tmp.four};
            funcSymbolTable.push_back(tmp2);
            getMid();
        }
        // 变量、数组定义部分
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
        // 复合语句部分
        // 先记录进一个vector，生成mips时再展开
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
        // 存入label_func_2
        otherMidCodeVec.push_back(tmp);
        getMid();
    //    printf("> check 2\n");
        // 将该函数所有信息push进allFuncInfoVector
        functionInfo tmpfunc = {funcSymbolTable, otherMidCodeVec};
        allFuncInfoVector.push_back(tmpfunc);
    }
//    printf("> check 3\n");
    // 处理完所有函数定义
    // 处理主函数
    if(tmp.one=="main"){
//        printf("> about to get in handle main.\n");
        handleMain();
    }
}

// 处理主函数，生成运行栈，生成MIPS
void handleMain(){
    getMid();
//    printf("> about to get in handle mid code.\n");
    handleMidCode();
}

// 即处理复杂语句，处理所有的四元式，生成大部分MIPS
void handleMidCode(){

    while(midCodeIndex <= (midCodeVec.size()-1)){
        // 已经预读了一条MidCode
        if(tmp.one=="BZ"){
            // BZ的上一条一定是条件，所以对于$ti<$tj的形式，将值存起来
            // 直接从变量中取值判断
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

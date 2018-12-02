#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <stdlib.h>

#include "mipsCode.h"


int midCodeIndex = 0;
midCodeItem tmp;
vector<mipsItem> mipsCodeVector;

void getMid(){
    tmp = midCodeVec.at(midCodeIndex++);
}

void genMips(){
    getMid();

    // 处理全局 常量定义
    if(tmp.one=="const"){
        while(true){
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

            // 终止
            getMid();
            if(tmp.one!="const")    break;
        }
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

void printMipsCode(){
//    printf("\nContents of MIPS codes\n1\t2\t3\t4\n");
    int i, cntMips = mipsCodeVector.size();
    for(i=0;i<cntMips;i++){
        mipsItem tmp = mipsCodeVector.at(i);
        printf("%s ", tmp.one.c_str());
        printf("%s, ", tmp.two.c_str());
        printf("%s, ", tmp.three.c_str());
        printf("%s\n", tmp.four.c_str());
    }
}

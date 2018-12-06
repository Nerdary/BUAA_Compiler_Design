#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <stdlib.h>

#include "mipsCode.h"
#include "error.h"


int midCodeIndex = 0;
int funcLevel = 0;      // 默认main为1，global为0
midCodeItem tmp;
vector<mipsItem> mipsCodeVector;
vector<globalRecordItem> globalRecordVector;
vector<functionInfo> allFuncInfoVector;
vector<functionInfo> funcStack;
/*  allFuncInfoVector 为存储函数信息的vector，查找函数
    funcStack 为运行栈，记录相应信息，查找变量
*/
int globalRecordCount = 0;
int globalValueOfFp = 0;
int compareResult;      // 关系运算四元式的结果，作为跳转的条件
                        // 将此结果存入寄存器$s2

int offsetGp = 0;      // 全局变量、常量的偏移量，因为要访问，所以$gp最好不要变
int offsetFp = 0;
int offsetSp = 0;

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
        ID,         // 变量名
        offset,     // 相对于$gp的偏移量
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
            pushGlobalRecord(tmp.three, ++globalRecordCount);   // 第一项count为-1
            // 生成mips
            // addi("$gp", "$gp", -4);
            offsetGp -= 4;
            // 这里需要判断一下int 和 char
            if(tmp.two=="int"){
                li("$t1", tmp.four);
            }else{
                char ctmp = tmp.four[0];
                int itmp = ctmp;
                li("$t1", to_string(itmp));
            }
            sw("$t1", offsetGp, "$gp");

            getMid();
            if(tmp.one!="const")    break;
        }
    }

    // 处理全局变量、数组信息，生成MIPS
    while(tmp.one!="func" && tmp.one!="main" && tmp.one!="label"){
        // 变量声明
        if(tmp.one=="var"){
            // 记录变量信息
            pushGlobalRecord(tmp.three, ++globalRecordCount);
            // 生成mips
            // addi("$gp", "$gp", -4);
            offsetGp -= 4;
        }else if(tmp.one=="array"){
            // 记录变量信息
            globalRecordCount += transNum(tmp.four);
            pushGlobalRecord(tmp.three, globalRecordCount);
            // 生成mips
            li("$t1", to_string(4));
            li("$t2", tmp.four);
            mul("$t3", "$t1", "$t2");
            //sub("$gp", "$gp", "$t3"); // 不能改变$gp
            offsetGp -= 4 * transNum(tmp.four);
        }else{
            printf(">>> ERROR: in dealing with global var and array.\n");
        }
        getMid();
    }

    // 处理所有子函数定义
    while(tmp.one=="label" && tmp.three=="func"){
        // 生成mips，记录所有信息用于生成运行栈
        getMid();
        if(tmp.one=="func"){
            // 在这里生成函数头部，操作fp sp，保存ra
            sw("$fp", 0, "$sp");
            add("$fp", "$sp", "$zero");
            addi("$sp", "$sp", -4);
            sw("$ra", 0, "$sp");

            getMid();
        }else{
            error();
            return ;
        }

        vector<funcRecordItem> funcSymbolTable;
        int funcSymbolCount = 0;
        // 参数部分
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
        while(true){
            if(tmp.one=="label"){
            //    printMidCodeTmp(tmp);
                if(tmp.three=="func"){
                    break;
                }
            }
            // 这里调用handle一句句生成mips
            handleMidCode();
            getMid();
        }
        // 当前为label_func_2,生成一句jr
        jr();

        getMid();

        // 将该函数所有信息push进allFuncInfoVector
        functionInfo tmpfunc = {funcLevel,          // level
                                globalValueOfFp,
                                globalValueOfFp + 8 + 4 * funcSymbolCount,
                                funcSymbolCount,    // length
                                funcSymbolTable};   // symbol table
        globalValueOfFp += (8 + 4 * funcSymbolCount);
        allFuncInfoVector.push_back(tmpfunc);
    }
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

// 即处理复杂语句，处理一条的四元式，生成大部分MIPS
void handleMidCode(){
    // 已经预读了一条MidCode
    if(tmp.one=="BZ"){
        // BZ的上一条一定是条件，所以对于$ti<$tj的形式，将值存起来
        // 直接从变量中取值判断
        // 这个分支应该是进不来的，进来的先抱一个错吧
        printf("Unexpected branch 'BZ' \n");
        getMid();
    }else if(tmp.one=="GOTO"){
        j(tmp.two);
        getMid();
    }else if(tmp.one=="ret"){
        // ret        $t24
        // ret
        if(tmp.two==""){
            // 返回值为空，跳过就行
            getMid();
        }else{
            // 返回值存入寄存器
            add("$v0", "$zero", tmp.two);
            getMid();
        }
    }else if(tmp.one=="print"){
        // print "xxxxx"
        // print $ti
        if(tmp.two[0]=='$' && tmp.two[1]=='t'){
            // v0 = 1, a0 = int
            addi("$v0", "$zero", 1);
            add("$a0", "$zero", tmp.two);
            syscall();
        }else{
            // v0 = 11, ao = char
            string content = tmp.two;
            int i, tmpi;
            char tmpc;
            for(i=0;i<content.length();i++){
                tmpc = content[i];
                tmpi = tmpc;
                addi("$v0", "$zero", 11);
                addi("$a0", "$zero", tmpi);
                syscall();
            }
        }
    }else if(tmp.one=="scan"){

    }else if(tmp.one=="para"){

    }else if(tmp.one=="push"){

    }else if(tmp.one=="call"){

    }else{
        if(tmp.one[0]=='$' && tmp.one[1]=='t'){
            // "$ti" 一共有si种可能
            //  $t6         $t4           *         $t5
            if(tmp.two[0]=='$' && tmp.two[1]=='t' &&
               tmp.four[0]=='$' && tmp.four[1]=='t'){
                if(tmp.three=="+"){
                    add(tmp.one, tmp.two, tmp.four);
                    getMid();
                }else if(tmp.three=="-"){
                    sub(tmp.one, tmp.two, tmp.four);
                    getMid();
                }else if(tmp.three=="*"){
                    mul(tmp.one, tmp.two, tmp.four);
                    getMid();
                }else if(tmp.three=="/"){
                    div(tmp.one, tmp.two, tmp.four);
                    getMid();
                }else{
                    printf("Unknown opt in '$ $ opt $' mid code.\n");
                    error();
                    return ;
                }
            }
            //  $t20           <        $t21
            else if(tmp.three[0]=='$' && tmp.three[1]=='t'){
            /*
                BEQZ条件转移指令，当寄存器中内容为0时转移发生BEQZ R1,0
                BENZ条件转移指令，当寄存器中内容不为0时转移发生BNEZ R1,0
                BEQ条件转移指令，当两个寄存器内容相等时转移发生BEQ R1,R2
                BNE条件转移指令，当两个寄存器中内容不等时转移发生BNE R1,R2
            */
                if(tmp.two=="<"){
                    sub("$s1", tmp.three, tmp.one); // s1 = 3 - 1
                    getMid();  // 一定是 BZ
                    // s1<=0则跳转
                    blez("$s1", tmp.three);
                    getMid();

                }else if(tmp.two==">"){
                    sub("$s1", tmp.one, tmp.three); // s1 = 1 - 3
                    getMid();
                    blez("$s1", tmp.three);
                    getMid();

                }else if(tmp.two=="<="){
                    sub("$s1", tmp.three, tmp.one); // s1 = 3 - 1
                    getMid();
                    // s1<0则跳转
                    bltz("$s1", tmp.three);
                    getMid();

                }else if(tmp.two==">="){
                    sub("$s1", tmp.one, tmp.three); // s1 = 1 - 3
                    getMid();
                    bltz("$s1", tmp.three);
                    getMid();

                }else if(tmp.two=="=="){
                    sub("$s1", tmp.one, tmp.three);
                    getMid();
                    bne("$s1", "$zero", tmp.three);
                    getMid();

                }else if(tmp.two=="!="){
                    sub("$s1", tmp.one, tmp.three);
                    getMid();
                    beq("$s1", "$zero", tmp.three);
                    getMid();

                }else{
                    printf("Unknown compare opt in '$ compare $' mid code.\n");
                    error();
                    return ;
                }
            }
            // $t22           1
            else if(transNum(tmp.two)!=-1){
                int value1 = transNum(tmp.two);
                // 直接赋值
                addi(tmp.one, "$zero", value1);
                getMid();
            }
            // $t1           a
            else{
                int flag = 1;
                searchResult res1 = searchStackID(tmp.one);
                if(res1.index==-1){
                    flag = 2;
                    int res2 = searchGlobalID(tmp.one);
                }
                if(res2==-1){
                    printf("Failed to find the ID:%s\n", tmp.one.c_str());
                    return ;
                }

                if(flag==1){
                    // 在符号栈表中找到了ID
                    // 0 1 2 3 ...
                    int fp = res1.targetFp;
                    int offset1 = 8 + 4 * res1.index;
                    addi("$s1", "$zero", fp);
                    lw(tmp.one, offset1, "$s1");
                    // read next
                    getMid();
                }else{
                    // 在全局表中查到了ID
                    // res2*4 是相对于gp的offset
                    int offset2 = 4 * res2;
                    lw(tmp.one, offset2, "$gp");
                    // read next
                    getMid();
                }

                getMid();
            }
        }else{
            // ID
            if(tmp.two[0]=='$' && tmp.two[1]=='t'){
                int flag = 1;
                searchResult res1 = searchStackID(tmp.one);
                if(res1.index==-1){
                    flag = 2;
                    int res2 = searchGlobalID(tmp.one);
                }
                if(res2==-1){
                    printf("Failed to find the ID:%s\n", tmp.one.c_str());
                    return ;
                }

                // 查找成功
                if(flag==1){
                    // 在符号栈表中找到了ID
                    // 0 1 2 3 ...
                    int fp = res1.targetFp;
                    int offset1 = 8 + 4 * res1.index;
                    addi("$s1", "$zero", fp);
                    sw(tmp.two, offset1, "$s1");
                    // read next
                    getMid();
                }else{
                    // 在全局表中查到了ID
                    // res2*4 是相对于gp的offset
                    int offset2 = 4 * res2;
                    sw(tmp.two, offset2, "$gp");
                    // read next
                    getMid();
                }

            }else{
                error();
                return ;
            }
        }
    }


}
searchResult searchStackID(string ID){
    // 在运行栈中查找符号名
    int i, j, length = funcStack.size();
    // 先查自己的域
    functionInfo info = funcStack.at(length-1);
    for(j=0;j<info.funcSymbolTable.size();j++){
        funcRecordItem tmp = info.funcSymbolTable.at(j);
        if(tmp.ID==ID){
        //    return tmp.offset;
            searchResult res = {
                info.fp,
                j,
            };
            return res;
        }
    }

    // 再查嵌套的域
    for(i=length-1;i>=0;i--){
        functionInfo info1 = funcStack.at(i);
        if(funcLevel > info1.level){
            // 更高级的域
            for(j=0;j<info1.funcSymbolTable.size();j++){
                funcRecordItem tmp = info1.funcSymbolTable.at(j);
                if(tmp.ID==ID){
                    searchResult res = {
                        info1.fp,
                        j,
                    };
                    return res;
                }

            }
        }
    }
    // 没有查到
    searchResult ures = {
        -1,
        -1,
    };
    return ures;
}

int searchGlobalID(string ID){
    // 查全局表
    int i, length = globalRecordVector.size();
    for(i=0;i<length;i++){
        funcRecordItem tmp = globalRecordVector.at(i);
        if(tmp.ID == ID)
            return tmp.offset;
    }
    // 没有查到
    return -1;
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

void lw(string res, int offset, string base){
    mipsItem tmp = {
        "lw",
        res,
        to_string(offset)+"("+base+")",
        "",
    };
    mipsCodeVector.push_back(tmp);
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

void div(string res, string d1, string d2){
    mipsItem tmp = {
        "div",
        res,
        d1,
        d2,
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

void jr(){
    mipsItem tmp = {
        "jr",
        "$ra",
        "",
        "",
    };
    mipsCodeVector.push_back(tmp);
}

void blez(string tag, string label){
    mipsItem tmp = {
        "blez",
        tag,
        label,
        "",
    };
    mipsCodeVector.push_back(tmp);
}

void bltz(string tag, string label){
    mipsItem tmp = {
        "bltz",
        tag,
        label,
        "",
    };
    mipsCodeVector.push_back(tmp);
}

void bne(string s1, string s2, string label){
    mipsItem tmp = {
        "bne",
        s1,
        s2,
        label,
    };
    mipsCodeVector.push_back(tmp);
}

void beq(string s1, string s2, string label){
    mipsItem tmp = {
        "beq",
        s1,
        s2,
        label,
    };
    mipsCodeVector.push_back(tmp);
}

void syscall(){
    mipsItem tmp = {
        "syscall",
        "",
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

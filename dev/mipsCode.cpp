#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <iomanip>

#include "mipsCode.h"
#include "syntaxAnalysis.h"     // 为了引用定义的AllFuncInfo
#include "error.h"

using namespace std;

int midCodeIndex = 0;
int funcLevel = 0;              // 默认main为1，global为0
int globalRecordCount = 0;
int globalValueOfFp = 0;
int compareResult;              // 关系运算四元式的结果，作为跳转的条件
                                // 将此结果存入寄存器$s2
int paraCount;                  // 用于记录函数调用的参数个数
int offsetGp = 0;               // 全局变量、常量的偏移量，因为要访问，所以$gp最好不要变
int offsetFp = 0;
int offsetSp = 0;

int global_v0 = -1;             // 优化：减少不必要的v0赋值 v0
int global_a0 = -1;             // 优化：减少不必要的a0赋值

string currentFuncName = "global";

funcInfoItem tmpAnaInfo;
midCodeItem nullItem = {"", "", "", ""};
midCodeItem tmp;
vector<mipsItem> mipsCodeVector;
vector<mipsItem> mipsCodeVector_Optimize;   // 优化后结果
vector<mipsItem> mipsCodeVector_tmp;        // 优化中间产物
vector<globalRecordItem> globalRecordVector;
vector<functionInfo> allFuncInfoVector;
vector<functionInfo> funcStack;

void printMidCodeTmp(midCodeItem tmp){
    printf("%s\t", tmp.one.c_str());
    printf("%s\t", tmp.two.c_str());
    printf("%s\t", tmp.three.c_str());
    printf("%s\n", tmp.four.c_str());
}

void getMid(){
    if(midCodeIndex>=midCodeVec.size()){
        tmp = nullItem;
        return ;
    }
    tmp = midCodeVec.at(midCodeIndex++);
    //printf("> %d\t", midCodeIndex);
    //printMidCodeTmp(tmp);
}

void pushGlobalRecord(string ID, int offset, string type){
    globalRecordItem tmp={
        ID,         // 变量名
        offset,     // 相对于$gp的偏移量
        type,       // 类型
    };
    globalRecordVector.push_back(tmp);
}

transResult transNum(string token){
	int len = token.length();
	int num = 0,i, tag = 1;
	if(token[0]=='+' || token[0]=='-'){
        if(token[0]=='-')
            tag = -1;
	}
	for(i=0;i<len;i++){
        if(i==0 && (token[i]=='+' || token[i]=='-'))
            continue;

		if(token[i]>='0' && token[i]<='9'){
			num *= 10;
			num += (token[i] - '0');
		} else{
            transResult errRes = {0, 0};
			return errRes;
		}
	}

    transResult theRes = {1, num*tag};
	return theRes;
}

void genMips(){   //类似于 programAnalysis
    // 添加开关：0: normal 1: optimized

    getMid();

    // 处理全局 常量定义
    if(tmp.one=="const"){
        while(true){
            // 记录常量信息
            pushGlobalRecord(tmp.three, globalRecordCount++, tmp.two);   // 第一项count为0
            // 生成mips
            offsetGp -= 4;
            // 这里需要判断一下int 和 char
            if(tmp.two=="int"){
                li("$t1", tmp.four);
            }else{
//                char ctmp = tmp.four[0];
//                int itmp = ctmp;
//                li("$t1", to_string(itmp));

                li("$t1", tmp.four);
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
            pushGlobalRecord(tmp.three, globalRecordCount++, tmp.two);
            // 生成mips
            offsetGp -= 4;
        }else if(tmp.one=="array"){
            // 记录变量信息

            pushGlobalRecord(tmp.three, globalRecordCount, tmp.two);
            globalRecordCount += transNum(tmp.four).value;
            // 数组的存储方式  head + offset

            // 生成mips
            li("$t1", to_string(4));
            li("$t2", tmp.four);
            mul("$t3", "$t1", "$t2");

            offsetGp -= 4 * transNum(tmp.four).value;
        }else{
            printf(">>> ERROR: in dealing with global var and array.\n");
        }
        getMid();
    }

    // 跳转到主函数
    j("main");

    // 处理所有子函数定义
    while(tmp.one=="label" && tmp.three=="func"){
        // 生成mips，记录所有信息用于生成运行栈
        mipsLabel(tmp.two);
        getMid();
        if(tmp.one=="func"){
            // 记录函数名
            currentFuncName = tmp.three;

            // 将函数信息载体取出来
            int i;
            for(i=0;i<AllFuncInfo.size();i++){
                if(AllFuncInfo.at(i).funcName==currentFuncName){
                    tmpAnaInfo = AllFuncInfo.at(i);
                    break;
                }
            }

            // 在这里生成函数头部，操作fp sp，保存ra
            sw("$fp", 0, "$sp");
            add("$fp", "$sp", "$zero");
            addi("$sp", "$sp", -4);
            sw("$ra", 0, "$sp");
            addi("$sp", "$sp", -4);

            getMid();
        }else{
            error();
            return ;
        }

         // 记录所有临时变量
        addi("$sp", "$sp", -36);
        sw("$t1", 36, "$sp");
        sw("$t2", 32, "$sp");
        sw("$t3", 28, "$sp");
        sw("$t4", 24, "$sp");
        sw("$t5", 20, "$sp");
        sw("$t6", 16, "$sp");
        sw("$t7", 12, "$sp");
        sw("$t8",  8, "$sp");
        sw("$t9",  4, "$sp");

        vector<funcRecordItem> funcSymbolTable;
        int funcSymbolCount = 0;

        // 参数部分
        // 先计数有多少个para
        paraCount = 0;
        int tmpParaIndex = midCodeIndex - 1;
        midCodeItem tmpParaItem;

        while(true){
            tmpParaItem = midCodeVec.at(tmpParaIndex);
            if(tmpParaItem.one != "para")
                break;

            paraCount++;
            tmpParaIndex++;
        }

        int paraTypeCount = 0;
        while(tmp.one=="para"){
            // 在函数信息表中登记参数信息
            int paraType = tmpAnaInfo.funcParaType.at(paraTypeCount);
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 1, "", to_string(paraType)};
            funcSymbolTable.push_back(tmp2);

            // 从栈中加载参数的值 paracount

            // 必须查函数表才能知道有几个参数
            int gap = 4 * paraCount + 8 + 36;
            addi("$s1", "$sp", gap);
            lw("$t1", 0, "$s1");
            sw("$t1", 0, "$sp");
            addi("$sp", "$sp", -4);

            // 预读下一条
            getMid();

            paraTypeCount++;
        }


        // 常量定义部分
        while(tmp.one=="const"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, tmp.four, tmp.two};
            funcSymbolTable.push_back(tmp2);

            // 留出空间并填入常量的值
            li("$t1", tmp.four);
            sw("$t1", 0, "$sp");
            addi("$sp", "$sp", -4);
            getMid();
        }

        // 变量、数组定义部分
        int optimize_cnt = 0;
        while(tmp.one=="var" || tmp.one=="array"){
            if(tmp.one=="var"){
                funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, "", tmp.two};
                funcSymbolTable.push_back(tmp2);

//                addi("$sp", "$sp", -4);
                optimize_cnt += 1;
                getMid();
            }else{
                funcRecordItem tmp2 = {tmp.three, funcSymbolCount, 0, "", tmp.two};
                funcSymbolCount += transNum(tmp.four).value;
                funcSymbolTable.push_back(tmp2);

//                addi("$sp", "$sp", -4*transNum(tmp.four).value);
                optimize_cnt += transNum(tmp.four).value;
                getMid();
            }

            // optimization: add together
            addi("$sp", "$sp", -4*optimize_cnt);
        }

        functionInfo tmpfunc = {currentFuncName,    // name / ID
                                funcLevel,          // level
                                0,                  // not main
                                funcSymbolTable};   // symbol table

        allFuncInfoVector.push_back(tmpfunc);
        funcStack.push_back(tmpfunc);

        // 复合语句部分
        while(true){
            if(tmp.one=="label"){

                if(tmp.three=="func"){
                    break;
                }else{
                    // 生成普通的label
                    label("label" + tmp.two);
                    getMid();
                }
            }

            // 这里调用handle一句句生成mips
            if(tmp.one!="label")
                handleMidCode();
        }

        // v0
        global_v0 = -1;

        // 恢复所有局部变量
        lw("$t1",  -8, "$fp");
        lw("$t2", -12, "$fp");
        lw("$t3", -16, "$fp");
        lw("$t4", -20, "$fp");
        lw("$t5", -24, "$fp");
        lw("$t6", -28, "$fp");
        lw("$t7", -32, "$fp");
        lw("$t8", -36, "$fp");
        lw("$t9", -40, "$fp");

        // 当前为label_func_2,取出ra，生成一句jr
        lw("$ra", -4, "$fp");

        // 暂时以这种方式恢复sp
        add("$sp", "$zero", "$fp");
        /*  本来需要保存sp
            现在有一个替代的方案：
            恢复sp时sp=fp+paraCount*4
            paraCount   */
        addi("$sp", "$sp", paraCount*4);

        // 将参数计数器清零
        paraCount = 0;

        // 还要恢复fp
        lw("$fp", 0, "$fp");

        jr();

        // 删除运行栈中该函数的部分
        // funcStack.pop_back();

        getMid();
    }

    // 处理完所有函数定义
    // 处理主函数
    if(tmp.one=="main"){
        handleMain();
    }
}

// 处理主函数，生成运行栈，生成MIPS
void handleMain(){
    //
    currentFuncName = "main";
    mipsLabel("main");
    getMid();

    add("$fp", "$sp", "$zero");     // 不需要动sp


    // 在这里处理所有常量变量数组定义
    int funcSymbolCount = 0;
    vector<funcRecordItem> mainSymbolTable;

    // 常量部分
    while(tmp.one=="const"){
        funcRecordItem tmpm = {tmp.three, funcSymbolCount++, 0, tmp.four, tmp.two};
        mainSymbolTable.push_back(tmpm);
        // 留出空间并填入常量的值

        if(tmp.two=="int"){
            li("$t1", tmp.four);
        }else{
            char ctmp = tmp.four[0];
            int itmp = ctmp;
            li("$t1", to_string(itmp));
        }
        sw("$t1", 0, "$sp");
        addi("$sp", "$sp", -4);
        getMid();
    }

    // 变量数组
    // 变量、数组定义部分
    while(tmp.one=="var" || tmp.one=="array"){
        if(tmp.one=="var"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, "", tmp.two};
            mainSymbolTable.push_back(tmp2);

            addi("$sp", "$sp", -4);
            getMid();
        }else{
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount, 0, "", tmp.two};
            funcSymbolCount += transNum(tmp.four).value;
            mainSymbolTable.push_back(tmp2);

            addi("$sp", "$sp", -4 * transNum(tmp.four).value);
            getMid();
        }
    }


    functionInfo tmpfunc = {"main",
                            1,                  // level
                            1,                  // is main
                            mainSymbolTable};   // symbol table

    funcStack.push_back(tmpfunc);
    allFuncInfoVector.push_back(tmpfunc);

    currentFuncName = "main";

    while(true){
        handleMidCode();
        if(midCodeIndex >= midCodeVec.size())
            break;
    }
    handleMidCode();
    mipsEndLabel();

    // 尚未优化
    mipsCodeVector_Optimize = mipsCodeVector;
}

// 即处理复杂语句，处理一条的四元式，生成大部分MIPS
void handleMidCode(){

    // 已经预读了一条MidCode
    if(tmp.one=="BZ"){
        // BZ的上一条一定是条件，所以对于$ti<$tj的形式，将值存起来
        // 直接从变量中取值判断
        // 这个分支是可以进来的，比如条件是一个表达式
        // 表达式的值为0则不满足， 非0则满足


        beq("$s1", "$zero", "label" + tmp.three);
        getMid();

    }else if(tmp.one=="GOTO"){
        j("label" + tmp.three);
        getMid();

    }else if(tmp.one=="label"){
        label("label" + tmp.two);
        getMid();

    }else if(tmp.one=="ret"){
        // for example: ret $t24
        if(tmp.two==""){
            // 返回值为空，跳过就行

        }else{
            // 返回值存入寄存器
            add("$v0", "$zero", tmp.two);
        }

        // 将global v0设置为-1， 表示下次使用syscall时一定要给v0赋值
        global_v0 = -1;
        // printf("set global v0 to -1\n");

        if(currentFuncName!="main"){
            // 恢复所有局部变量
            lw("$t1",  -8, "$fp");
            lw("$t2", -12, "$fp");
            lw("$t3", -16, "$fp");
            lw("$t4", -20, "$fp");
            lw("$t5", -24, "$fp");
            lw("$t6", -28, "$fp");
            lw("$t7", -32, "$fp");
            lw("$t8", -36, "$fp");
            lw("$t9", -40, "$fp");

            // 还需要干很多事
            lw("$ra", -4, "$fp");

            // 暂时以这种方式恢复sp
            add("$sp", "$zero", "$fp");

            // 更新之后新的恢复sp的方法
            addi("$sp", "$sp", paraCount*4);

            // ret 分支不清零

            // 还要恢复fp
            lw("$fp", 0, "$fp");


            jr();
        }else{
            // main 里面的return ;
            j("THIS_IS_THE_END_OF_ALL");
        }

        getMid();

    }else if(tmp.one=="print"){
        // print "xxxxx"
        // print $ti
        if(tmp.two[0]=='$' && tmp.two[1]=='t' && tmp.three!="func"){
            // v0 = 1, a0 = int
            if(global_v0 != 1){
                addi("$v0", "$zero", 1);
                global_v0 = 1;
            }

            add("$a0", "$zero", tmp.two);
            syscall();

            nextLine();
        }else if(tmp.two[0]=='$' && tmp.two[1]=='t' && tmp.three=="func"){
            // print $ti func
            // 返回值类型为char的函数调用
            if(global_v0 != 11){
                addi("$v0", "$zero", 11);
                global_v0 = 11;
            }

            add("$a0", "$zero", tmp.two);
            syscall();

            nextLine();
        }else if(tmp.three=="IDSY"){
            // printf("check: PRINT IDSY: global_v0: %d\n", global_v0);

            // 对象是单个标识符，判断是int还是char
            int thisPrintType = searchIDType(currentFuncName, tmp.two);
            if(thisPrintType==1){
                // int
                if(global_v0 != 1){
                    addi("$v0", "$zero", 1);
                    global_v0 = 1;
                }

                add("$a0", "$zero", tmp.four);
                syscall();

                nextLine();
            }else if(thisPrintType==2){
                // ! char
                if(global_v0 != 11){
                    addi("$v0", "$zero", 11);
                    global_v0 = 11;
                }

                add("$a0", "$zero", tmp.four);
                syscall();

                nextLine();
            }else{
                printf("UNEXPECTED print ID type.\n");
            }

        }else{
            // v0 = 11, ao = char
            string content = tmp.two;
            int i, tmpi;
            char tmpc;
            for(i=0;i<content.length();i++){
                tmpc = content[i];
                tmpi = tmpc;
                if(global_v0 != 11){
                    addi("$v0", "$zero", 11);
                    global_v0 = 11;
                }

                addi("$a0", "$zero", tmpi);
                syscall();
            }

            nextLine();
        }

        getMid();
    }else if(tmp.one=="scan"){
        // v0 = 5
        if (tmp.two=="int"){
            addi("$v0", "$zero", 5);        // scan 里面一定要给v0重新赋值，v0是不能重复利用的
        }else if(tmp.two=="char"){
            addi("$v0", "$zero", 12);
        }

        global_v0 = -1;
        syscall();


        // assignment
        // search ID tmp.three
        int flag = 1;
        int res2;
        searchResult res1 = searchStackID(currentFuncName, tmp.three);
        if(res1.index==-1){
            flag = 2;
            res2 = searchGlobalID(tmp.three);
        }
        if(res2==-1){
            printf("Failed to find the ID:%s\n", tmp.three.c_str());
            return ;
        }

        if(flag==1){
            // find ID in function's stack
            int offset1 = 4 * res1.index;

            if(res1.isMain!=1){
                offset1 += (8+36);
            }

            sw("$v0", -offset1, "$fp");

            // read next
            getMid();
        }else{

            // find ID in global table
            int offset2 = 4 * res2 + 4;
            sw("$v0", -offset2, "$gp");
            // read next
            getMid();
        }

    }else if(tmp.one=="para"){
        // 应该进不来这个分支
        printf("Unexpected 'parameter' branch.\n");
        getMid();
    }else if(tmp.one=="push"){

        sw(tmp.two, 0, "$sp");
        addi("$sp", "$sp", -4);

        getMid();

    }else if(tmp.one=="call"){

        // 这里只生成jal，加载参数放在读函数的时候
        jal(tmp.two);
        getMid();

    }else{
        if(tmp.one[0]=='$' && tmp.one[1]=='t'){
            // "$ti" 一共有...种可能
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
                    blez("$s1", "label" + tmp.three);
                    getMid();

                }else if(tmp.two==">"){
                    sub("$s1", tmp.one, tmp.three); // s1 = 1 - 3
                    getMid();
                    blez("$s1", "label" + tmp.three);
                    getMid();

                }else if(tmp.two=="<="){
                    sub("$s1", tmp.three, tmp.one); // s1 = 3 - 1
                    getMid();
                    // s1<0则跳转
                    bltz("$s1", "label" + tmp.three);
                    getMid();

                }else if(tmp.two==">="){
                    sub("$s1", tmp.one, tmp.three); // s1 = 1 - 3
                    getMid();
                    bltz("$s1", "label" + tmp.three);
                    getMid();

                }else if(tmp.two=="=="){
                    sub("$s1", tmp.one, tmp.three);
                    getMid();
                    bne("$s1", "$zero", "label" + tmp.three);
                    getMid();

                }else if(tmp.two=="!="){
                    sub("$s1", tmp.one, tmp.three);
                    getMid();
                    beq("$s1", "$zero", "label" + tmp.three);
                    getMid();

                }else if(tmp.two=="-"){
                    // $t1  = - $t2
                    sub(tmp.one, "$zero", tmp.three);
                    getMid();

                }else{
                    printf("Unknown compare opt in '$ compare $' mid code.\n");
                    error();
                    return ;
                }
            }
            // $t22           1
            else if(transNum(tmp.two).success==1){
                int value1 = transNum(tmp.two).value;
                // 直接赋值
                addi(tmp.one, "$zero", value1);
                getMid();

            }
            // $t1           a
            else if(tmp.three=="" && tmp.two!="RET"){
                int flag = 1;
                int res2;
                searchResult res1 = searchStackID(currentFuncName, tmp.two);
                if(res1.index==-1){
                    flag = 2;
                    res2 = searchGlobalID(tmp.two);
                }
                if(res2==-1){
                    printf("Failed to find the ID:%s\n", tmp.two.c_str());
                    return ;
                }

                if(flag==1){
                    // 在符号栈表中找到了ID
                    // 0 1 2 3 ...
                    int offset1 = 4 * res1.index;
                    if(res1.isMain!=1){
                        offset1 += (8+36);
                    }

                    lw(tmp.one, -offset1, "$fp");

                    // read next
                    getMid();
                }else{
                    // 在全局表中查到了ID
                    // res2*4 是相对于gp的offset
                    int offset2 = 4 * res2 + 4;
                    lw(tmp.one, -offset2, "$gp");
                    // read next
                    getMid();
                }

            }else if(tmp.three=="[]"){
                // $ti ID [] $tj

                // 首先正常地查找ID
                int flag = 1;
                int res2;
                searchResult res1 = searchStackID(currentFuncName, tmp.two);
                if(res1.index==-1){
                    flag = 2;
                    res2 = searchGlobalID(tmp.two);
                }
                if(res2==-1){
                    printf("Failed to find the ID:%s\n", tmp.two.c_str());
                    return ;
                }

                // after searching
                if(flag==1){
                    // 在符号栈表中找到了ID
                    // 0 1 2 3 ...

                    addi("$s1", "$zero", 4);
                    addi("$s2", "$zero", res1.index);
                    mul("$s2", "$s1", "$s2");

                    // if(res1.isMain!=1){offset1 += 8;}
                    if(res1.isMain!=1)
                    addi("$s2", "$s2", (8+36));

                    // 以上只取出了ID，还有index
                    addi("$s1", "$zero", 4);
                    mul("$s1", "$s1", tmp.four);

                    // 修改过后
                    sub("$s1", "$fp", "$s1");
                    sub("$s1", "$s1", "$s2");

                    // 最终取出
                    lw(tmp.one, 0, "$s1");

                    // read next
                    getMid();
                }else{
                    // 在全局表中查到了ID
                    // res2*4 是相对于gp的offset
                    int offset2 = 4 * res2 + 4;
                    add("$s1", "$gp", "$zero");
                    addi("$s1", "$s1", -offset2);

                    addi("$s2", "$zero", 4);
                    mul("$s2", "$s2", tmp.four);

                    sub("$s1", "$s1", "$s2");

                    lw(tmp.one, 0, "$s1");

                    // read next
                    getMid();
                }
            }else if(tmp.two=="RET"){
                // $t1  RET
                add(tmp.one, "$v0", "$zero");
                getMid();

                global_v0 = -1;

            }else{
                // 尚未考虑到的情况
                printf(">>> ERROR: what the fuck is this mid code?\n");
                getMid();
                error();
            }
        }else if(tmp.one[0]=='$' && tmp.one[1]=='s'){
            // $s1 $ti 赋值操作
            add(tmp.one, tmp.two, "$zero");
            getMid();

        }else{
            // ID      $ti
            if(tmp.two[0]=='$' && tmp.two[1]=='t'){
                int flag = 1;
                int res2;
                searchResult res1 = searchStackID(currentFuncName, tmp.one);
                if(res1.index==-1){
                    flag = 2;
                    res2 = searchGlobalID(tmp.one);
                }
                if(res2==-1){
                    printf("Failed to find the ID:%s\n", tmp.one.c_str());
                    return ;
                }

                // 查找成功
                if(flag==1){
                    // 在符号栈表中找到了ID
                    // 0 1 2 3 ...
                    int offset1 = 4 * res1.index;
                    if(res1.isMain==0){
                        offset1 += (8+36);
                    }

                    sw(tmp.two, -offset1, "$fp");
                    // read next
                    getMid();
                }else{
                    // 在全局表中查到了ID
                    // res2*4 是相对于gp的offset
                    int offset2 = 4 * res2 + 4;
                    sw(tmp.two, -offset2, "$gp");
                    // read next
                    getMid();
                }

            }else if(tmp.two=="[]"){
            // ID   []  index   $ti

                // 首先正常地查找ID
                int flag = 1;
                int res2;
                searchResult res1 = searchStackID(currentFuncName, tmp.one);
                if(res1.index==-1){
                    flag = 2;
                    res2 = searchGlobalID(tmp.one);
                }
                if(res2==-1){
                    printf("Failed to find the ID:%s\n", tmp.one.c_str());
                    return ;
                }

                // after searching
                if(flag==1){
                    // 在符号栈表中找到了ID
                    // 0 1 2 3 ...

                    addi("$s1", "$zero", 4);
                    addi("$s2", "$zero", res1.index);
                    mul("$s2", "$s1", "$s2");

                    if(res1.isMain!=1)
                    addi("$s2", "$s2", (8+36));

                    // 以上只取出了ID，还有index
                    addi("$s1", "$zero", 4);
                    mul("$s1", "$s1", tmp.three);

                    sub("$s1", "$fp", "$s1");
                    sub("$s1", "$s1", "$s2");

                    // 最终取出
                    sw(tmp.four, 0, "$s1");

                    // read next
                    getMid();

                }else{
                    // 在全局表中查到了ID
                    // res2*4 是相对于gp的offset
                    int offset2 = 4 * res2 + 4;
                    add("$s1", "$gp", "$zero");
                    addi("$s1", "$s1", -offset2);

                    addi("$s2", "$zero", 4);
                    mul("$s2", "$s2", tmp.three);

                    sub("$s1", "$s1", "$s2");

                    // save in
                    sw(tmp.four, 0, "$s1");

                    // read next
                    getMid();
                }

            }else{
                getMid();
                error();
                return ;
            }
        }
    }


}

searchResult searchStackID(string targetFuncName, string targetID){
    // 在运行栈中查找符号名
    int i, j, length = funcStack.size();
    searchResult ures = {-1,-1,};
    if(length==0){
        // 还没有建函数信息表（栈）
        printf("have not set up function stack yet.\n");
        return ures;
    }


    // 找出自己的函数体的符号表
    functionInfo tmpinfo;
    for(i=0;i<length;i++){
        tmpinfo = funcStack.at(i);
        if(tmpinfo.funcName==targetFuncName)
            break;
    }

    int symSize = tmpinfo.funcSymbolTable.size();
    for(j=0;j<symSize;j++){
        funcRecordItem tmp = tmpinfo.funcSymbolTable.at(j);
        // check window

        if(tmp.ID==targetID){
            searchResult res = {
                tmp.offset,
                tmpinfo.isMain,
            };

            return res;
        }
    }

    return ures;
}

int searchGlobalID(string ID){
    // 查全局表
    int i, length = globalRecordVector.size();
    for(i=0;i<length;i++){
        globalRecordItem tmp = globalRecordVector.at(i);
        if(tmp.ID == ID){

            return tmp.offset;
        }
    }

    // 没有查到
    return -1;
}

int searchIDType(string func, string ID){
    int i, j, length = funcStack.size();

    // 找出自己的函数体的符号表
    functionInfo tmpinfo;
    for(i=0;i<length;i++){
        tmpinfo = funcStack.at(i);
        if(tmpinfo.funcName==func){
            break;
        }

    }


    int symSize = tmpinfo.funcSymbolTable.size();
    for(j=0;j<symSize;j++){
        funcRecordItem ftmp = tmpinfo.funcSymbolTable.at(j);

        if(ftmp.ID==ID){
            if(ftmp.type=="int" || ftmp.type=="1")
                return 1;
            else if(ftmp.type=="char" || ftmp.type=="2")
                return 2;
            else
                return -1;
        }
    }

    // 再在全局中查找
    length = globalRecordVector.size();
    for(i=0;i<length;i++){
        globalRecordItem ftmp = globalRecordVector.at(i);

        if(ftmp.ID == ID){
            if(ftmp.type=="int")
                return 1;
            else if(ftmp.type=="char")
                return 2;
            else
                return -1;
        }
    }

    printf("Can not find this ID's type.\n");
    return -1;
}

void nextLine(){
    char nextLineSymC = '\n';
    int nextLineSymI = nextLineSymC;

    if(global_v0 != 11){
        addi("$v0", "$zero", 11);
        global_v0 = 11;
    }

    addi("$a0", "$zero", nextLineSymI);

    global_v0 = -1;
    syscall();
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
        "addu",
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

void jal(string label){
    mipsItem tmp = {
        "jal",
        label,
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

void mipsLabel(string label){

    vector<string> v;
    SplitString(label, v,"_");
    int i, length = v.size();
    string newLbael = v.at(0);
    for(i=1;i<length-1;i++){
        newLbael = newLbael + "_" + v.at(i);
    }

    mipsItem tmp = {
        newLbael + ":",
        "",
        "",
        "",
    };
    mipsCodeVector.push_back(tmp);
}

void mipsEndLabel(){
    mipsItem tmp = {
        "THIS_IS_THE_END_OF_ALL:",
        "",
        "",
        "",
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
    //printf("\nContents of MIPS codes\n");
    //printf(".data\n");
    //printf(".text\n");
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

void Mips2File(){
    int i, cntMips = mipsCodeVector.size();
    ofstream ofile;
    ofile.open("MipsCode.txt");

    for(i=0;i<cntMips;i++){
        mipsItem tmp = mipsCodeVector.at(i);
        ofile<<setw(20)<<tmp.one.c_str()<<"\t"<<setw(20)<<tmp.two.c_str()<<"\t"<<setw(20)<<tmp.three.c_str()<<"\t"<<setw(20)<<tmp.four.c_str()<<endl;
    }
    ofile.close();
}

void Mips2File_optimize(){
    int i, cntMips = mipsCodeVector_Optimize.size();
    ofstream ofile;
    ofile.open("MipsCode_Optimized.txt");

    for(i=0;i<cntMips;i++){
        mipsItem tmp = mipsCodeVector_Optimize.at(i);
        ofile<<setw(20)<<tmp.one.c_str()<<"\t"<<setw(20)<<tmp.two.c_str()<<"\t"<<setw(20)<<tmp.three.c_str()<<"\t"<<setw(20)<<tmp.four.c_str()<<endl;
    }
    ofile.close();
}

void printGlobalRecord(){
    printf("\nContents of global Record\n");
    int i, cntMips = globalRecordVector.size();
    for(i=0;i<cntMips;i++){
        globalRecordItem tmp = globalRecordVector.at(i);
        printf("%d\t%s\t%d\n", i, tmp.ID.c_str(), tmp.offset);
    }
}

void SplitString(const string& s, vector<string>& v, const string& c){
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

void optimize_sp(){
    mipsCodeVector_tmp.clear();
    int i, cntMips = mipsCodeVector_Optimize.size(), optimize_count = 0;
    for(i=0;i<cntMips;i++){
        mipsItem tmp = mipsCodeVector_Optimize.at(i);
        if(tmp.one=="addi" && tmp.two=="$sp" && tmp.three=="$sp" && i>0){
            mipsItem tmp2 = mipsCodeVector_Optimize.at(i-1);
            if(tmp2.one=="addi" && tmp2.two=="$sp" && tmp2.three=="$sp"){
                // 合并两条mips指令
                // 取出上一条
                mipsCodeVector_tmp.pop_back();
                // 处理当前两条
                int tmp_int_1 = transNum(tmp2.four).value;
                int tmp_int_2 = transNum(tmp.four).value;
                string new_four = to_string(tmp_int_1 + tmp_int_2);
                // push
                mipsCodeVector_tmp.push_back(tmp);
                // modify
                mipsCodeVector_tmp.back().four = new_four;
                optimize_count++;

                continue;
            }
        }

        // else
        mipsCodeVector_tmp.push_back(tmp);
    }

    // 赋值
    mipsCodeVector_Optimize = mipsCodeVector_tmp;
    printf(">>> optimize_sp reduce %d mips code instructions.\n", optimize_count);
}








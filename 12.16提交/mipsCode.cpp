#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <iomanip>

#include "mipsCode.h"
#include "error.h"

using namespace std;

int midCodeIndex = 0;
int funcLevel = 0;      // Ĭ��mainΪ1��globalΪ0
midCodeItem tmp;
vector<mipsItem> mipsCodeVector;
vector<globalRecordItem> globalRecordVector;
vector<functionInfo> allFuncInfoVector;
vector<functionInfo> funcStack;
/*  allFuncInfoVector Ϊ�洢������Ϣ��vector�����Һ���
    funcStack Ϊ����ջ����¼��Ӧ��Ϣ�����ұ���
*/
int globalRecordCount = 0;
int globalValueOfFp = 0;
int compareResult;      // ��ϵ������Ԫʽ�Ľ������Ϊ��ת������
                        // ���˽������Ĵ���$s2
int paraCount;          // ���ڼ�¼�������õĲ�������
string currentFuncName;

int offsetGp = 0;      // ȫ�ֱ�����������ƫ��������ΪҪ���ʣ�����$gp��ò�Ҫ��
int offsetFp = 0;
int offsetSp = 0;

void printMidCodeTmp(midCodeItem tmp){
    printf("%s\t", tmp.one.c_str());
    printf("%s\t", tmp.two.c_str());
    printf("%s\t", tmp.three.c_str());
    printf("%s\n", tmp.four.c_str());
}

void getMid(){
    if(midCodeIndex>=midCodeVec.size()){
        printf(">>> out of midCodeVec's length\n");
        return ;
    }
    tmp = midCodeVec.at(midCodeIndex++);
    printf("> %d\t", midCodeIndex);
    printMidCodeTmp(tmp);
}

void pushGlobalRecord(string ID, int offset){
    globalRecordItem tmp={
        ID,         // ������
        offset,     // �����$gp��ƫ����
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

    // ����sp
    // add("$sp", "$zero", "$zero");

    // ����ȫ�� ��������
    if(tmp.one=="const"){
        while(true){
            // ��¼������Ϣ
            pushGlobalRecord(tmp.three, globalRecordCount++);   // ��һ��countΪ0
            // ����mips
            offsetGp -= 4;
            // ������Ҫ�ж�һ��int �� char
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

    // ����ȫ�ֱ�����������Ϣ������MIPS
    while(tmp.one!="func" && tmp.one!="main" && tmp.one!="label"){
        // ��������
        if(tmp.one=="var"){
            // ��¼������Ϣ
            pushGlobalRecord(tmp.three, globalRecordCount++);
            // ����mips
            // addi("$gp", "$gp", -4);
            offsetGp -= 4;
        }else if(tmp.one=="array"){
            // ��¼������Ϣ

            pushGlobalRecord(tmp.three, globalRecordCount);
            globalRecordCount += transNum(tmp.four);
            // ����Ĵ洢��ʽ
            // head + offset

            // ����mips
            li("$t1", to_string(4));
            li("$t2", tmp.four);
            mul("$t3", "$t1", "$t2");

            offsetGp -= 4 * transNum(tmp.four);
        }else{
            printf(">>> ERROR: in dealing with global var and array.\n");
        }
        getMid();
    }

    // ��ת��������
    j("main");


    // ���������Ӻ�������
    while(tmp.one=="label" && tmp.three=="func"){
        // ����mips����¼������Ϣ������������ջ
        mipsLabel(tmp.two);
        getMid();
        if(tmp.one=="func"){
            // ��¼������
            currentFuncName = tmp.three;

            // ���������ɺ���ͷ��������fp sp������ra
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

         // ��¼������ʱ����
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

        // ��������

        // �ȼ����ж��ٸ�para
        paraCount = 0;
        int tmpParaIndex = midCodeIndex - 1;
        midCodeItem tmpParaItem;
        // tmp = midCodeVec.at(midCodeIndex++);
        while(true){
            tmpParaItem = midCodeVec.at(tmpParaIndex);
            if(tmpParaItem.one != "para")
                break;

            paraCount++;
            tmpParaIndex++;
        }


        while(tmp.one=="para"){
            // �ں�����Ϣ���еǼǲ�����Ϣ
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 1, ""};
            funcSymbolTable.push_back(tmp2);

            // ��ջ�м��ز�����ֵ paracount
            printf(">>> check para count: %d\n", paraCount);
            // ����麯�������֪���м�������
            int gap = 4 * paraCount + 8 + 36;
            addi("$s1", "$sp", gap);
            lw("$t1", 0, "$s1");
            sw("$t1", 0, "$sp");
            addi("$sp", "$sp", -4);


//            add("$t1", "$zero", "$ra");
//            addi("$t1", "$t1", (4+4*paraCount));
//            paraCount -= 1; // ��Ϊ0ǡ����ֹ
//            lw("$t2", 0, "$t1");
//            addi("$sp", "$sp", 4);
//            sw("$t2", 0, "$sp");

            // Ԥ����һ��
            getMid();
        }
        // ����������������
        paraCount = 0;

        // �������岿��
        while(tmp.one=="const"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, tmp.four};
            funcSymbolTable.push_back(tmp2);
            // �����ռ䲢���볣����ֵ
            //addi("$sp", "$sp", 4);
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

        // ���������鶨�岿��
        while(tmp.one=="var" || tmp.one=="array"){
            if(tmp.one=="var"){
                printf(">>> check value of VAR count:%d\n", funcSymbolCount);
                funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, ""};
                funcSymbolTable.push_back(tmp2);
                //
                addi("$sp", "$sp", -4);
                getMid();
            }else{
                printf(">>> check value of ARRAY count:%d\n", funcSymbolCount);
                funcRecordItem tmp2 = {tmp.three, funcSymbolCount, 0, ""};
                funcSymbolCount += transNum(tmp.four);
                funcSymbolTable.push_back(tmp2);
                //
                addi("$sp", "$sp", -4*transNum(tmp.four));
                getMid();
            }
        }


        //printf(">>> check funcSymbolCount = %d\n", funcSymbolCount);
        functionInfo tmpfunc = {currentFuncName,    // name / ID
                                funcLevel,          // level
//                                globalValueOfFp,
//                                globalValueOfFp + 8 + 4 * funcSymbolCount + 36,
//                                8 + 4 * funcSymbolCount + 36,    // length
                                0,                  // not main
                                funcSymbolTable};   // symbol table
//        globalValueOfFp += (8 + 4 * funcSymbolCount + 36);
        //printf(">>> check globalValueOfFp = %d\n", globalValueOfFp);
        allFuncInfoVector.push_back(tmpfunc);
        funcStack.push_back(tmpfunc);

        //printf(">>> CHECK2 globalValueOfFp = %d\n", globalValueOfFp);
        // ������䲿��
        while(true){
            if(tmp.one=="label"){
            //    printMidCodeTmp(tmp);
                if(tmp.three=="func"){
                    break;
                }else{
                    // ������ͨ��label
                    label("label" + tmp.two);
                    getMid();
                }
            }
            // �������handleһ�������mips
            if(tmp.one!="label")
                handleMidCode();
            //    printf(">>> CHECK3 globalValueOfFp = %d\n", globalValueOfFp);
            //getMid();
        }

        // �ָ����оֲ�����
        lw("$t1",  -8, "$fp");
        lw("$t2", -12, "$fp");
        lw("$t3", -16, "$fp");
        lw("$t4", -20, "$fp");
        lw("$t5", -24, "$fp");
        lw("$t6", -28, "$fp");
        lw("$t7", -32, "$fp");
        lw("$t8", -36, "$fp");
        lw("$t9", -40, "$fp");

        // ��ǰΪlabel_func_2,ȡ��ra������һ��jr
        lw("$ra", -4, "$fp");

        // ��ʱ�����ַ�ʽ�ָ�sp
        //addi("$sp", "$fp", -4);
        add("$sp", "$zero", "$fp");

        // ��Ҫ�ָ�fp
        lw("$fp", 0, "$fp");

        // ��sp�ƶ������һ���ֲ���������һ��λ��
        // funcSymbolCount  globalValueOfFp
//        printf("setting sp as : %d\n", globalValueOfFp);
//        addi("$sp", "$zero", globalValueOfFp);
//        addi("$sp", "$fp", 8+4*funcSymbolCount);




        jr();

        // ɾ������ջ�иú����Ĳ���
        // funcStack.pop_back();

        getMid();


    }
    // ���������к�������
    // ����������
    if(tmp.one=="main"){
//        printf("> about to get in handle main.\n");
        handleMain();
    }
}

// ��������������������ջ������MIPS
void handleMain(){
    //
    mipsLabel("main");
    getMid();

    // ����ֱ����������� func stack
//    printf(">>> ABOUT TO WIPE FUNCSTACK\n");
//    while(funcStack.size() > 0){
//        funcStack.pop_back();
//    }
//    globalValueOfFp = 0;
//    printf(">>> wiped up the function stack, size:%d\n", funcStack.size());

    // ��Ҫ��sp��fp���õ�λ
//    addi("$fp", "$zero", globalValueOfFp);
//    add("$sp", "$fp", "$zero");
    add("$fp", "$sp", "$zero"); // ����Ҫ��sp


    // �����ﴦ�����г����������鶨��
    int funcSymbolCount = 0;
    vector<funcRecordItem> mainSymbolTable;

    // ��������
    while(tmp.one=="const"){
        funcRecordItem tmpm = {tmp.three, funcSymbolCount++, 0, tmp.four};
        mainSymbolTable.push_back(tmpm);
        // �����ռ䲢���볣����ֵ

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

    // ��������
    // ���������鶨�岿��
    while(tmp.one=="var" || tmp.one=="array"){
        if(tmp.one=="var"){
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount++, 0, ""};
            mainSymbolTable.push_back(tmp2);
            //
            addi("$sp", "$sp", -4);
            getMid();
        }else{
            funcRecordItem tmp2 = {tmp.three, funcSymbolCount, 0, ""};
            funcSymbolCount += transNum(tmp.four);
            mainSymbolTable.push_back(tmp2);
            //
            addi("$sp", "$sp", -4 * transNum(tmp.four));
            getMid();
        }
    }

    // function info func stack
    functionInfo tmpfunc = {"main",
                            1,                  // level
//                            globalValueOfFp,
//                            globalValueOfFp + 4 * funcSymbolCount,
//                            4 * funcSymbolCount,// length
                            1,                  // is main
                            mainSymbolTable};   // symbol table
//    globalValueOfFp += (4 * funcSymbolCount);
    funcStack.push_back(tmpfunc);
    allFuncInfoVector.push_back(tmpfunc);

    currentFuncName = "main";


//    printf("> about to get in handle mid code.\n");
//    handleMidCode();
    while(true){
        handleMidCode();
        if(midCodeIndex >= midCodeVec.size())
            break;
    }
    handleMidCode();
}

// ����������䣬����һ������Ԫʽ�����ɴ󲿷�MIPS
void handleMidCode(){
    //printf(">>> in Handle-MidCode\n");
//    printf(">>> check tmp one:%s\n", tmp.one.c_str());
    // �Ѿ�Ԥ����һ��MidCode
    if(tmp.one=="BZ"){
        // BZ����һ��һ�������������Զ���$ti<$tj����ʽ����ֵ������
        // ֱ�Ӵӱ�����ȡֵ�ж�
        // �����֧Ӧ���ǽ������ģ��������ȱ�һ�����
        printf("Unexpected branch 'BZ' \n");
        getMid();
    }else if(tmp.one=="GOTO"){
        j("label" + tmp.three);
        getMid();
    }else if(tmp.one=="label"){
        label("label" + tmp.two);
        getMid();
    }else if(tmp.one=="ret"){
        // ret        $t24
        // ret
        if(tmp.two==""){
            // ����ֵΪ�գ���������
        }else{
            // ����ֵ����Ĵ���
            add("$v0", "$zero", tmp.two);

        }

        // �ָ����оֲ�����
        lw("$t1",  -8, "$fp");
        lw("$t2", -12, "$fp");
        lw("$t3", -16, "$fp");
        lw("$t4", -20, "$fp");
        lw("$t5", -24, "$fp");
        lw("$t6", -28, "$fp");
        lw("$t7", -32, "$fp");
        lw("$t8", -36, "$fp");
        lw("$t9", -40, "$fp");

        // ����Ҫ�ɺܶ���
        lw("$ra", -4, "$fp");

        // ��ʱ�����ַ�ʽ�ָ�sp
        add("$sp", "$zero", "$fp");

        // ��Ҫ�ָ�fp
        lw("$fp", 0, "$fp");

        jr();
        getMid();

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
        // get next
        getMid();
    }else if(tmp.one=="scan"){
        // v0 = 5
        addi("$v0", "$zero", 5);
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
            //int fp = res1.targetFp;
            int offset1 = 4 * res1.index;

            if(res1.isMain!=1){
                offset1 += (8+36);
            }

//            addi("$s1", "$zero", fp);
//            add("$s1", "$zero", "$fp");
//            sw("$v0", offset1, "$s1");
            sw("$v0", -offset1, "$fp");

            // read next
            getMid();
        }else{
            // find ID in global table
            int offset2 = 4 * res2;
            sw("$v0", -offset2, "$gp");
            // read next
            getMid();
        }

    }else if(tmp.one=="para"){
        // Ӧ�ý����������֧
        printf("Unexpected 'parameter' branch.\n");
        getMid();
    }else if(tmp.one=="push"){

        sw(tmp.two, 0, "$sp");
        addi("$sp", "$sp", -4);

        getMid();

    }else if(tmp.one=="call"){
        printf(">>> check in branch 'call' func \n");


        // ����ֻ����jal�����ز������ڶ�������ʱ��
        jal(tmp.two);

        // ��ʱ���������������µ� func stack
//        int i, length = funcStack.size();
//        functionInfo tmpc;
//        for(i=0;i<length;i++){
//            tmpc = funcStack.at(i);
//            if(tmpc.funcName == tmp.two)
//                break;
//        }

        //functionInfo newTmp = funcStack.at(i);
//        functionInfo newTmp = tmpc;
//        functionInfo oldTmp = funcStack.back();
//        // ��fp����ΪoldTmp.sp
//        globalValueOfFp = oldTmp.sp;
//
//        // ��fp sp���޸ĺ�ĺ�������ѹ��ջ����ʵ��������Ҫ���ĳ���
//        newTmp.fp = globalValueOfFp;
//        newTmp.sp = globalValueOfFp + newTmp.length;
//        funcStack.push_back(newTmp);
//        printf(">>> push new func in stack :%s, len:%d\n", newTmp.funcName.c_str(), funcStack.size());

        getMid();

    }else{
        if(tmp.one[0]=='$' && tmp.one[1]=='t'){
            //printf(">>> in branch tmp.one = '$ti' \n");
            // "$ti" һ����si�ֿ���
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
                BEQZ����ת��ָ����Ĵ���������Ϊ0ʱת�Ʒ���BEQZ R1,0
                BENZ����ת��ָ����Ĵ��������ݲ�Ϊ0ʱת�Ʒ���BNEZ R1,0
                BEQ����ת��ָ��������Ĵ����������ʱת�Ʒ���BEQ R1,R2
                BNE����ת��ָ��������Ĵ��������ݲ���ʱת�Ʒ���BNE R1,R2
            */
                if(tmp.two=="<"){
                    sub("$s1", tmp.three, tmp.one); // s1 = 3 - 1
                    getMid();  // һ���� BZ
                    // s1<=0����ת
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
                    // s1<0����ת
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

                }else{
                    printf("Unknown compare opt in '$ compare $' mid code.\n");
                    error();
                    return ;
                }
            }
            // $t22           1
            else if(transNum(tmp.two)!=-1){
                printf(">>> check! in branch ; $ti   integer; \n");

                int value1 = transNum(tmp.two);
                // ֱ�Ӹ�ֵ
                addi(tmp.one, "$zero", value1);
                getMid();
            }
            // $t1           a
            else if(tmp.three=="" && tmp.two!="RET"){
            //    printf(">>> check: in this branch: '$ti  a'\n");
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

                //printf(">>> check it out! flag=%d\n", flag);
                if(flag==1){
                    // �ڷ���ջ�����ҵ���ID
                    // 0 1 2 3 ...
                    //int fp = res1.targetFp;
                    int offset1 = 4 * res1.index;
                    if(res1.isMain!=1){
                        offset1 += (8+36);
                    }
                    printf(">>> check: func stack search: index=%d\n", res1.index);
//                    add("$s1", "$zero", "$fp");
//                    lw(tmp.one, offset1, "$s1");
                    lw(tmp.one, -offset1, "$fp");

                    // read next
                    getMid();
                }else{
                    // ��ȫ�ֱ��в鵽��ID
                    // res2*4 �������gp��offset
                    int offset2 = 4 * res2;
                    lw(tmp.one, -offset2, "$gp");
                    // read next
                    getMid();
                }

            }else if(tmp.three=="[]"){
                // $ti ID [] $tj

                // ���������ز���ID
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
                    // �ڷ���ջ�����ҵ���ID
                    // 0 1 2 3 ...
                    //int fp = res1.targetFp;
                    printf(">>> check: func stack search: index=%d\n", res1.index);

                    // offset1 = 4 * res1.index;
                    addi("$s1", "$zero", 4);
                    addi("$s2", "$zero", res1.index);
                    mul("$s2", "$s1", "$s2");

                    // if(res1.isMain!=1){offset1 += 8;}
                    if(res1.isMain!=1)
                    addi("$s2", "$s1", (8+36));

                    // ����ֻȡ����ID������index
                    addi("$s1", "$zero", 4);
                    mul("$s1", "$s1", tmp.four);

//                    add("$s1", "$fp", "$s1");
//                    add("$s1", "$s1", "$s2");
                    // �޸Ĺ���
                    sub("$s1", "$fp", "$s1");
                    sub("$s1", "$s1", "$s2");

                    // ����ȡ��
                    lw(tmp.one, 0, "$s1");

                    // read next
                    getMid();
                }else{
                    // ��ȫ�ֱ��в鵽��ID
                    // res2*4 �������gp��offset
                    int offset2 = 4 * res2;
                    add("$s1", "$gp", "$zero");
                    addi("$s1", "$s1", -offset2);

                    addi("$s2", "$zero", 4);
                    mul("$s2", "$s2", tmp.four);

//                    add("$s1", "$s1", "$s2");
                    sub("$s1", "$s1", "$s2");

                    lw(tmp.one, 0, "$s1");

                    // read next
                    getMid();
                }
            }else if(tmp.two=="RET"){
                // $t1  RET
                add(tmp.one, "$v0", "$zero");
                getMid();

            }else{
                // ��δ���ǵ������
                printf(">>> ERROR: what the fuck is this mid code?\n");
                getMid();
                error();
            }
        }else{
            printf(">>> check in branch: 'ID $ti' \n");
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

                // ���ҳɹ�
                if(flag==1){
                    // �ڷ���ջ�����ҵ���ID
                    // 0 1 2 3 ...
                    //int fp = res1.targetFp;
                    //int fp = res1.targetFp;
                    int offset1 = 4 * res1.index;
                    if(res1.isMain==0){
                        offset1 += (8+36);
                    }
                    //addi("$s1", "$zero", fp);
                    sw(tmp.two, -offset1, "$fp");
                    // read next
                    getMid();
                }else{
                    // ��ȫ�ֱ��в鵽��ID
                    // res2*4 �������gp��offset
                    int offset2 = 4 * res2;
                    sw(tmp.two, -offset2, "$gp");
                    // read next
                    getMid();
                }

            }else if(tmp.two=="[]"){
            // ID   []  index   $ti
                printf(">>> checkpoint! in the branch ;ID   []  index   $ti;\n");

                // ���������ز���ID
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
                    // �ڷ���ջ�����ҵ���ID
                    // 0 1 2 3 ...
                    //int fp = res1.targetFp;
                    printf(">>> check: func stack search: index=%d\n", res1.index);

                    // offset1 = 4 * res1.index;
                    addi("$s1", "$zero", 4);
                    addi("$s2", "$zero", res1.index);
                    mul("$s2", "$s1", "$s2");

                    // if(res1.isMain!=1){offset1 += 8;}
                    if(res1.isMain!=1)
                    addi("$s2", "$s1", (8+36));

                    // ����ֻȡ����ID������index
                    addi("$s1", "$zero", 4);
                    mul("$s1", "$s1", tmp.three);

//                    add("$s1", "$fp", "$s1");
//                    add("$s1", "$s1", "$s2");
                    sub("$s1", "$fp", "$s1");
                    sub("$s1", "$s1", "$s2");

                    // ����ȡ��
                    sw(tmp.four, 0, "$s1");

                    // read next
                    getMid();

                }else{
                    // ��ȫ�ֱ��в鵽��ID
                    // res2*4 �������gp��offset
                    int offset2 = 4 * res2;
                    add("$s1", "$gp", "$zero");
                    addi("$s1", "$s1", -offset2);

                    addi("$s2", "$zero", 4);
                    mul("$s2", "$s2", tmp.three);

//                    add("$s1", "$s1", "$s2");
                    sub("$s1", "$s1", "$s2");

                    // save in
                    sw(tmp.four, 0, "$s1");

                    // read next
                    getMid();
                }


//                getMid();
            }else{
                getMid();
                error();
                return ;
            }
        }
    }


}

//int searchFuncSymbol(string TargetID){
//    // allFuncInfoVector
//    int i, j, length = allFuncInfoVector.size();
//    for(i=length-1;i>=0;i--){
//        functionInfo tmpf = allFuncInfoVector.at(i);
//        for(j=0;j<tmpf.funcSymbolTable.size();j++){
//            // to be continued
//        }
//    }
//}

searchResult searchStackID(string targetFuncName, string targetID){
    // ������ջ�в��ҷ�����
    int i, j, length = funcStack.size();
    searchResult ures = {-1,-1,};
    if(length==0){
        // ��û�н�������Ϣ��ջ��
        printf("have not set up function stack yet.\n");
        return ures;
    }


    // �ҳ��Լ��ĺ�����ķ��ű�
    functionInfo tmpinfo;
    for(i=0;i<length;i++){
        tmpinfo = funcStack.at(i);
        if(tmpinfo.funcName==targetFuncName)
            break;
    }
//    functionInfo info = funcStack.at(length-1);

    int symSize = tmpinfo.funcSymbolTable.size();
    for(j=0;j<symSize;j++){
        funcRecordItem tmp = tmpinfo.funcSymbolTable.at(j);
        // check window
    //    printf(">>> targetID:%s\tID:%s\toffset:%d\tvalue:%s\n", targetID.c_str(), tmp.ID.c_str(), tmp.offset, tmp.value.c_str());

        if(tmp.ID==targetID){
        //    return tmp.offset;
            searchResult res = {
                tmp.offset,
                tmpinfo.isMain,
            };

            return res;
        }
    }

//    // �ٲ�Ƕ�׵���
//    for(i=length-1;i>=0;i--){
//        functionInfo info1 = funcStack.at(i);
//    //    if(funcLevel > info1.level){
//            // ���߼�����
//            for(j=0;j<info1.funcSymbolTable.size();j++){
//                funcRecordItem tmp = info1.funcSymbolTable.at(j);
//                if(tmp.ID==targetID){
//                    searchResult res = {
//                        info1.fp,
//                        tmp.offset,
//                        info1.isMain,
//                    };
//                    printf(">>> find ID in stack, fp = %d, index = %d\n", res.targetFp, res.index);
//                    return res;
//                }
//
//            }
//    //    }
//    }
//    // û�в鵽

    printf(">>> NOT find ID in stack\n");
    return ures;
}

int searchGlobalID(string ID){
    // ��ȫ�ֱ�
    int i, length = globalRecordVector.size();
    for(i=0;i<length;i++){
        globalRecordItem tmp = globalRecordVector.at(i);
        if(tmp.ID == ID)
            printf("find ID in global, index = %d\n", tmp.offset);
            return tmp.offset;
    }
    // û�в鵽
    printf("NOT find ID in global\n");
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
    //int i, length = label.length();
//    char * newLabel = new char[label.length() + 1] ; //��Ҫ����
//    strcpy(newLabel, label.c_str());
//    char *parts;
//	const char *delim = "_";
//	parts = strtok(newLabel, delim);
//	string theLabel = parts;
    //for(i=0;i<length-2;i++)
    //    newLabel[i] = label[i];
    //newLabel[i-1] = '\0';

    vector<string> v;
    SplitString(label, v,"_");
    string newLbael = v.at(0);
    mipsItem tmp = {
        newLbael + ":",
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

void Mips2File(){
    int i, cntMips = mipsCodeVector.size();
    ofstream ofile;
    ofile.open("MipsCode.txt");

    ofile<<"Contents of MIPS"<<endl;
    ofile<<"\n"<<endl;
    for(i=0;i<cntMips;i++){
        mipsItem tmp = mipsCodeVector.at(i);
        ofile<<setw(12)<<tmp.one.c_str()<<setw(12)<<tmp.two.c_str()<<setw(12)<<tmp.three.c_str()<<setw(12)<<tmp.four.c_str()<<endl;
    }
    ofile.close();
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

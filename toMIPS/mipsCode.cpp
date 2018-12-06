#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <stdlib.h>

#include "mipsCode.h"
#include "error.h"


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
    tmp = midCodeVec.at(midCodeIndex++);
//    printf("> %d\t", midCodeIndex);
//    printMidCodeTmp(tmp);
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

    // ����ȫ�� ��������
    if(tmp.one=="const"){
        while(true){
            // ��¼������Ϣ
            pushGlobalRecord(tmp.three, ++globalRecordCount);   // ��һ��countΪ-1
            // ����mips
            // addi("$gp", "$gp", -4);
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
            pushGlobalRecord(tmp.three, ++globalRecordCount);
            // ����mips
            // addi("$gp", "$gp", -4);
            offsetGp -= 4;
        }else if(tmp.one=="array"){
            // ��¼������Ϣ
            globalRecordCount += transNum(tmp.four);
            pushGlobalRecord(tmp.three, globalRecordCount);
            // ����mips
            li("$t1", to_string(4));
            li("$t2", tmp.four);
            mul("$t3", "$t1", "$t2");
            //sub("$gp", "$gp", "$t3"); // ���ܸı�$gp
            offsetGp -= 4 * transNum(tmp.four);
        }else{
            printf(">>> ERROR: in dealing with global var and array.\n");
        }
        getMid();
    }

    // ���������Ӻ�������
    while(tmp.one=="label" && tmp.three=="func"){
        // ����mips����¼������Ϣ������������ջ
        getMid();
        if(tmp.one=="func"){
            // ���������ɺ���ͷ��������fp sp������ra
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
        // ��������
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
        while(true){
            if(tmp.one=="label"){
            //    printMidCodeTmp(tmp);
                if(tmp.three=="func"){
                    break;
                }
            }
            // �������handleһ�������mips
            handleMidCode();
            getMid();
        }
        // ��ǰΪlabel_func_2,����һ��jr
        jr();

        getMid();

        // ���ú���������Ϣpush��allFuncInfoVector
        functionInfo tmpfunc = {funcLevel,          // level
                                globalValueOfFp,
                                globalValueOfFp + 8 + 4 * funcSymbolCount,
                                funcSymbolCount,    // length
                                funcSymbolTable};   // symbol table
        globalValueOfFp += (8 + 4 * funcSymbolCount);
        allFuncInfoVector.push_back(tmpfunc);
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
    getMid();
//    printf("> about to get in handle mid code.\n");
    handleMidCode();
}

// ������������䣬����һ������Ԫʽ�����ɴ󲿷�MIPS
void handleMidCode(){
    // �Ѿ�Ԥ����һ��MidCode
    if(tmp.one=="BZ"){
        // BZ����һ��һ�������������Զ���$ti<$tj����ʽ����ֵ������
        // ֱ�Ӵӱ�����ȡֵ�ж�
        // �����֧Ӧ���ǽ������ģ��������ȱ�һ������
        printf("Unexpected branch 'BZ' \n");
        getMid();
    }else if(tmp.one=="GOTO"){
        j(tmp.two);
        getMid();
    }else if(tmp.one=="ret"){
        // ret        $t24
        // ret
        if(tmp.two==""){
            // ����ֵΪ�գ���������
            getMid();
        }else{
            // ����ֵ����Ĵ���
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
                    // s1<0����ת
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
                // ֱ�Ӹ�ֵ
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
                    // �ڷ���ջ�����ҵ���ID
                    // 0 1 2 3 ...
                    int fp = res1.targetFp;
                    int offset1 = 8 + 4 * res1.index;
                    addi("$s1", "$zero", fp);
                    lw(tmp.one, offset1, "$s1");
                    // read next
                    getMid();
                }else{
                    // ��ȫ�ֱ��в鵽��ID
                    // res2*4 �������gp��offset
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

                // ���ҳɹ�
                if(flag==1){
                    // �ڷ���ջ�����ҵ���ID
                    // 0 1 2 3 ...
                    int fp = res1.targetFp;
                    int offset1 = 8 + 4 * res1.index;
                    addi("$s1", "$zero", fp);
                    sw(tmp.two, offset1, "$s1");
                    // read next
                    getMid();
                }else{
                    // ��ȫ�ֱ��в鵽��ID
                    // res2*4 �������gp��offset
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
    // ������ջ�в��ҷ�����
    int i, j, length = funcStack.size();
    // �Ȳ��Լ�����
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

    // �ٲ�Ƕ�׵���
    for(i=length-1;i>=0;i--){
        functionInfo info1 = funcStack.at(i);
        if(funcLevel > info1.level){
            // ���߼�����
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
    // û�в鵽
    searchResult ures = {
        -1,
        -1,
    };
    return ures;
}

int searchGlobalID(string ID){
    // ��ȫ�ֱ�
    int i, length = globalRecordVector.size();
    for(i=0;i<length;i++){
        funcRecordItem tmp = globalRecordVector.at(i);
        if(tmp.ID == ID)
            return tmp.offset;
    }
    // û�в鵽
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
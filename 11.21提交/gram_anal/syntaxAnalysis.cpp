#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "error.cpp"
#include "getsym.cpp"
#include "syntaxAnalysis.h"

#define CONSTSY     1
#define INTSY       2
#define CHARSY      3
#define VOIDSY      4
#define MAINSY      5
#define IFSY        6
#define ELSESY      7
#define WHILESY     8
#define FORSY       9
#define SCANFSY     10
#define PRINTSY     11
#define RETSY       12

#define USINTSY     18
#define ACHARSY     19
#define IDSY        20
#define STRINGSY    21

#define PLUSSY      22
#define MINUSSY     23
#define STARSY      24
#define DIVISY      25
#define LPARSY      26
#define RPARSY      27
#define COMMASY     28
#define SEMISY      29
#define COLONSY     30
#define ASSIGNSY    31
#define EQUSY       32
#define LESSSY      33
#define LOESY       34
#define MORESY      35
#define MOESY       36
#define LOMSY       37
#define AEQUSY      38
#define DBQUOSY     39
#define LBRACSY     40
#define RBRACSY     41
#define LBRACESY    42
#define RBRACESY    43



int unsignedInt();
int signedInt();
int constDefine();
int constState();
int factor();
int item();
int expr();
int varDefine();
int varState();
int paraList();
int complexSentence();
int sentenceSequence();
int retValueFuncDefine();
int condSentence();
int condition();
int loopSentence();
int retValueFuncCall();
int unretValueFuncCall();
int assignSentence();
int scanSentence();
int printSentence();
int nullSentence();
int retSentence();
int programAnalysis();
int mainAnalysis();


int unsignedInt(){                  // �ķ��е��޷�������
    if(result==USINTSY){
        while(true){
            getsym();
            if(result==USINTSY){
                continue;
            }else   break;
        }
    }else{
        error();
        return -1;
    }

    printf("This is an unsigned integer.\n");
    return 0;
}

int signedInt(){                    // ��Ӧ�ķ��е�����
    if(result==PLUSSY||result==MINUSSY){
        getsym();
    }

    unsignedInt();

    printf("This is a signed integer.\n");
    return 0;
}

int constDefine(){
    if(result==INTSY){              // INTSY
        int tmp_count = 0;
                    while(true){
                        getsym();
                        if(result==IDSY){
                            getsym();
                            if(result==EQUSY){
                                getsym();
                                signedInt();
                                // ����signedInt�Ѿ�Ԥ������һ��symbol
                                if(result==COMMASY){
                                    tmp_count += 1;
                                    continue;
                                }else{
                                    break;
                                }
                            }else{
                                error();
                                return -1;
                            }
                        }else{
                            error();
                            return -1;
                        }
                    }

    }else if(result==CHARSY){       // CHARSY
        int tmp_count = 0;
                        while(true){
                            getsym();
                            if(result==IDSY){
                                getsym();
                                if(result==EQUSY){
                                    getsym();
                                    if(result==ACHARSY){
                                        getsym();
                                        if(result==COMMASY){
                                            tmp_count += 1;
                                            continue;
                                        }
                                        else    break;
                                    }else {
                                        error();
                                        return -1;
                                    }
                                }else{
                                    error();
                                    return -1;
                                }
                            }else{
                                error();
                                return -1;
                            }
                        }
    }else{
        error();
        return -1;
    }
    printf("This is a const definition.\n");
    return 0;
}

int constState(){
    int constStateCount = 0;
    if(result!=CONSTSY){
        error();
        return -1;
    }
    getsym();
    constDefine();
    if(result!=SEMISY){
        error();
        return -1;
    }
    getsym();
    constStateCount += 1;

    while(true){
    //    printf("in constState result = %d\n", result);
        if(result!=CONSTSY){
            // should continue for next syntax part
            break;
        }
        else{
            getsym();
            constDefine();          // ����������������getsym�������ٶ���һ��
        }

        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();

    }
//    printf("constStateCount = %d\n", constStateCount);
    printf("This is a const statement.\n");
    return 0;
}

int declareHead(){
    if(result==INTSY){
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }else{
            getsym();
        }
    }else if(result==CHARSY){
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }else{
            getsym();
        }
    }else{
        error();
        return -1;
    }

    printf("This is a head declaration.\n");
    return 0;
}

int varDefine(){
//    printf("in varDefine 1\n");
//    printf(">>>result=%d.\n", result);
    if(result!=INTSY&&result!=CHARSY){
        error();
        return -1;
    }
    // ��ʼʶ���ʶ������
//    printf("in varDefine 2\n");
    getsym();
//    printf(">>>result=%d.\n", result);
    while(true){
        if(result!=IDSY){
            error();
            return -1;
        }
//        printf("in varDefine 3\n");
        getsym();
        if(result==LBRACSY){
            getsym();
            unsignedInt();
//            printf("in varDefine 4\n");
            if(result!=RBRACSY){
//                printf("in varDefine 5\n");
                error();
                return -1;
            }else{
                getsym();
            }
        }
        if(result==COMMASY){
//            printf("in varDefine 6\n");
            getsym();
            continue;
        }else   break;
    }


    printf("This is a variable definition.\n");
    return 0;
}

int varState(){
//    printf("in var state.\n");

    if(varDefine()!=0){
        printf("something went wrong in the first variable definition.\n");
        error();
        return -1;
    }

    if(result==SEMISY){
        getsym();
    }else{
        error();
        return -1;
    }

    while(true){
//        printf("result = %d\n", result);

        int varStateTag = 0;
        recordRead();
        if(result==INTSY||result==CHARSY){
            getsym();
            if(result==IDSY){
                getsym();
                if(result==LPARSY || result==LBRACESY) varStateTag = 1;
            }else   varStateTag = 1;
        }else   varStateTag = 1;

        resetRead();

//        printf("varStateTag=%d\n", varStateTag);
        if(varStateTag==1){
            // ˵����һ���ɷֲ��Ǳ�������

            break;
        }else{
            varDefine();
            if(result!=SEMISY){
                error();
                return -1;
            }
            getsym();

        }

    }

    printf("This is a variable statement.\n");
    return 0;
}

int paraValueList(){
    while(true){
        expr();
        if(result==COMMASY){
            getsym();
            continue;
        }else{
            break;
        }
    }

    printf("This is a parameter value list.\n");
    return 0;
}

int paraList(){
    while(true){
        if(result!=INTSY&&result!=CHARSY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();

        if(result==COMMASY){
            getsym();
            continue;
        }else{
            break;
        }
    }

    printf("This is a parameter list.\n");
    return 0;
}

int retValueFuncDefine(){
    declareHead();
    // ����������֣�����У�
//    printf("pass declare head\n");
    if(result==LPARSY){
        getsym();
        paraList();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();
    }
//    printf("pass tag 1\n");
    if(result!=LBRACESY){   // "{"
        error();
        return -1;
    }
//    printf("pass tag 2\n");
    getsym();
    complexSentence();
//    printf(">>>>>>>>>>>result = %d\n", result);
    if(result!=RBRACESY){   // "}"
        error();
        return -1;
    }
    getsym();

    printf("This is function declaration with returned value.\n");
    return 0;
}

int unretValueFuncDefine(){
    printf("in unretValueFuncDefine.\n");
    if(result!=VOIDSY){
        error();
        return -1;
    }
    getsym();
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
    // ����ֵ�������֣�����У�
    if(result==LPARSY){
        getsym();
        paraList();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();
    }

    if(result!=LBRACESY){   // "{"
        error();
        return -1;
    }
    getsym();
 //   printf("check void complex 1.\n");
    complexSentence();
//    printf("check void complex 2.\n");
//    printf("________________result = %d\n", result);
    if(result!=RBRACESY){   // "}"
        error();
        return -1;
    }
    getsym();

    printf("This is a function declaration without return value.\n");
    return 0;
}

int sentence(){

//    printf(">>>> in sentence\n");
//    printf("result=%d\n", result);
    switch(result){
        case(IFSY):             // �������
            condSentence();
            break;
        case(WHILESY):          // ѭ�����
            loopSentence();
            break;
        case(FORSY):            // ѭ�����
            loopSentence();
            break;
        case(LBRACESY):         // �����
//            printf("this is where the bug might be for a while.\n");
            getsym();
            sentenceSequence();
            if(result!=RBRACESY){
                error();
                return -1;
            }else{
                getsym();
            }
            break;
        case(SEMISY):           // ����䣬ֱ�ӷֺ�
            nullSentence();
            break;
        case(PRINTSY):          // д���
            printSentence();
            if(result!=SEMISY){
                error();
                return -1;
            }else{
                getsym();
            }
            break;
        case(SCANFSY):          // �����
            scanSentence();
            if(result!=SEMISY){
                error();
                return -1;
            }else{
                getsym();
            }
            break;
        case(RETSY):            // �������
            retSentence();
//            printf(">>>>>>>>>>>>>>>>>>>>>>>result=%d\n",result);
            if(result!=SEMISY){
                error();
                return -1;
            }else{
                getsym();
            }
            break;
        case(IDSY):             // ����������������û�ֵ���
            {
                int senTag = 0;
                recordRead();
                getsym();
                if(result==LPARSY || result==SEMISY)  senTag = 1;
                else if(result==EQUSY)    senTag = 2;
                resetRead();



                if(senTag==1){              // ��������
                    retValueFuncCall();
                    if(result!=SEMISY){
                        error();
                        return -1;
                    }else{
                        getsym();
                    }
                    break;
                }else if(senTag==2){        // ��ֵ���
                    assignSentence();
                    if(result!=SEMISY){
                        error();
                        return -1;
                    }else{
                        getsym();
                    }
                    break;
                }else{
                    error();
                    return -1;
                }
            }
            break;

        default:    return -1;
    }

    printf("This is a sentence.\n");
    return 0;
}

int sentenceSequence(){
//    printf(">>>> in sequence sentence\n");
    while(true){
        if(result==IFSY||result==WHILESY||result==FORSY||result==LBRACESY||result==SEMISY||
           result==PRINTSY||result==SCANFSY||result==RETSY||result==IDSY){
        //    printf("send to sentence.\n");
            sentence();
        }else{
            break;
        }

    }

    printf("This is a sequence of sentences.\n");
    return 0;
}

int complexSentence(){
    // to be modified ��ҪԤ������
//    printf("complex sentence 1\n");
    if(result==CONSTSY){
        constState();
    }
//    printf("complex sentence 2\n");

//    int complexTag = 0;
//    recordRead();
//    if(result!=INTSY&&result!=CHARSY){
//        complexTag = 1;
//    }
//    getsym();
//    if(result!=IDSY){
//        complexTag = 1;
//    }
//    getsym();
//    if(result==LBRACSY){
//        complexTag = 1;
//    }
//
//    resetRead();
//
//    if(complexTag==1){
//        varState();
//    }
    // ������������
    int complexTag = 0;
    recordRead();
    if(result!=INTSY&&result!=CHARSY){
        complexTag = 1;
    }
    getsym();
    if(result!=IDSY){
        complexTag = 1;
    }
    getsym();
    if(result==LBRACSY){
        complexTag = 1;
    }

    resetRead();
//    printf("complex-tag = %d\n", complexTag);
    if(complexTag==0){
        varState();
    }
//    printf("complex sentence 3\n");
    // �޸ĺ����������������ֱ�ӵ��ã���������ϲ����ƻ�ָ���λ��
    // ����в���
    sentenceSequence();

    printf("This is a complex sentences.\n");
    return 0;
}

int condSentence(){
    if(result!=IFSY){
        error();
        return -1;
    }
//    printf("in cond 1\n");
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
//    printf("in cond 2\n");
    getsym();
    condition();
//    printf("in cond 3\n");
//    printf("result = %d\n", result);
    if(result!=RPARSY){
        error();
        return -1;
    }
    getsym();
//    printf("in cond 4\n");
    sentence();
    // ��ѡ�� else��֧
    if(result==ELSESY){
//        printf("in cond 5\n");
        getsym();
        sentence();
    }

    printf("This is a conditional sentence.\n");
    return 0;
}

int condition(){
    expr();
    if(result==LESSSY||result==LOESY||result==MORESY||
       result==MOESY||result==LOMSY||result==AEQUSY){
        getsym();
        expr();
    }

    return 0;
}

int stepLength(){
    unsignedInt();
}

int loopSentence(){
    if(result==WHILESY){
        getsym();
        if(result!=LPARSY){
            error();
            return -1;
        }
        getsym();
        condition();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();

        sentence();
    }else if(result==FORSY){
        // ENDRIGHTGERE!!!!
        getsym();
        if(result!=LPARSY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();
        if(result!=EQUSY){
            error();
            return -1;
        }
        getsym();
        expr();
        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();
        condition();
        if(result!=SEMISY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();
        if(result!=EQUSY){
            error();
            return -1;
        }
        getsym();
        if(result!=IDSY){
            error();
            return -1;
        }
        getsym();
        if(result!=PLUSSY&&result!=MINUSSY){
            error();
            return -1;
        }
        getsym();
        stepLength();
        if(result!=RPARSY){
            error();
            return -1;
        }
        getsym();
        sentence();

    }else{
        error();
        return -1;
    }

    printf("This is a loop sentence.\n");
    return 0;
}

int retValueFuncCall(){
//    printf("RETVFC: result=%d\n", result);
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
    if(result==LPARSY){
        getsym();
        paraValueList();
        if(result!=RPARSY){
            error();
            return -1;
        }else{
            getsym();
        }
    }

    printf("This is a function call with returned value.\n");
    return 0;
}

int assignSentence(){
//    printf("assign tag 1\n");
//    printf("result = %d\n", result);
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
//    printf("assign tag 2\n");
    if(result==LBRACSY){
        getsym();
        expr();
        if(result!=RBRACSY){
            error();
            return -1;
        }else   getsym();
    }
//    printf("assign tag 3\n");
    if(result!=EQUSY){
        error();
        return -1;
    }
    getsym();
//    printf("assign tag 4\n");
    expr();
//    printf("assign tag 5\n");
    printf("This is an assignment sentence.\n");
    return 0;
}

int scanSentence(){
    if(result!=SCANFSY){
        error();
        return -1;
    }
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
    getsym();
    if(result!=IDSY){
        error();
        return -1;
    }
    getsym();
    while(true){
        if(result!=COMMASY) break;
        else{
            getsym();
            if(result!=IDSY){
                error();
                return -1;
            }else{
                getsym();
                continue;
            }
        }
    }

    if(result!=RPARSY){
        error();
        return -1;
    }
    getsym();

    printf("This is a scan sentence.\n");
    return 0;
}

int printSentence(){
    if(result!=PRINTSY){
        error();
        return -1;
    }
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
    getsym();
    // �������ж���һ�����ʽ����һ���ַ���
    if(result==STRINGSY){
        getsym();
        if(result==COMMASY){
            getsym();
            expr();
            if(result!=RPARSY){
                error();
                return -1;
            }else{
                getsym();
                printf("This is a print sentence.\n");
                return 0;
            }
        }else if(result==RPARSY){
            // end of print
            getsym();
            printf("This is a print sentence.\n");
            return 0;
        }else{
            error();
            return -1;
        }
    }else{
        expr();
        if(result!=RPARSY){
            error();
            return -1;
        }else{
            getsym();
            printf("This is a print sentence.\n");
            return 0;
        }
    }
    //������Ϸ�֧����������е����������δ֪����
    return -2;
}

int nullSentence(){
    if(result==SEMISY){
        getsym();

        printf("This is an empty sentence.\n");
        return 0;
    }else{
        error();
        return -1;
    }
}

int retSentence(){
    if(result!=RETSY){
        error();
        return -1;
    }
    getsym();

    if(result==LPARSY){
        getsym();
        expr();
        if(result!=RPARSY){
            error();
            return -1;
        }else{

        }
    }
    getsym();
    printf("This is a return sentence.\n");
    return 0;
}

int programAnalysis(){


    // ������������
    if(result==CONSTSY){
        constState();
    }

    // ������������
    int complexTag = 0;
    recordRead();
    if(result!=INTSY&&result!=CHARSY){
        complexTag = 1;
    }
    getsym();
    if(result!=IDSY){
        complexTag = 1;
    }
    getsym();
    if(result==LBRACSY){
        complexTag = 1;
    }

    resetRead();
//    printf("tag = %d\n", complexTag);
    if(complexTag==0){
        varState();
    }

    while(true){
        if(result==INTSY||result==CHARSY){
            retValueFuncDefine();
        }else if(result==VOIDSY){

            int mainTag = 0;
            recordRead();
            getsym();
            if(result==MAINSY){
                mainTag = 1;
            }
            resetRead();


            if(mainTag==0){

                unretValueFuncDefine();
            }else{  // ��һ������������ void main
                break;
            }

        }else{
            error();
            return -1;
        }
    }

    mainAnalysis();

    printf("This is a program analysis.\n");
}

int mainAnalysis(){
//    printf("in main anal\n");
    if(result!=VOIDSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
    if(result!=MAINSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
    if(result!=LPARSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
    if(result!=RPARSY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
    if(result!=LBRACESY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();
//    printf("check main complex 1.\n");
    complexSentence();
//    printf("in main anal\n");
//    printf("check main complex 2.\n");
    if(result!=RBRACESY){
        error();
        return -1;
    }
//    printf("in main anal\n");
    getsym();

    printf("This is a main function.\n");
    return 0;
}

int factor(){
//    printf("DEBUG: in factor\n");
    if(result==LPARSY){             // result = "("
    //    printf("factor-debug branch-1\n");
        getsym();
        expr();                     // ���ʽ
        if(result==RPARSY){         // result = ")"
            getsym();
        }else{
            error();
            return -1;
        }
    }else if(result==IDSY){         // result = IDSY
        getsym();
        if(result==LBRACSY){        // result = "["
        //    printf("factor-debug branch-2\n");
            getsym();
            expr();                 // ���ʽ
            if(result==RBRACSY){
        //        printf("factor-debug branch-2-2\n");
                getsym();
            }else {
                error();
                return -1;
            }
        }else if(result==LPARSY){   // result = "("
        //    printf("factor-debug branch-3\n");
            getsym();
            paraValueList();             // ֵ������
            if(result==RPARSY)      // result = ")"
                getsym();
            else {
                error();
                return -1;
            }
        }else{
            // �����ı�ʶ��
//            printf("factor-debug branch-IDSY\n");

        }
    }else if(result==PLUSSY || result==MINUSSY){
        getsym();
        // ���������ͨ�ġ���������֧
        if(result!=USINTSY){    // �з�������
            error();
            return -1;
        }else{
            getsym();
        }
    }else if(result==USINTSY){
        getsym();
    }else if(result==ACHARSY){
            getsym();               // �ַ� ACHARSY

    }else{
        error();
        return -1;
    }

    printf("This is a factor.\n");
    return 0;
}

int term(){
//    printf(">>>>> result = %d\n", result);
    if(factor()==-1){
        error();
        return -1;
    }
    while(true){
//        printf("term 1 result=%d\n", result);
        if(result==STARSY||result==DIVISY){
            getsym();
//            printf("term 2 result=%d\n", result);
            if(factor()==-1){
                error();
                return -1;
            }
        }else   break;

    }

    printf("This is a term.\n");
    return 0;
}

int expr(){
    // �����ӷ�������������
    if(result==PLUSSY||result==MINUSSY){
        getsym();
    }

    term();
    while(true){
//        printf("expr result = %d\n", result);
        if(result==PLUSSY||result==MINUSSY){
            getsym();
            term();
        }else   break;
    }

    printf("This is an expression.\n");
    return 0;
}


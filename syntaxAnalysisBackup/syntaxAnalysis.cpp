#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "error.cpp"

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
#define RETTSY      12

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
#define RBRACS#define SYY     41
#define LBRACESY    42
#define RBRACESY    43


extern int result;

int factor();
int item();
int expr();

int unsighedInt(){                  // �ķ��е��޷�������

}

int signedInt(){                    // ��Ӧ�ķ��е�����
    if(result==PLUSSY||result==MINUSSY){

    }
}

int constdefine(){
    if(result==INTSY){              // INTSY
        getsym();
        if(result==IDSY){           // ��ʶ��
            getsym();
            if(result==EQUSY){      // "="

            }else{
                error();
            }
        }else{
            error();
        }
    }else if(result==CHARSY){       // CHARSY

    }else{
        error();
    }
}

int conststate(){
    if(result!=CONSTSY){            // ����CONSTSY
        error();
    }else{

    }
}

int factor(){
    if(result==LPARSY){             // result = "("
        getsym();
        expr();                     // ���ʽ
        if(result==RPARSY){         // result = ")"
            getsym();
        }else error();
    }else if(result==IDSY){         // result = IDSY
        getsym();
        if(result==LBRACSY){        // result = "["
            getsym();
            expr();                 // ���ʽ
            if(result==RBRACSY){
                getsym();
            }else error();
        }else if(result==LPARSY){   // result = "("
            getsym();
            paralist();             // ֵ������
            if(result==RPARSY)      // result = ")"
                getsym();
            else    error();
        }else if(result==PLUSSY || result==MINUSSY){
            getsym();
            if(result!=USINTSY)     // �з�������
                error();
        }else if(result==USINTSY){
            getsym();               // �޷�������
        }else if(result==ACHARSY){
            getsym();               // �ַ� ACHARSY
        }
    }
}

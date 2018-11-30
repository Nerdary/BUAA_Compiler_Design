#ifndef SYNTAXANALYSIS_H_INCLUDED
#define SYNTAXANALYSIS_H_INCLUDED

#include <vector>
#include "symbolTable.h"

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

/*
int unsignedInt();
int signedInt();
int constDefine();
int constState();
int decalreHead();
int varDefine();
int varState();
int paraValueList();
int paraList();
int retValueFuncDefine();
int unretValueFuncDefine();
int sentence();
int sentenceSequence();
int complexSentence();
int condSentence();
int condition();
int stepLength();
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
int factor();
int item();
int expr();
*/

extern vector<symbolTableItem> symbolTable;
//extern vector<arrayTableItem> arrayTable;
//extern vector<funcTableItem> funcTable;

extern int globalOffset;

int programAnalysis();


#endif // SYNTAXANALYSIS_H_INCLUDED

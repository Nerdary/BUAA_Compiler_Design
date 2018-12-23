#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "error.h"


void LexicalAnalysisError(LexicalError errorCode, int line){
    switch(errorCode){
        case errSingleChar:
            printf("Line:%d\tMore than one character between single quotes.\n", line);
            break;
        case errIllegalChar:
            printf("Line:%d\tIllegal character between single quotes.\n", line);
            break;
        case errIllegalString:
            printf("Line:%d\tIllegal characters between double quotes.\n", line);
            break;
        case errTransInt:
            printf("Line:%d\tFail to transform string into integer.\n", line);
            break;
        case errNotEqual:
            printf("Line:%d\tIllegal unequal symbol, single exclamation mark.\n", line);
            break;

        default:
            printf("An unknown error might have occurred.\n");
            break;
    }
}

void SyntaxAnalysisError(SyntaxError errorCode, int line){
    switch(errorCode){
        case(errUnexpextedBranch):
            printf("Line:%d\tUnexpected branch.\n", line);
            break;
        case(errLackSemiSymbol):
            printf("Line:%d\tExpected Semi Symbol not exist.\n", line);
            break;
        case(errFactorNotComplete):
            printf("Line:%d\tFactor does not finished normally.\n", line);
            break;
        case(errTermNotComplete):
            printf("Line:%d\tTerm does not finished normally.\n", line);
            break;
        case(errExprNotComplete):
            printf("Line:%d\tExpression does not finished normally.\n", line);
            break;
        case(errCondNotComplete):
            printf("Line:%d\tCondition does not finished normally.\n", line);
            break;
        case(errAssignNotComplete):
            printf("Line:%d\tAssignment sentence does not finished normally.\n", line);
            break;
        case(errCondSentence):
            printf("Line:%d\tCondition sentence does not finished normally.\n", line);
            break;
        case(errAssignDifferType):
            printf("Line:%d\tDifferent type assignment, can not assign.\n", line);
            break;
        case(errUnfitParaType):
            printf("Line:%d\tPara types not match definition.\n", line);
            break;
        case(errParaAmount):
            printf("Line:%d\tPara number not match definition.\n", line);
            break;
        case(errFuncCallNotComplete):
            printf("Line:%d\tFunction call does not finished normally.\n", line);
            break;
        case(errParaValueList):
            printf("Line:%d\tPara value list does not finished normally.\n", line);
            break;
        case(errRetValueInVoid):
            printf("Line:%d\tReturn value in an none return function definition.\n", line);
            break;
        case(errAssignToConst):
            printf("Line:%d\tAssign value to a const.\n", line);
            break;
        case(errRetTypeNotMatch):
            printf("Line:%d\tReturn type not match with function type.\n", line);
            break;
        case(errNoRetInRetFunc):
            printf("Line:%d\tNo return sentence in returned function.\n", line);
            break;
        case(errConditionType):
            printf("Line:%d\tExpression of condition type error.\n", line);
            break;

        case(errSomethingElse):
            printf("Line:%d\tSomething else occurred.\n", line);
            break;

    }
}

void error(){
    printf("an temporary error output.\n");
}

void symbolTableError(TableError errorCode, int line){
    switch(errorCode){
        case(errDuplicate):
            printf("Line:%d\tTheres already an ele with same ID and field in the table.\n", line);
            break;
        case(errArrayLength):
            printf("Line:%d\tFind an ele but it is not an array, cannot get length.\n", line);
            break;
        case(errArrayOutOfRange):
            printf("Line:%d\tThe index is out of the range of array.\n", line);
            break;
        default:
            printf("Line:%d\tAn unknown error might have occurred.\n", line);
            break;

    }
}

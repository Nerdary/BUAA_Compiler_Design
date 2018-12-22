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
        case(errSomethingElse):
            printf("Line:%d\tSomething else occurred.\n", line);
            break;

    }
}

void error(){
    printf("an temporary error output.\n");
}

void symbolTableError(TableError errorCode){
    switch(errorCode){
        case(errDuplicate):
            printf("Theres already an ele with same ID and field in the table.\n");
            break;
        case(errArrayLength):
            printf("Find an ele but it is not an array, cannot get length.\n");
            break;
        case(errArrayOutOfRange):
            printf("The index is out of the range of array.\n");
            break;
        default:
            printf("An unknown error might have occurred.\n");
            break;

    }
}

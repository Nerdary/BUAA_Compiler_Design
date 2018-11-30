#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <iostream>
#include <string>

// 词法分析错误处理
enum LexicalError{
    errSingleChar,      // 两个单引号之间多于一个字符
    errIllegalChar,     // 两个单引号之间出现非法字符
    errIllegalString,   // 非法的字符串
    errTransInt,        // 转成整型数字过程中出错
    errNotEqual,        // 不等于号出错，不允许单独的感叹号
};

// 语法分析错误处理
enum SyntaxError{
    errUnfitSyntaxElement,  // 与该子程序分析对象不符，直接弹出
    errUnfitWhileProcess,   // 成功进入子程序，但过程中元素不匹配
                            // 指一般处理中遇到错误，需要终止子程序
                            // 的情况
    errUnfitTypeSymbol,     // 不匹配的类型标识符
    errLackRightBracket,    // 缺少右中括号
    errLackRightBrace,      // 缺少右大括号
    errLackRightParen,      // 缺少右小括号
    errLackSemiSymbol,      // 没有出现预期的分号
};

enum TableError{
    errDuplicate,           // 重复定义
    errArrayLength,         // 查找array长度出错
    errArrayOutOfRange,     // 数组索引越界
};

extern void LexicalAnalysisError(LexicalError errorCode, int line);
extern void SyntaxAnalysisError(SyntaxError errorCode, int line);
extern void symbolTableError(TableError errorCode);

extern void error();

#endif // ERROR_H_INCLUDED

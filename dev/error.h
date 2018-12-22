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

    errUnexpextedBranch,    // 预期之外的分支
    errFactorNotComplete,   // 因子分程序没有正常结束
    errTermNotComplete,     // 项分程序没有正常结束
    errExprNotComplete,     // 表达式分程序没有正常结束
    errCondNotComplete,     // 条件没有正常结束
    errAssignNotComplete,   // 赋值语句中出错
    errCondSentence,        // 条件语句中出错
    errAssignDifferType,    // 赋值语句两边类型不同

    errUnfitParaType,       // 传入的参数类型不正确
    errParaValueList,       // 值参数表分程序没有正常完成
    errFuncCallNotComplete, // 函数调用分程序没有正常完成

    errSomethingElse,
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

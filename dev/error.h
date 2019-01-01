#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <iostream>
#include <string>

// �ʷ�����������
enum LexicalError{
    errSingleChar,          // ����������֮�����һ���ַ�
    errIllegalChar,         // ����������֮����ַǷ��ַ�
    errIllegalString,       // �Ƿ����ַ���
    errTransInt,            // ת���������ֹ����г���
    errNotEqual,            // �����ںų������������ĸ�̾��

    errUnsignInt,           //
    errConstDef,            //
    errConstState,          //
    errDecHead,             //
    errVarDef,              //
    errVarState,            //
    errParaList,            //
    errUnRetFuncDef,        //


};

// �﷨����������
enum SyntaxError{
    errUnfitSyntaxElement,  // ����ӳ���������󲻷���ֱ�ӵ���
    errUnfitWhileProcess,   // �ɹ������ӳ��򣬵�������Ԫ�ز�ƥ��
                            // ָһ�㴦��������������Ҫ��ֹ�ӳ���
                            // �����
    errUnfitTypeSymbol,     // ��ƥ������ͱ�ʶ��
    errLackLeftBracket,     // ȱ����������
    errLackRightBracket,    // ȱ����������
    errLackRightBrace,      // ȱ���Ҵ�����
    errLackRightParen,      // ȱ����С����
    errLackSemiSymbol,      // û�г���Ԥ�ڵķֺ�

    errUnexpextedBranch,    // Ԥ��֮��ķ�֧
    errFactorNotComplete,   // ���ӷֳ���û����������
    errTermNotComplete,     // ��ֳ���û����������
    errExprNotComplete,     // ���ʽ�ֳ���û����������
    errCondNotComplete,     // ����û����������
    errAssignNotComplete,   // ��ֵ����г���
    errCondSentence,        // ��������г���
    errAssignDifferType,    // ��ֵ����������Ͳ�ͬ

    errUnfitParaType,       // ����Ĳ������Ͳ���ȷ
    errParaAmount,          // ����Ĳ�����������ȷ
    errParaValueList,       // ֵ������ֳ���û���������
    errFuncCallNotComplete, // �������÷ֳ���û���������

    errRetValueInVoid,      // ���޷���ֵ�����з�����ֵ

    errAssignToConst,       // ��������ֵ

    errRetTypeNotMatch,     // ����ֵ�����뺯�����Ͳ����
    errNoRetInRetFunc,      // �з���ֵ��������û��ret

    errConditionType,       // ��ϵ��������߱��붼��int

    errSomethingElse,
};

enum TableError{
    errDuplicate,           // �ظ�����
    errArrayLength,         // ����array���ȳ���
    errArrayOutOfRange,     // ��������Խ��
};

extern void LexicalAnalysisError(LexicalError errorCode, int line);
extern void SyntaxAnalysisError(SyntaxError errorCode, int line);
extern void symbolTableError(TableError errorCode, int line);

extern void error();

#endif // ERROR_H_INCLUDED

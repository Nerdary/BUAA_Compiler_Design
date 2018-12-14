#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <iostream>
#include <string>

// �ʷ�����������
enum LexicalError{
    errSingleChar,      // ����������֮�����һ���ַ�
    errIllegalChar,     // ����������֮����ַǷ��ַ�
    errIllegalString,   // �Ƿ����ַ���
    errTransInt,        // ת���������ֹ����г���
    errNotEqual,        // �����ںų������������ĸ�̾��
};

// �﷨����������
enum SyntaxError{
    errUnfitSyntaxElement,  // ����ӳ���������󲻷���ֱ�ӵ���
    errUnfitWhileProcess,   // �ɹ������ӳ��򣬵�������Ԫ�ز�ƥ��
                            // ָһ�㴦��������������Ҫ��ֹ�ӳ���
                            // �����
    errUnfitTypeSymbol,     // ��ƥ������ͱ�ʶ��
    errLackRightBracket,    // ȱ����������
    errLackRightBrace,      // ȱ���Ҵ�����
    errLackRightParen,      // ȱ����С����
    errLackSemiSymbol,      // û�г���Ԥ�ڵķֺ�
};

enum TableError{
    errDuplicate,           // �ظ�����
    errArrayLength,         // ����array���ȳ���
    errArrayOutOfRange,     // ��������Խ��
};

extern void LexicalAnalysisError(LexicalError errorCode, int line);
extern void SyntaxAnalysisError(SyntaxError errorCode, int line);
extern void symbolTableError(TableError errorCode);

extern void error();

#endif // ERROR_H_INCLUDED

#ifndef GETSYM_H_INCLUDED
#define GETSYM_H_INCLUDED

extern int result;
extern int lc;
extern int IntValue;
extern char strings[200];
extern char symbol[10];
extern char token[200];

void recordRead();
void resetRead();
int getsym();
void err(int index);


void clearToken();
int isSpace(char a);
int isNewline(char a);
int isTab(char a);
int isLetter(char a);
int isDigit(char a);
void catToken(char a);
int isReserved();
int transNum(char* token);
#endif // GETSYM_H_INCLUDED

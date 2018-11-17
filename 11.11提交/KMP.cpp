#include <stdio.h>
#include <string.h>

#define N 100

int kmp_ret = -1;

void get_next(char *s, int *nextval){
	int len = strlen(s);
	int i = 0, j = -1;
	
	nextval[0] = -1;
	
	while(i < (len - 1)){
		if (j == -1){
			i = i + 1;
			j = j + 1;
			
			if(s[i] != s[j]){
				nextval[i] = j;
			} else{
				nextval[i] = nextval[j];
			}
		} else{
			if (s[i] == s[j]){
				i = i + 1;
				j = j + 1;
			
				if(s[i] != s[j]){
					nextval[i] = j;
				} else{
					nextval[i] = nextval[j];
				}
			} else{
				j = nextval[j];
			}
		}
	}
	
	return ;
} 

int kmp(char *s, char *t){
	int k = -1, i = 0, j = 0;
	int nextval[N] = {0};// to be modified
	
	int s_len = strlen(s);
	int t_len = strlen(t);
	
	get_next(s, nextval);
	printf("the content of nextval: ");
	for(k=0;k<s_len;k=k+1){
		printf("%d, ", nextval[k]);
	}
	
	
	while(i<t_len && j<s_len){
		if (j == -1 || t[i] == s[j]){
			i = i + 1;
			j = j + 1;
		} else{
			j = nextval[j];	
		}
		
		
	}
	
	if(j >= strlen(s)){
		kmp_ret = i - strlen(s);
		return (kmp_ret);
	} else{
		return (-1);
	}
}

int main(){
	char s[N], t[N];
	printf("ÇëÊäÈë´ıÆ¥Åä×Ö·û´®:");
    scanf("%s",s);

    printf("ÇëÊäÈëÆ¥Åä×Ö·û´®:");
    scanf("%s",t);
	kmp(s, t);
	printf("kmp_ret = %d\n", kmp_ret);
	
	return 0;
}







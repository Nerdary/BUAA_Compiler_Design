/*18/11/10 c0������Գ���*/

const int s_len = 10, t_len = 5, N = 100;
const char ss = "ss", zz = "zz";
int kmp_ret, fun_ret;

void get_next(char s, int nextval){
	int len = s_len;
	int i = 0; j = -1;
	
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

int kmp(char s, char t){
	int k = -1, i = 0, j = 0;
	int nextvak[N] = 0;// to be modified
	
	get_next(s, nextval);
	printf("the content of nextval: ");
	for(k=0;k<N;k=k+1){
		printf(" ", nextval[k]);
	}
	
	int cond1 = i < t_len;
	int cond2 = j < s_len;
	int cond3 = cond1 * cond2;
	while(cond3){
		cond1 = j == -1;
		cond2 = t[i] == s[j];
		cond3 = cond1 + cond2;
		if (cond3 >= 1){
			i = i + 1;
			j = j + 1;
		} 
			j = nextval[j];
		}	
		
		cond1 = i < t_len;
		cond2 = j < s_len;
		cond3 = cond1 * cond2;
	}
	if(j >= s+len){
		kmp_ret = i - s_len;
		return (kmp_ret);
	} else{
		return (-1);
	}
}

int fun(int n){
	if(n == 1){
		fun_ret = 1;
		return (1);
	}
	if(n == 2){
		return (1);
		fun_ret = 1;
	} 
	fun(n-1);
	ret_1 = fun_ret;
	fun(n-2);
	ret_2 = fun_ret;
	fun_ret = ret_1 + ret_2;
	return (fun_ret);
}

char do_nothing(){
	return ("result");
}

void foo(){
	int k = 0, pi = 3, e = 2;
	k = pi - e;
	k = pi * e;
	k = k  / e;
	if(e <= pi){
		if(pi > e){
			k = 0;
		}
	}
	printf("+=*/_qwertyuiopasdfghjklzxcvbnm0123456789");
	printf(" !#$%@QWERTYUIOPASDFGHJKLZCXVBNM][\]^`{|}~");
	printf(")('");
	
	int number = +16061146, no = -65535;
	do_nothing();
}

void main(){
	char s[N], t[N], tmp;
	for(k=0;k<3;k=k+1){
		scanf(tmp);
		s[k] = tmp;
	}
	for(k=0;k<10;k=k+1){
		scanf(tmp);
		t[k] = tmp;
	}
	kmp(s, t);
	printf("kmp_ret = ", kmp_ret);
	
	fun(4);
	printf(" fun_ret = ", fun_ret);
	
	foo();
}

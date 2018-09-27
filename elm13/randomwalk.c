#include <stdio.h>

int main(){
	int r = 0;
	int i;
	for(i = 0; i < 2196; i ++){
		if(rand() % 2)
			r ++;
		else
			r --;
		printf("%d\n", r);
	}
	return 0;
}

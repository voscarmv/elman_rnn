#include <stdio.h>

int main(){
	int i = 0;
	int r;
	for(i = 0; i < 4400; i ++)
	{
		r = rand() % 4;
		if(r == 0)
			printf("-1 -1\n");
		if(r == 1)
			printf("-1 1\n");
		if(r == 2)
			printf("1 -1\n");
		if(r == 3)
			printf("1 1\n");
	}
	return 0;
}

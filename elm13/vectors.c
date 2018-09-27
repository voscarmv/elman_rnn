#include <stdio.h>
#include <stdlib.h>

int main(){
	int **v = (int **) malloc(sizeof(int *) * 5);
	int i;
	for(i = 0; i < 5; i ++)
		v[i] = (int *) malloc(sizeof(int) * i);
	for(i = 0; i < 5; i ++)
		free(v[i]);

	int lol[][3] = {
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	};

	free(v);
	return 0;
}

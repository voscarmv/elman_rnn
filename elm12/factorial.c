#include <stdio.h>

long double factorial(long double n);

int main(){
	long double i;
	long double t = 0;
	long double m = 100.0;
	long double p[100];
	int j;
	for(j = 0, i = 0.0; i < m; j ++, i ++){
		p[j] = (factorial(m) / (factorial(i) * factorial(m - i))) / 1.2676506e+30;
		printf("%Le\n", p[j]);
//		t += factorial(m) / (factorial(i) * factorial(m - i));
	}

//	long double pr = 0;
//	for(j = 50; j < 100; j ++)
//		printf("%d. %Le\n", j, pr += p[j]);

//	printf("Total: %Le\n", t);
//	printf("Expected: %e\n", 1.2676506e+30);
}

long double factorial(long double n){
	long double x = 1.0;
	n ++;
	while(--n)
		x *= n;
	return x;
}

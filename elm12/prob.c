/*
	prob.c

	Comprobacion de la ley de los grandes numeros
	utilizando las formulas para varianza y media de una
	distribucion binomial con n = atoi(argv[1]), y p = 0.5

	<vomv1988@gmail.com>

	compilar con
		$ gcc -lm -o prob prob.c
	ejecutar con
		$ ./prob 100
		$ ./prob 1200
		$ ./prob 10034
		...
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

int main(int argc, char *argv[]){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	int i, j, k, m = atoi(argv[1]);
	double min = 1, max = 0, x;

	for(k = 0; k < 1000; k ++){
		j = 0;
		for(i = 0; i < m; i ++)
			if(rand() % 2)
				j ++;

		x = (double) j / m;

		if(x < min)
			min = x;
		if(x > max)
			max = x;
	}

	double tmin = ((m-(sqrt(m)*3))/2)/m;
	double tmax = ((m+(sqrt(m)*3))/2)/m;

	printf("theoretical\nmin %f\nmax %f\n\n", tmin, tmax);
	printf("practical\nmin %f\nmax %f\n", min, max);

	return 0;
}

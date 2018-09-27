#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]){
	int
		i, j,
		l = atoi(argv[1])
//		start=atoi(argv[2]),
//		trainlen=atoi(argv[3]),
//		testlen=atoi(argv[4])
	;

	double *s = (double *) malloc(sizeof(double) * l); /* samples */
	double *r = (double *) malloc(sizeof(double) * l); /* logarithmic return rates */
//	double *m = (double *) malloc(sizeof(double) * l); /* moving averages (5 samples) */
//	double *m2 = (double *) malloc(sizeof(double) * l); /* moving averages (5 samples) */

	for(i = 0; i < l; r[i++] = 0);
//	for(i = 0; i < l; m[i++] = 0);

	for(i = 0; i < l; i ++)
		scanf("%lf", s + i);
	for(i = 1; i < l; i ++)
		if(s[i] > s[i - 1])
			r[i] = 1;
		else
			r[i] = -1;
/*
	for(i = 5; i < l; i ++)
		m[i] = (r[i] + r[i - 1] + r[i - 2] + r[i - 3] + r[i - 4]) / 5;
*/
	for(i = 1; i < l; i ++)
		printf("%.12f %.12f\n", r[i], r[i+1]);
/*
	double max = m[5];
	double min = m[5];
	for(i = start; i < start + trainlen; i ++){
		if(m[i] > max)
			max = m[i];
		if(m[i] < min)
			min = m[i];
	}

	double ampl = max - min;
	double low = 0.3;
	double high = 0.98;
	double factor = (high - low) / ampl;
	for(i = start; i < start + trainlen + testlen + 1; i ++)
		m2[i] = ((m[i] - min) * factor) + low;

	printf("factor %f\n", factor);
	printf("min %f\n", min);
	printf("max %f\n", max);
	printf("low %f\n", low);
	printf("high %f\n", high);

	for(i = start; i < start + trainlen; i ++)
		printf("train %.4f %.4f %.4f\n", r[i], m[i], m2[i + 1]);
	for(i = start + trainlen; i < start + trainlen + testlen; i++)
		printf("test1 %.4f %.4f %.4f\n", r[i], m[i], m2[i + 1]);

	for(i = start; i < start + trainlen; i++)
		printf("test2 \n");
	for(i = start + trainlen; i < start + trainlen + testlen; i++)
		printf("test2 %.4f %.4f %.4f\n", r[i], m[i], m2[i + 1]);
*/

	free(s);
	free(r);
//	free(m);
//	free(m2);

	return 0;
}

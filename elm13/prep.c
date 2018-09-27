/*
	prep.c
	Sun Dec 28 17:33:06 CST 2014

	Formatea los datos crudos de get_dataset.sh para
	introducirlos por STDIN a elmanx.c

	Para una neurored con mas de 1 neurona en la capa
	de entrada y/o salida, habria que incrementar el
	tamano del arreglo s[]

	<vomv1988@gmail.com>
*/

#include <stdio.h>

int main(){
	double s[3];

	scanf("%lf", s);
	scanf("%lf", s + 1);
	while(scanf("%lf", s + 2) != EOF){
		printf("%d %d\n", direction(s[0], s[1]), direction(s[1], s[2]));
		s[0] = s[1];
		s[1] = s[2];
	}

	return 0;
}

int direction(double s1, double s2){
	if(s1 < s2)
		return 1;
	else
		return -1;
}

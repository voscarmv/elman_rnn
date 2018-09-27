#include <stdio.h>
#include <stdlib.h>

typedef struct srn{
	int
		**neurolayers,
		*layerlens,
		layers,
		**weights,
		(*propagation)[3],
		fitness
	;
} Srn, *SrnPtr;

int main(){

	int
		layers = 4,
		*layerlens = (int *) malloc(sizeof(int) * layers),
		(*propagation)[3] = (int (*)[3]) malloc(sizeof(int [3]) * layers)
	;

	layerlens[0] = 1; /* input */
	layerlens[1] = 4; /* hidden */ 
	layerlens[2] = 3; /* context */
	layerlens[3] = 1; /* output */

	propagation[0][0] = 2; /* connect layer 2 to layer 1 */
	propagation[0][1] = 0; /* through weights 0 */
	propagation[0][2] = 1;

	propagation[1][0] = 0;
	propagation[1][1] = 1;
	propagation[1][2] = 1;

	propagation[2][0] = 1;
	propagation[2][1] = 2;
	propagation[2][2] = 2;

	propagation[3][0] = 2;
	propagation[3][1] = 3;
	propagation[3][2] = 3;

	SrnPtr net = (SrnPtr) malloc(sizeof(Srn));

	net->layers = 4;
	net->layerlens = layerlens;
	net->propagation = propagation;

	free(layerlens);
	free(propagation);
	free(net);
}

SrnPtr netalloc(int layers, int *layerlens, int (*propagation)[3){
	
}

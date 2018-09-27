#include <stdio.h>

typedef struct weight Weight, *WeightPtr;

typedef struct neuron{
	int
		value,
		neighbors
	;
	WeightPtr weights;
} Neuron, *NeuronPtr;

struct weight{
	int value;
	NeuronPtr neighbor;
};

typedef struct layer{
	int length;
	NeuronPtr neurons;
} Layer, *LayerPtr;

typedef struct srn{
	int
		laylen,
		ordlen,
		*proporder[2]
	;
	LayerPtr layers;
} Srn, *SrnPtr;

int main(){
	
	return 0;
}

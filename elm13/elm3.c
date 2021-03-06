#include <stdio.h>
#include <stdlib.h>

typedef struct srn{ /* Simple Recurrent Network */
	int
		**neurolayers,
		*neurolengs,
		layers,
		***weightlayers,
		weights,
		(*propagation)[2],
		fitness
	;
} Srn, *SrnPtr;

typedef struct sample{ /* Dataset sample */
	int
		*inputs,
		*outputs
	;
} Sample, *SamplePtr;

typedef struct dataset{ /* Whole dataset */
	SamplePtr samples; /* array of samples */
	int
		inlen, /* all samples must have the same amount of inputs */
		outlen /* and outputs */
	;
} Dataset, DatasetPtr;

SrnPtr netalloc(int layers, int *neurolengs, int weights, int (*propagation)[2]);
int netfree(SrnPtr net);
int randd(int x);
int clearnet(SrnPtr net);
int printnet(SrnPtr net);
int disturbnet(SrnPtr net);
int propanet(SrnPtr net, SamplePtr sample);
SamplePtr sampalloc(int *inputs, int inlen, int *outputs, int outlen);
int sampfree(SamplePtr sample);
int clearlayer(SrnPtr net, int layer);

int main(){
	int layers = 4; /* number of neurolayers in srn */
	int weights = 4; /* number of weight layers in srn */
	int neurolengs[] = { /* neurolayer lengths */
		1, /* 0 input */
		4, /* 1 hidden */
		3, /* 2 context */
		1 /* 3 output */
	};
	int propagation[][2] = { /* propagation pattern for srn */
		{2, 1}, /* connect neurolayer 2 with 1 through weightlayer 0 */
		{0, 1}, /* etc. */
		{1, 2}, /* etc. */
		{2, 3} /* etc. */
	};

	int inputs[] = {
		1
	};
	int outputs[] = {
		-1
	};

	SrnPtr net = netalloc(layers, neurolengs, weights, propagation);
	SamplePtr sample = sampalloc(inputs, 1, outputs, 1);

	printf("CLEAR\n");
	clearnet(net);
	printnet(net);

	printf("DISTURB\n");
	disturbnet(net);
	printnet(net);

	printf("PROPAGATE\n");
	propanet(net, sample);
	printnet(net);

	printf("CLEAR NOT CONTEXT\n");
	clearlayer(net, 0);
	clearlayer(net, 1);
	clearlayer(net, 3);
	printnet(net);

	printf("PROPAGATE AGAIN\n");
	propanet(net, sample);
	printnet(net);

	netfree(net);
	sampfree(sample);

	return 0;
}

SrnPtr netalloc(int layers, int *neurolengs, int weights, int (*propagation)[2]){
	int i, j;

	int **neurolayers = (int **) malloc(sizeof(int *) * layers);
	for(i = 0; i < layers; i ++)
		neurolayers[i] = (int *) malloc(sizeof(int) * neurolengs[i]);

	int ***weightlayers = (int ***) malloc(sizeof(int **) * weights);
	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < weights; i ++){
		from = propagation[i][0];
		fromlen = neurolengs[from];

		to = propagation[i][1];
		tolen = neurolengs[to];

		weightlayers[i] = (int **) malloc(sizeof(int *) * fromlen);
		for(j = 0; j < fromlen; j ++)
			weightlayers[i][j] = (int *) malloc(sizeof(int) * tolen);
	}

	SrnPtr net = (SrnPtr) malloc(sizeof(Srn));

	net->layers = layers;
	net->weights = weights;
	net->neurolengs = neurolengs;
	net->propagation = propagation;

	net->neurolayers = neurolayers;
	net->weightlayers = weightlayers;

	return net;
}

int netfree(SrnPtr net){
	int i, j;

	for(i = 0; i < net->layers; i ++)
		free(net->neurolayers[i]);
	free(net->neurolayers);

	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			free(net->weightlayers[i][j]);
		free(net->weightlayers[i]);
	}
	free(net->weightlayers);

	free(net);

	return 0;
}

int randd(int x){
	return x + rand() % 3 - 1;
}

int clearnet(SrnPtr net){
	int i, j, k;

	for(i = 0; i < net->layers; i ++)
		for(j = 0; j < net->neurolengs[i]; j ++)
			net->neurolayers[i][j] = 0;
	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				net->weightlayers[i][j][k] = 0;
	}

	return 0;
}

int printnet(SrnPtr net){
	int i, j, k;

	printf("layers: %d,  weights: %d\n", net->layers, net->weights);

	for(i = 0; i < net->layers; i ++)
		printf("neuroleng %d: %d\n", i, net->neurolengs[i]);

	for(i = 0; i < net->layers; i ++)
		for(j = 0; j < net->neurolengs[i]; j ++)
			printf("neuron %d, %d: %d\n", i, j, net->neurolayers[i][j]);
	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		printf("weightlayer %d: from neurolayer %d to neurolayer %d\n", i, from, to);

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				printf("weight %d, %d: %d\n", j, k, net->weightlayers[i][j][k]);
	}

	return 0;
}

int disturbnet(SrnPtr net){
	int i, j, k;

	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				net->weightlayers[i][j][k] = randd(net->weightlayers[i][j][k]);
	}

	return 0;
}

int propanet(SrnPtr net, SamplePtr sample){
	int i, j, k;

	for(i = 0; i < net->neurolengs[0]; i ++)
		net->neurolayers[0][i] = sample->inputs[i];

	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				net->neurolayers[to][k] +=
					net->neurolayers[from][j] * net->weightlayers[i][j][k];
	}
	return 0;
}

SamplePtr sampalloc(int *inputs, int inlen, int *outputs, int outlen){
	int i;
	SamplePtr sample = (SamplePtr) malloc(sizeof(Sample));

	sample->inputs = (int *) malloc(sizeof(int) * inlen);
	for(i = 0; i < inlen; i ++)
		sample->inputs[i] = inputs[i];

	sample->outputs = (int *) malloc(sizeof(int) * outlen);
	for(i = 0; i < outlen; i ++)
		sample->outputs[i] = outputs[i];

	return sample;
}

int sampfree(SamplePtr sample){
	free(sample->inputs);
	free(sample->outputs);
	free(sample);

	return 0;
}

int clearlayer(SrnPtr net, int layer){
	int i;

	for(i = 0; i < net->neurolengs[layer]; i ++)
		net->neurolayers[layer][i] = 0;

	return 0;
}

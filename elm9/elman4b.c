#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define NUMSTRLEN 10
#define MAXSAMPLES 5000

/* Neurons */
#define INPUT 0
#define HIDDEN 1
#define CONTEXT 2
#define OUTPUT 3

/* Weights */
#define INPUT_HIDDEN 0
#define CONTEXT_HIDDEN 1
#define HIDDEN_CONTEXT 2
#define HIDDEN_OUTPUT 3

#define FIRST 0
#define SECOND 1

typedef struct elman{
	int lengths[4];
	int lengthpairs[4][2];
	double
		fitness,
		*neurons[4],
		**weights[4]
	;
	char ***c_weights[4];
} Elman, *ElmanPtr;

typedef struct dataset{
	int dataset_l;
	double **dataset;
} Dataset, *DatasetPtr;

ElmanPtr elmalloc(int input_l, int hidden_l, int context_l, int output_l);
void elmfree(ElmanPtr net);

DatasetPtr datalloc(int io_l);
void datfree(DatasetPtr data);

void printnet(ElmanPtr net);

void randnet(ElmanPtr net);
void randstr(char *numstring);
char randsign();
char randrange(int r);
void stodnet(ElmanPtr net);
double stod(char *s);

void sortnets(ElmanPtr *nets);
int comparenets(const void *net1, const void *net2);

void crossnets(ElmanPtr outnet, ElmanPtr net1, ElmanPtr net2);
void mutanet(ElmanPtr net, int invprob);

double fitness(ElmanPtr net, DatasetPtr dataset);
void clearnet(ElmanPtr net);
void clearnotctx(ElmanPtr net);
void netoutput(ElmanPtr net, double *io);
double sigmoid(double x);
void printnetio(ElmanPtr net, DatasetPtr data);
void fprintnetio(char *fname, ElmanPtr net, DatasetPtr data);

void savenet(char *fname, ElmanPtr net);
void loadnet(char *fname, ElmanPtr net);

int main(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	int
		i, j,
		input = 1,
		hidden = 50,
		context = 50,
		output = 1,
		population = 120,
		chosen = 20,
		applefromtree = 1000,
		epochs = 20000
	;

	char fname[64];

	DatasetPtr data = datalloc(input + output);
	ElmanPtr *nets = (ElmanPtr *) malloc(sizeof(ElmanPtr) * population);
/**/
		printf("Initializing nets:\n");
		fflush(NULL);
		for(i = 0; i < population; i ++){
			nets[i] = elmalloc(input, hidden, context, output);
			randnet(nets[i]);
			fitness(nets[i], data);
			printf("%d / %d\n", i, population);
			fflush(NULL);
		}
		for(j = 0; j < epochs; j ++){
			qsort(nets, population, sizeof(nets[0]), comparenets);
			printf("Generation %d/%d, best net: %f\n", j + 1, epochs, nets[0]->fitness);
			fflush(NULL);

			sprintf(fname, "generation_%09d_bestsrn.bin", j);
			printf("Saving best srn: %s\n", fname);
			fflush(NULL);
			savenet(fname, nets[0]);
			printf("Saved.\n");
			fflush(NULL);

			for(i = 0; i < chosen; i ++){
				sprintf(fname, "generation_%09d_srn_%09d.dat", j, i);
				printf("Saving plot: %s\n", fname);
				fflush(NULL);
				fprintnetio(fname, nets[i], data);
				printf("Saved.\n");
				fflush(NULL);
			}

			printf("Cross nets:\n");
			fflush(NULL);
			for(i = chosen; i < population; i ++){
				crossnets(
					nets[i],
					nets[rand() % chosen],
					nets[rand() % chosen]
				);
				mutanet(nets[i], applefromtree);
				fitness(nets[i], data);
				printf("%d / %d\n", i, population);
				fflush(NULL);
			}
		}
/**/
	for(i = 0; i < population; i ++)
		elmfree(nets[i]);
	datfree(data);
	return 0;
}

void fprintnetio(char *fname, ElmanPtr net, DatasetPtr data){
	int i, j, k;
	FILE *fp = fopen(fname, "w");
	clearnet(net);
	for(i = 0; i < data->dataset_l; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i]);
		for(k = 0; k < net->lengths[OUTPUT]; k ++)
			fprintf(fp, "%f ", sigmoid(net->neurons[OUTPUT][k]));
		fprintf(fp, "\n");
	}
	fclose(fp);
	return;
}
/*
void fprintnetio(char *fname, ElmanPtr *nets, DatasetPtr data, int chosen){
	int i, j, k;
	FILE *fp = fopen(fname, "w");
	for(i = 0; i < chosen; i ++)
		clearnet(nets[i]);
	for(i = 0; i < data->dataset_l; i ++){
		for(
			j = nets[0]->lengths[INPUT];
			j < nets[0]->lengths[INPUT] + nets[0]->lengths[OUTPUT];
			j ++
		)
			fprintf(fp, "%f ", data->dataset[i][j]);
		for(j = 0; j < chosen; j ++){
			clearnotctx(nets[j]);
			netoutput(nets[j], data->dataset[i]);
			for(k = 0; k < nets[0]->lengths[OUTPUT]; k ++)
				fprintf(fp, "%f ", sigmoid(nets[j]->neurons[OUTPUT][k]));
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return;
}
*/
void savenet(char *fname, ElmanPtr net){
	int i, j, k;
	char *c;
	FILE *fp = fopen(fname, "w");
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++){
				c = (char *) &(net->weights[i][j][k]);
				fprintf(
					fp, "%c%c%c%c%c%c%c%c",
					c[0], c[1], c[2], c[3],
					c[4], c[5], c[6], c[7]
				);
			}
	fclose(fp);
	return;
}

void loadnet(char *fname, ElmanPtr net){
	int i, j, k, l;
	char *c = (char *) malloc(sizeof(double));
	double *w;
	FILE *fp = fopen(fname, "r");
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++){
				for(l = 0; l < sizeof(double); l ++)
					c[i] = fgetc(fp);
				w = (double *) c;
				net->weights[i][j][k] = *w;
			}
	free(c);
	fclose(fp);
	return;
}


DatasetPtr datalloc(int io_l){
	int
		i = 0,
		j = 0
	;

	double **dataset = (double **) malloc(sizeof(double *));
	dataset[0] = (double *) malloc(sizeof(double) * io_l);

	while(
		scanf(
			"%lf",
			&(dataset[i][j++])
		) != EOF && i < MAXSAMPLES
	){
		if(j > io_l - 1){
			dataset = realloc(dataset, sizeof(double *) * ((++i) + 1));
			dataset[i] = malloc(sizeof(double) * io_l);
			j = 0;
		}
	}
	int dataset_l = i;
	free(dataset[dataset_l]);

	DatasetPtr data = (DatasetPtr) malloc(sizeof(Dataset));
	data->dataset_l = dataset_l;
	data->dataset = dataset;

	return data;
}

void datfree(DatasetPtr data){
	int i;
	for(i = 0; i < data->dataset_l; i ++)
		free(data->dataset[i]);
	free(data->dataset);
	free(data);
	return;
}

void randnet(ElmanPtr net){
	int i, j, k;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++)
				randstr(net->c_weights[i][j][k]);
	stodnet(net);
	return;
}

void randstr(char *numstring){
	numstring[0] = randsign();
	numstring[1] = randrange(6);
	numstring[2] = randrange(6);

	int i;
	for(i = 3; i < NUMSTRLEN - 1; i ++)
		numstring[i] = randrange(10);

	numstring[NUMSTRLEN - 1] = '\0';
	return;
}

char randsign(){
	if(rand() % 2 == 0)
		return '0';
	else
		return '-';
}

char randrange(int r){
	return (char) ((rand() % r) + (int) '0');
}

void stodnet(ElmanPtr net){
	int i, j, k;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++)
				net->weights[i][j][k] = stod(net->c_weights[i][j][k]);
	return;
}

double stod(char *s){
	return ((double) atoi(s)) / ((double) pow(10, NUMSTRLEN - 3));
}

void printnet(ElmanPtr net){
	int i, j, k;
	for(i = 0; i < 4; i ++)
		printf("lengths[%d] = %d\n", i, net->lengths[i]);
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++)
				printf(
					"weights[%d][%d][%d] = %f\n"
					"c_weights[%d][%d][%d] = %s\n",
					i, j, k, net->weights[i][j][k],
					i, j, k, net->c_weights[i][j][k]
				);

	return;
}

void mutanet(ElmanPtr net, int invprob){
	int i, j, k, l;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++)
				for(l = 0; l < NUMSTRLEN - 1; l ++)
					if(!(rand() % invprob))
						if(l == 0)
							net->c_weights[i][j][k][l] = randsign();
						else if(l == 1 || l == 2)
							net->c_weights[i][j][k][l] = randrange(6);
						else
							net->c_weights[i][j][k][l] = randrange(10);
	stodnet(net);
	return;
}

void crossnets(ElmanPtr outnet, ElmanPtr net1, ElmanPtr net2){
/*
	IMPROVE
	Change all code to use contiguous 1D array accesible by
		c_weights[i][j * width * height + k * width + l];
	This will make it possible to use
		memcpy(no, n1, sizeof(char) * r)
		memcpy(no + r, n1 + r, sizeof(char) * (c_length - r)
*/
	int
		i, j, k, l, ctr = 0,
		c_length =
		(
			net1->lengths[INPUT] * net1->lengths[HIDDEN] +
			net1->lengths[HIDDEN] * net1->lengths[CONTEXT] +
			net1->lengths[CONTEXT] * net1->lengths[HIDDEN] +
			net1->lengths[HIDDEN] * net1->lengths[OUTPUT]
		) * NUMSTRLEN
	;

	int r = rand() % c_length;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net1->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net1->lengthpairs[i][SECOND]; k ++)
				for(l = 0; l < NUMSTRLEN; l ++, ctr ++)
					if(ctr <= r)
						outnet->c_weights[i][j][k][l] =
							net1->c_weights[i][j][k][l];
					else
						outnet->c_weights[i][j][k][l] =
							net2->c_weights[i][j][k][l];

	stodnet(outnet);
	return;
}


void sortnets(ElmanPtr *nets){
	return;
}

int comparenets(const void *net1, const void *net2){
	ElmanPtr *n1 = (ElmanPtr *) net1;
	ElmanPtr *n2 = (ElmanPtr *) net2;
	double a = (*n1)->fitness;
	double b = (*n2)->fitness;
	if(a > b)
		return 1;
	else if(a < b)
		return -1;
	else
		return 0;
}

double fitness(ElmanPtr net, DatasetPtr data){
	int i, j;
	double
		actual, expected,
		error = 0
	;

	clearnet(net);
	for(i = 0; i < data->dataset_l; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i]);
		for(j = 0; j < net->lengths[OUTPUT]; j ++){
			if(i >= 5){
				actual = sigmoid(net->neurons[OUTPUT][j]);
				expected = data->dataset[i][net->lengths[INPUT] + j];
				error += pow(actual - expected, 2);
			}
		}
	}

	net->fitness = error;
	return error;
}

void netoutput(ElmanPtr net, double *io){
	int i, j;
	for(i = 0; i < net->lengths[INPUT]; i ++)
		net->neurons[INPUT][i] = io[i];
/*
	for(i = 0; i < 4; i ++){
		printf("i %d\nfirst %d\nsecond %d\n_\n", i, net->lengthpairs[i][FIRST], net->lengthpairs[i][SECOND]);
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++){
				net->neurons[  net->lengthpairs[i][SECOND]  ][k] +=
					net->weights[i][j][k] *
						sigmoid(net->neurons[  net->lengthpairs[i][FIRST]  ][j]);
			}
	}
*/
	for(i = 0; i < net->lengths[INPUT]; i ++)
		for(j = 0; j < net->lengths[HIDDEN]; j ++)
			net->neurons[HIDDEN][j] +=
				net->weights[INPUT_HIDDEN][i][j] *
					sigmoid(net->neurons[INPUT][i]);

	for(i = 0; i < net->lengths[CONTEXT]; i ++)
		for(j = 0; j < net->lengths[HIDDEN]; j ++)
			net->neurons[HIDDEN][j] +=
				net->weights[CONTEXT_HIDDEN][i][j] *
					sigmoid(net->neurons[CONTEXT][i]);

	for(i = 0; i < net->lengths[HIDDEN]; i ++)
		for(j = 0; j < net->lengths[CONTEXT]; j ++)
			net->neurons[CONTEXT][j] +=
				net->weights[HIDDEN_CONTEXT][i][j] *
					sigmoid(net->neurons[HIDDEN][i]);

	for(i = 0; i < net->lengths[HIDDEN]; i ++)
		for(j = 0; j < net->lengths[OUTPUT]; j ++)
			net->neurons[OUTPUT][j] +=
				net->weights[HIDDEN_OUTPUT][i][j] *
					sigmoid(net->neurons[HIDDEN][i]);

	return;
}

void clearnet(ElmanPtr net){
	int i, j;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengths[i]; j ++)
			net->neurons[i][j] = 0;
	return;
}

void clearnotctx(ElmanPtr net){
	int i;
	for(i = 0; i < net->lengths[INPUT]; i ++)
		net->neurons[INPUT][i] = 0;
	for(i = 0; i < net->lengths[HIDDEN]; i ++)
		net->neurons[HIDDEN][i] = 0;
	for(i = 0; i < net->lengths[OUTPUT]; i ++)
		net->neurons[OUTPUT][i] = 0;
	return;
}

void printnetio(ElmanPtr net, DatasetPtr data){
	int i, j;
	clearnet(net);
	for(i = 0; i < data->dataset_l; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i]);
		for(j = 0; j < net->lengths[INPUT]; j ++){
			printf("%f ", net->neurons[INPUT][j]);
		}
		for(j = 0; j < net->lengths[OUTPUT]; j ++){
			printf("%f ", sigmoid(net->neurons[OUTPUT][j]));
		}
		printf("\n");
	}
	return;
}

ElmanPtr elmalloc(int input_l, int hidden_l, int context_l, int output_l){
	int i, j, k;
	ElmanPtr net = (ElmanPtr) malloc(sizeof(Elman));

	net->lengths[INPUT] = input_l;
	net->lengths[HIDDEN] = hidden_l;
	net->lengths[CONTEXT] = context_l;
	net->lengths[OUTPUT] = output_l;

	net->lengthpairs[INPUT_HIDDEN][FIRST] = input_l;
	net->lengthpairs[INPUT_HIDDEN][SECOND] = hidden_l;

	net->lengthpairs[CONTEXT_HIDDEN][FIRST] = context_l;
	net->lengthpairs[CONTEXT_HIDDEN][SECOND] = hidden_l;

	net->lengthpairs[HIDDEN_CONTEXT][FIRST] = hidden_l;
	net->lengthpairs[HIDDEN_CONTEXT][SECOND] = context_l;

	net->lengthpairs[HIDDEN_OUTPUT][FIRST] = hidden_l;
	net->lengthpairs[HIDDEN_OUTPUT][SECOND] = output_l;

	for(i = 0; i < 4; i ++){
		net->neurons[i] = (double *) malloc(
			sizeof(double) * net->lengths[i]
		);
		net->weights[i] = (double **) malloc(
			sizeof(double *) * net->lengthpairs[i][FIRST]
		);
		net->c_weights[i] = (char ***) malloc(
			sizeof(char **) * net->lengthpairs[i][FIRST]
		);
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++){
			net->weights[i][j] = (double *) malloc(
				sizeof(double) * net->lengthpairs[i][SECOND]
			);
			net->c_weights[i][j] = (char **) malloc(
				sizeof(char *) * net->lengthpairs[i][SECOND]
			);
			for(k = 0; k < net->lengthpairs[i][SECOND]; k++)
				net->c_weights[i][j][k] = (char *) malloc(
					sizeof(char) * NUMSTRLEN
				);
		}
	}

	return net;
}

void elmfree(ElmanPtr net){
	int i, j, k;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k++)
				free(net->c_weights[i][j][k]);

	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++){
			free(net->weights[i][j]);
			free(net->c_weights[i][j]);
		}

	for(i = 0; i < 4; i ++){
		free(net->neurons[i]);
		free(net->weights[i]);
		free(net->c_weights[i]);
	}
	free(net);

	return;
}

double sigmoid(double x){
	if(x > 0)
		return 1;
	else
		return -1;
}


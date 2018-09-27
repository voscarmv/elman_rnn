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
/*
	char ***c_weights[4];
*/
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
// void stodnet(ElmanPtr net);
double stod(char *s);

void sortnets(ElmanPtr *nets);
int comparenets(const void *net1, const void *net2);

void crossnets(ElmanPtr outnet, ElmanPtr net1, ElmanPtr net2);
void mutanet(ElmanPtr net, int rate);
double bincombine(double f1, double f2);

double fitness(ElmanPtr net, DatasetPtr dataset);
void clearnet(ElmanPtr net);
void clearnotctx(ElmanPtr net);
void netoutput(ElmanPtr net, double *io);
double sigmoid(double x);
void printnetio(ElmanPtr net, DatasetPtr data);
void fprintnetio(char *fname, ElmanPtr *nets, DatasetPtr data, int chosen, int population);
void netcpy(ElmanPtr dest, ElmanPtr src);
double randfloat();
double mutagene(double gene);

void savenet(char *fname, ElmanPtr net);
void loadnet(char *fname, ElmanPtr net);

int main(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	int
		i, j, r1, r2,
		input = 2,
		hidden = 70,
		context = 70,
		output = 1,
		population = 120,
		chosen = 20,
		applefromtree = 2000,
		epochs = 20000,
		expo = 10
	;

	char fname[64];

	DatasetPtr data = datalloc(input + output);
	ElmanPtr *nets = (ElmanPtr *) malloc(sizeof(ElmanPtr) * population);
	ElmanPtr *nets2 = (ElmanPtr *) malloc(sizeof(ElmanPtr) * population);
/**/
		printf("Start:\n");
		fflush(NULL);
		for(i = 0; i < population; i ++){
			nets[i] = elmalloc(input, hidden, context, output);
			nets2[i] = elmalloc(input, hidden, context, output);
			randnet(nets[i]);
			fitness(nets[i], data);
			randnet(nets2[i]);
			fitness(nets2[i], data);
			printf("|");
			fflush(NULL);
		}
		printf("\n");
		fflush(NULL);
		for(j = 0; j < epochs; j ++){
			qsort(nets, population, sizeof(nets[0]), comparenets);
			for(i = population - 1; i >= population - chosen; i --)
				printf("Epoch %d/%d, net %i: %f\n", j + 1, epochs, i, nets[i]->fitness);
			fflush(NULL);
			printf("Saving best.bin...");
			fflush(NULL);
			savenet("best.bin", nets[population - 1]);
			printf(" Saved.\n");
			fflush(NULL);
			sprintf(fname, "epoch%d.dat", j);
			fprintnetio(fname, nets, data, chosen, population);
			printf("Start:\n");
			fflush(NULL);
			for(i = 0; i < ((population - chosen) / 2); i ++){
				r1 = (int) (log(rand() % (int) exp(expo)) * (chosen/expo)) + population - chosen;
				r2 = (int) (log(rand() % (int) exp(expo)) * (chosen/expo)) + population - chosen;
				crossnets(
					nets2[i],
					nets[r1],
					nets[r2]
				);
				mutanet(nets2[i], 100);
				printf("Fitness %d, %d, %d: %f\n", i, r1, r2, fitness(nets2[i], data));
				fflush(NULL);
			}
			for(; i < (population - chosen); i ++){
				r1 = (int) (log(rand() % (int) exp(expo)) * (population/expo));
				r2 = (int) (log(rand() % (int) exp(expo)) * (population/expo));
				crossnets(
					nets2[i],
					nets[r1],
					nets[r2]
				);
				mutanet(nets2[i], 100);
				printf("Fitness %d, %d, %d: %f\n", i, r1, r2, fitness(nets2[i], data));
				fflush(NULL);
			}
			for(; i < population; i ++){
				netcpy(nets2[i], nets[i]);
			}
			for(i = 0; i < population; i ++){
				netcpy(nets[i], nets2[i]);
			}
		}
/**/
	for(i = 0; i < population; i ++)
		elmfree(nets2[i]);
	for(i = 0; i < population; i ++)
		elmfree(nets[i]);
	datfree(data);
	return 0;
}

void netcpy(ElmanPtr dest, ElmanPtr src){
	int i, j, k, l;
/*
	IMPROVE
	Change all code to use contiguous 1D array accesible by
		c_weights[i][j * width * height + k * width + l];
	This will make it possible to use
		memcpy(no, n1, sizeof(char) * r)
		memcpy(no + r, n1 + r, sizeof(char) * (c_length - r)
*/

	for(i = 0; i < 4; i ++)
		for(j = 0; j < dest->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < dest->lengthpairs[i][SECOND]; k ++)
				dest->weights[i][j][k] = src->weights[i][j][k];
	dest->fitness = src->fitness;

	return;
}

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

void fprintnetio(char *fname, ElmanPtr *nets, DatasetPtr data, int chosen, int population){
	int i, j, k;
	FILE *fp = fopen(fname, "w");
	for(j = population - 1; j > population - chosen; j --)
		clearnet(nets[j]);
	for(i = 0; i < data->dataset_l; i ++){
		for(
			j = nets[0]->lengths[INPUT];
			j < nets[0]->lengths[INPUT] + nets[0]->lengths[OUTPUT];
			j ++
		)
			fprintf(fp, "%f ", data->dataset[i][j]);
		for(j = population - 1; j > population - chosen; j --){
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
				net->weights[i][j][k] = randfloat();
/*
				randstr(net->c_weights[i][j][k]);
*/
	return;
}

double randfloat(){
	double out;
	char numstring[NUMSTRLEN];
	numstring[0] = randsign();
	numstring[1] = randrange(6);
	numstring[2] = '.';
	numstring[3] = randrange(6);

	int i;
	for(i = 4; i < NUMSTRLEN - 1; i ++)
		numstring[i] = randrange(10);

	numstring[NUMSTRLEN - 1] = '\0';
	return atof(numstring);
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
/*
void stodnet(ElmanPtr net){
	int i, j, k;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k ++)
				net->weights[i][j][k] = stod(net->c_weights[i][j][k]);
	return;
}
*/
double stod(char *s){
	return ((double) atoi(s)) / ((double) pow(10, NUMSTRLEN - 3));
}
/*
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
*/
void mutanet(ElmanPtr net, int rate){
	int i, j, k, l;
		for(i = 0; i < 4; i ++)
			for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
				for(k = 0; k < net->lengthpairs[i][SECOND]; k ++)
					if(!(rand()%rate))
						net->weights[i][j][k] = mutagene(net->weights[i][j][k]);

	return;
}

double mutagene(double gene){
	double factor1 = (double) (rand() % 1751) / 1000;
	if(rand() % 2)
		factor1 *= -1;
	double factor2 = (double) (rand() % 1751) / 1000;
	if(rand() % 2)
		factor2 *= -1;

	return (gene * factor1) + (gene * factor1 * factor2);
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
		length =
			net1->lengths[INPUT] * net1->lengths[HIDDEN] +
			net1->lengths[HIDDEN] * net1->lengths[CONTEXT] +
			net1->lengths[CONTEXT] * net1->lengths[HIDDEN] +
			net1->lengths[HIDDEN] * net1->lengths[OUTPUT]
	;

	int r = rand() % length;
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net1->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net1->lengthpairs[i][SECOND]; k ++, ctr++)
					if(ctr < r)
						outnet->weights[i][j][k] = net1->weights[i][j][k];
					else if(ctr == r)
						outnet->weights[i][j][k] =
							bincombine(
								net1->weights[i][j][k],
								net2->weights[i][j][j]
							);
					else
						outnet->weights[i][j][k] = net2->weights[i][j][k];


	return;
}

double bincombine(double f1, double f2){
	int j;
	char str1[64], str2[64], str3[64];

	if(f1 >= 0){
		str1[0] = '+';
		sprintf(str1 + 1, "%.8f", f1);
	} else
		sprintf(str1, "%.8f", f1);

	if(f2 >= 0){
		str2[0] = '+';
		sprintf(str1 + 1, "%.8f", f2);
	} else
		sprintf(str2, "%.8f", f2);

	int half = NUMSTRLEN - (rand() % (NUMSTRLEN + 1));
	for(j = 0; j < half; j ++)
		str3[j] = str1[j];
	for(;j < NUMSTRLEN; j ++)
		str3[j] = str2[j];

	return atof(str3);
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
		return -1;
	else if(a < b)
		return 1;
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
			actual = sigmoid(net->neurons[OUTPUT][j]);
			expected = data->dataset[i][net->lengths[INPUT] + j];
			error += fabs(actual - expected);
		}
	}
	net->fitness = error;

	return error;
}

void netoutput(ElmanPtr net, double *io){
	int i, j;
	for(i = 0; i < net->lengths[INPUT]; i ++)
		net->neurons[INPUT][i] = io[i];

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

	net->lengthpairs[HIDDEN_CONTEXT][FIRST] = hidden_l;
	net->lengthpairs[HIDDEN_CONTEXT][SECOND] = context_l;

	net->lengthpairs[CONTEXT_HIDDEN][FIRST] = context_l;
	net->lengthpairs[CONTEXT_HIDDEN][SECOND] = hidden_l;

	net->lengthpairs[HIDDEN_OUTPUT][FIRST] = hidden_l;
	net->lengthpairs[HIDDEN_OUTPUT][SECOND] = output_l;

	for(i = 0; i < 4; i ++){
		net->neurons[i] = (double *) malloc(
			sizeof(double) * net->lengths[i]
		);
		net->weights[i] = (double **) malloc(
			sizeof(double *) * net->lengthpairs[i][FIRST]
		);
/*
		net->c_weights[i] = (char ***) malloc(
			sizeof(char **) * net->lengthpairs[i][FIRST]
		);
*/
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++){
			net->weights[i][j] = (double *) malloc(
				sizeof(double) * net->lengthpairs[i][SECOND]
			);
/*
			net->c_weights[i][j] = (char **) malloc(
				sizeof(char *) * net->lengthpairs[i][SECOND]
			);
			for(k = 0; k < net->lengthpairs[i][SECOND]; k++)
				net->c_weights[i][j][k] = (char *) malloc(
					sizeof(char) * NUMSTRLEN
				);
*/
		}

	}

	return net;
}

void elmfree(ElmanPtr net){
	int i, j, k;
/*
	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
			for(k = 0; k < net->lengthpairs[i][SECOND]; k++)
				free(net->c_weights[i][j][k]);
*/

	for(i = 0; i < 4; i ++)
		for(j = 0; j < net->lengthpairs[i][FIRST]; j ++){
			free(net->weights[i][j]);
//			free(net->c_weights[i][j]);
		}

	for(i = 0; i < 4; i ++){
		free(net->neurons[i]);
		free(net->weights[i]);
//		free(net->c_weights[i]);
	}
	free(net);

	return;
}

double sigmoid(double x){
	return (2 / (1 + exp(-x))) - 1;
}

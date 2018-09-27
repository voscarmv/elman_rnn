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
// void fprintnetio(char *fname, ElmanPtr *nets, DatasetPtr data, int chosen, int population);
void fprintnetio(char *fname, ElmanPtr net, DatasetPtr data);
void netcpy(ElmanPtr dest, ElmanPtr src);
double randfloat();
double mutagene(double gene);
void disturbnet(ElmanPtr net, double rate);
DatasetPtr datacpy(DatasetPtr ori, int io_l, int from, int length);
int netpredict(ElmanPtr net, DatasetPtr data, int from, int length);
int onepredict(ElmanPtr net, DatasetPtr data, int from, int length);
int zeropredict(ElmanPtr net, DatasetPtr data, int from, int length);
int randpredict(ElmanPtr net, DatasetPtr data, int from, int length);
int kuperinpredict(ElmanPtr net, DatasetPtr data, int from, int length);

int main(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	int
		i, j, k,
		input = 1,
		hidden = 5,
		context = 5,
		output = 1,
		population = 120,
		chosen = 20,
		applefromtree = 500,
		epochs = 20000,
		expo = 10
	;

	char fname[64];

	DatasetPtr data = datalloc(input + output);
//	printf("Length %d\n", data->dataset_l);
	ElmanPtr net = elmalloc(input, hidden, context, output);
/**/

	ElmanPtr disturbednet = elmalloc(input, hidden, context, output);
	double best, better;
	DatasetPtr data2;

	double bench;
	int
		m,
		q = 0,
		netp = 0,
		onep = 0,
		zerop = 0,
		randp = 0,
		kuperinp = 0,
		train = 10,
		success = 0,
		zero31 = 0,
		one31 = 0,
		sxss,
		p 
	;
	for(m = 0; m < 1290; m ++){
		zero31 = 0;
		one31 = 0;
		sxss = 0;
		for(p = 0; p < 31; p ++){
			for(j = 0; j < net->lengthpairs[INPUT_HIDDEN][FIRST]; j ++)
				for(k = 0; k < net->lengthpairs[INPUT_HIDDEN][SECOND]; k ++)
					net->weights[INPUT_HIDDEN][j][k] = 0;

			for(j = 0; j < net->lengthpairs[HIDDEN_OUTPUT][FIRST]; j ++)
				for(k = 0; k < net->lengthpairs[HIDDEN_OUTPUT][SECOND]; k ++)
					net->weights[HIDDEN_OUTPUT][j][k] = 0;

			for(j = 0; j < net->lengthpairs[CONTEXT_HIDDEN][FIRST]; j ++)
				for(k = 0; k < net->lengthpairs[CONTEXT_HIDDEN][SECOND]; k ++)
					net->weights[CONTEXT_HIDDEN][j][k] = 0;

			for(j = 0; j < net->lengthpairs[HIDDEN_CONTEXT][FIRST]; j ++)
				for(k = 0; k < net->lengthpairs[HIDDEN_CONTEXT][SECOND]; k ++)
					net->weights[HIDDEN_CONTEXT][j][k] = 0;

			data2 = datacpy(data, input + output, m, train);
			best = fitness(net, data2);
			bench = best;
	//		printf("Benchmark fitness: %f\n", best);

			for(i = 0; i < 20000 && best > 0; i ++){
				netcpy(disturbednet, net);
				disturbnet(disturbednet, 0.05);
				better = fitness(disturbednet, data2);
				if(better < best){
	//				printf("%d: Found better fitness: %f\n", i, better);
					best = better;
					netcpy(net, disturbednet);
				}
			}
			if(success = netpredict(net, data, m, train + 1)){
				netp ++;
				sxss ++;
			}
			if(randpredict(net, data, m, train))
				randp ++;

			if(success && data->dataset[m + train][1] == -1)
				zero31++;
			if(success && data->dataset[m + train][1] == 1)
				one31++;
			if(!success && data->dataset[m + train][1] == -1)
				one31++;
			if(!success && data->dataset[m + train][1] == 1)
				zero31++;

			printf("%d.%d. netqlty %.2f, netp %.4f, "
				"randp %.4f, sxs %d\n",
				(m+1), (p+1), (bench-best)/bench, (double) netp/(m+1),
				(double) randp/(m+1), success);
			fflush(NULL);
		}
		printf("sxss %d, ones %d, zeros %d\n", sxss, one31, zero31);
	}

//	fprintnetio("netout.dat", net, data2);

/**/
	elmfree(net);
	elmfree(disturbednet);
	datfree(data);
	datfree(data2);
	return 0;
}

int netpredict(ElmanPtr net, DatasetPtr data, int from, int length){
	int i, j, k;
	clearnet(net);
	for(i = 0; i < length; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i + from]);
//		printf("%d. %f %f =? %f\n",
//			i, data->dataset[i + from][0], data->dataset[i + from][1],
//			sigmoid(net->neurons[OUTPUT][0]));
	}

	if(sigmoid(net->neurons[OUTPUT][0]) == data->dataset[from + length][1]){
		return 1;
//		printf("Predict success\n");
	}
	else
		return 0;
}

int onepredict(ElmanPtr net, DatasetPtr data, int from, int length){
	int i, j, k;
/*
	clearnet(net);
	for(i = 0; i < length; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i + from]);
	}
*/
	if(data->dataset[from + length][1] == 1)
		return 1;
	else
		return 0;
}

int zeropredict(ElmanPtr net, DatasetPtr data, int from, int length){
	int i, j, k;
/*
	clearnet(net);
	for(i = 0; i < length; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i + from]);
	}
*/
	if(data->dataset[from + length][1] == -1)
		return 1;
	else
		return 0;
}

int randpredict(ElmanPtr net, DatasetPtr data, int from, int length){
	int i, j, k;
/*
	clearnet(net);
	for(i = 0; i < length; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i + from]);
	}
*/
	int result;
	if(rand() % 2)
		result = -1;
	else
		result = 1;
	if(data->dataset[from + length][1] == result)
		return 1;
	else
		return 0;
}

int kuperinpredict(ElmanPtr net, DatasetPtr data, int from, int length){
	int i, j, k;
/*
	clearnet(net);
	for(i = 0; i < length; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i + from]);
	}
*/
	if(data->dataset[from + length][1] == data->dataset[from + length - 2][1])
		return 1;
	else
		return 0;
}


DatasetPtr datacpy(DatasetPtr ori, int io_l, int from, int length){
	int i, j;
	double **dataset = (double **) malloc(sizeof(double *) * length);
	for(i = 0; i < length; i ++){
		dataset[i] = (double *) malloc(sizeof(double) * io_l);
		for(j = 0; j < io_l; j ++)
			dataset[i][j] = ori->dataset[i + from][j];
	}
	DatasetPtr dest = (DatasetPtr) malloc(sizeof(Dataset));
	dest->dataset_l = length;
	dest->dataset = dataset;
	return dest;
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

void fprintnetio(char *fname, ElmanPtr net, DatasetPtr data){
	int i, j, k;
	FILE *fp = fopen(fname, "w");
	clearnet(net);
	for(i = 0; i < data->dataset_l; i ++){
		clearnotctx(net);
		netoutput(net, data->dataset[i]);
		for(k = 0; k < net->lengths[OUTPUT]; k ++)
			fprintf(fp, "%f\n", sigmoid(net->neurons[OUTPUT][k]));
	}
	fclose(fp);
	return;
}

/*
void fprintnetio(char *fname, ElmanPtr *nets, DatasetPtr data, int chosen, int population){
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
*/

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

void disturbnet(ElmanPtr net, double rate){
	int i, j, k, r;
		for(i = 0; i < 4; i ++)
			for(j = 0; j < net->lengthpairs[i][FIRST]; j ++)
				for(k = 0, r = rand() % 3; k < net->lengthpairs[i][SECOND]; k ++, r = rand() % 3)
					if(r == 0)
						net->weights[i][j][k] += rate;
					else if(r == 1)
						net->weights[i][j][k] -= rate;

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
	if(x > 0)
		return 1;
	else
		return -1;
}

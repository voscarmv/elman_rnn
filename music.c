#include <stdio.h>
#include <stdlib.h>

#define MAXSAMPLES 5000
#define RANDRANGE 20
#define NEUROOUTRANGE 10

typedef struct nodo{
	int sum;
	int cuantos;
	int *pesos;
	struct nodo **vecinos;
	struct nodo *out;
} Nodo, *NodoPtr;

NodoPtr nodalloc(int cuantos, int capas);
void nodfree(NodoPtr net, int cuantos, int capas);
void liberared(NodoPtr net, int cuantos, int capas);

typedef struct dataset{
	int dataset_l;
	int **dataset;
} Dataset, *DatasetPtr;

DatasetPtr datalloc(int io_l);
void datfree(DatasetPtr data);

int printnet(NodoPtr *net, int capas, int cuantos);
int propaganet(NodoPtr *net, int capas, int cuantos);

int borrar(NodoPtr net, int capas, int cuantos);
int borrasums(NodoPtr *net, int capas, int cuantos);

int pulsar(NodoPtr net, int entrada, int capas, int cuantos, int capa_salida);
int netoutput(NodoPtr net, int capa);

NodoPtr netcapa(NodoPtr net, int capa);
int feedback(NodoPtr net, int capa_entrada, int capa_memoria, int capas, int cuantos);

int randopes();

// Hay que darle rango de [255,-255] a rand() para las dendritas y promediar net->sum / cuantos, y alimentar una onda senoidal para hacer
// un entrenamiento de prueba con un algoritmo avaro.

// Despues de eso sigue construir el algoritmo genetico. Se me ocurre que para cada dendrita de un hijo de dos redes A y B
// existan 4 posibilidades. Que la dendrita del hijo termine siendo al azar cualquiera de:
// 	1 dendrita de progenitor A
//	2 dendrita del progenitor B
//	3 mezcla de ambas dendritas (binaria? proedio?)
//	4 nueva dendrita
// Esto para discretizar y para simular un proceso de reproduccion sexual.
//

int main(){
	int cuantos = 5;
	int capas = 6;

	int input = 1;
	int output = 1;

//	DatasetPtr data = datalloc(input + output);
	DatasetPtr data = datalloc(1);

	NodoPtr red = nodalloc(cuantos,capas);

	int i;

//	borrasums(red->vecinos, capas, cuantos);

//printnet(red->vecinos, capas, cuantos);

//	borrar(red, capas, cuantos);

	printnet(red->vecinos, capas, cuantos);
	printf("Direct out: %d\n", red->out->sum);
//	printnet(red->vecinos, capas, cuantos);
	printf("Borrar\n");
	borrar(red, capas, cuantos);

	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 3));
	feedback(red, 2, 5, capas, cuantos);
	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 3));

	printf("Borrar\n");
	borrar(red, capas, cuantos);

	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 3));
	feedback(red, 4, 5, capas, cuantos);
	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 3));

	printf("Borrar\n");
	borrar(red, capas, cuantos);

	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 3));
	//feedback(red, 4, 5, capas, cuantos);
	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 3));



	printf(
		"Outputs \n%d\n%d\n%d\n%d\n", 
		red->sum, 
		red->vecinos[0]->sum, 
		red->vecinos[0]->vecinos[0]->sum, 
		red->vecinos[0]->vecinos[0]->vecinos[0]->sum
	);

	for(i=0;i<data->dataset_l;i++)
		printf("%d\n",data->dataset[i][0]);

	borrar(red, capas, cuantos);
	int salida = pulsar(red, data->dataset[0][0], capas, cuantos, 3);
	feedback(red, 4, 5, capas, cuantos);

	for(i=1;i<data->dataset_l;i++){
		printf("Red: %d, Data: %d\n", salida, data->dataset[i][0]);
		salida = pulsar(red, salida, capas, cuantos, 3);
		feedback(red, 4, 5, capas, cuantos);
	}

	liberared(red, cuantos, capas);
	datfree(data);

	return 0;
}



DatasetPtr datalloc(int io_l){
	int
		i = 0,
		j = 0
	;

	int **dataset = (int **) malloc(sizeof(int *));
	dataset[0] = (int *) malloc(sizeof(int) * io_l);

	while(
		scanf(
			"%d",
			&(dataset[i][j++])
		) != EOF && i < MAXSAMPLES
	){
		if(j > io_l - 1){
			dataset = realloc(dataset, sizeof(int *) * ((++i) + 1));
			dataset[i] = malloc(sizeof(int) * io_l);
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


int printnet(NodoPtr *net, int capas, int cuantos){
	int i,j,k,m,p;
	if(net[0]->vecinos[0]->vecinos != NULL){
				for(m=0;m<cuantos;m++){
					for(p=0;p<cuantos;p++){
						printf("sum %d peso %d\n", net[m]->sum, net[m]->pesos[p]);
//						net[m]->vecinos[p]->sum += net[m]->sum * net[m]->pesos[p];
					}
				}	

					printf("\n-\n");

		printnet(net[0]->vecinos, capas, cuantos);
	} else {
		for(m=0;m<cuantos;m++){
			printf("sum %d peso ultimo %d\n", net[m]->sum, net[m]->pesos[0]);
//			net[m]->vecinos[0]->sum += net[m]->sum * net[m]->pesos[0];
		}
		printf("Salida de la red: %d\n", net[0]->vecinos[0]->sum);
	}
	return 0;
}

NodoPtr malla(NodoPtr *net, int capas, int cuantos){
	int i,j,k,m;
	if(--capas==0){
		NodoPtr *net2 = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);
		for(j=0;j<cuantos;j++){
			net2[j] = (NodoPtr) malloc(sizeof(Nodo));

			NodoPtr *nodarray3 = (NodoPtr *) malloc(sizeof(NodoPtr) * 1);
			int *pesosarray3 = (int *) malloc(sizeof(int) * 1);
		
			net2[j]->vecinos = nodarray3;
			net2[j]->pesos = pesosarray3;
			net2[j]->sum = 0;
		}

		NodoPtr out = (NodoPtr) malloc(sizeof(Nodo));
		out->vecinos = NULL;
		out->sum = 0;
			
		for(j=0;j<cuantos;j++){
			net2[j]->vecinos[0] = out;
			net2[j]->pesos[0] = randopes();
		}


		for(j=0;j<cuantos;j++)
			for(m=0;m<cuantos;m++)
				net[j]->vecinos[m] = net2[m];

		return out;
	}  else {


		NodoPtr *net2 = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);
		for(j=0;j<cuantos;j++){
			net2[j] = (NodoPtr) malloc(sizeof(Nodo));

			NodoPtr *nodarray3 = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);
			int *pesosarray3 = (int *) malloc(sizeof(int) * cuantos);
		
			net2[j]->vecinos = nodarray3;
			net2[j]->pesos = pesosarray3;
			net2[j]->sum=0;
		}

		for(j=0;j<cuantos;j++)
			for(int n = 0; n < cuantos ; n ++)
				net2[j]->pesos[n] = randopes();
	
		for(j=0;j<cuantos;j++)
			for(m=0;m<cuantos;m++)
				net[j]->vecinos[m] = net2[m];

		malla(net2, capas, cuantos);
	} 
}

NodoPtr nodalloc(int cuantos, int capas){
	int max_node = cuantos;

	int i, j, k;
	NodoPtr net = (NodoPtr) malloc(sizeof(Nodo));
	net->sum = 0; // entrada de la red

	NodoPtr *nodarray = (NodoPtr *) malloc(sizeof(NodoPtr) * max_node);
	int *pesosarray = (int *) malloc(sizeof(int) * max_node);

	for(i=0;i<cuantos;i++){
		nodarray[i] = (NodoPtr) malloc(sizeof(Nodo));

		NodoPtr *nodarray2 = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);
		int *pesosarray2 = (int *) malloc(sizeof(int) * cuantos);
	
		nodarray[i]->vecinos = nodarray2;
		nodarray[i]->pesos = pesosarray2;
		nodarray[i]->sum = 0;
	}

	for(j=0;j<cuantos;j++)
		for(k=0;k<cuantos;k++)
			nodarray[j]->pesos[k] = randopes();

	net->vecinos = nodarray;
	net->pesos = pesosarray;


	for(j=0;j<cuantos;j++)
		net->pesos[j] = randopes();

	for(j=0;j<cuantos;j++)
		net->vecinos[j]->sum += net->sum * net->pesos[j];



	NodoPtr out = malla( net->vecinos, capas, cuantos);

	net->out = out;

	return net;
}

int pulsar(NodoPtr net, int entrada, int capas, int cuantos, int capa_salida){
	int i,j,k,m,p;

	net->sum = entrada;

	for(j=0;j<cuantos;j++)
		net->vecinos[j]->sum = net->sum * net->pesos[j];

	 propaganet(net->vecinos, capas, cuantos);

	//return net->out->sum;
	return netoutput(net, capa_salida);
}

int netoutput(NodoPtr net, int capa){
	if(capa > 0)
		return netoutput(net->vecinos[0], --capa);
	else
		return net->sum;
}

NodoPtr netcapa(NodoPtr net, int capa){
	if(capa > 0)
		return netcapa(net->vecinos[0], --capa);
	else
		return net;
}

int feedback(NodoPtr net, int capa_entrada, int capa_memoria, int capas, int cuantos){
	NodoPtr capa_in = netcapa(net, capa_entrada);
	NodoPtr capa_mem = netcapa(net, capa_memoria);

	int i,j,k;

//	for(i=0;i<cuantos;i++)
//		capa_in->vecinos[i]->sum *= cuantos;

	for(i=0;i<cuantos;i++)
		for(j=0;j<cuantos;j++)
			capa_in->vecinos[i]->sum += capa_mem->vecinos[j]->pesos[0] * capa_mem->vecinos[j]->sum;

	for(i=0;i<cuantos;i++)
		capa_in->vecinos[i]->sum %= NEUROOUTRANGE;
//		capa_in->vecinos[i]->sum /= (cuantos * 2);

	
	return 0;
}

int propaganet(NodoPtr *net, int capas, int cuantos){
	int i,j,k,m,p;
	if(net[0]->vecinos[0]->vecinos != NULL){
		for(m=0;m<cuantos;m++)
			for(p=0;p<cuantos;p++)
				net[m]->vecinos[p]->sum += net[m]->sum * net[m]->pesos[p];
		for(m=0;m<cuantos;m++)
			net[0]->vecinos[m]->sum %= NEUROOUTRANGE;
//			net[0]->vecinos[m]->sum /= cuantos;
		propaganet(net[0]->vecinos, capas, cuantos);
	} else {
		for(m=0;m<cuantos;m++)
			net[m]->vecinos[0]->sum += net[m]->sum * net[m]->pesos[0];
	}
	return 0;
}

int borrar(NodoPtr net, int capas, int cuantos){
	net->sum = 0;
	for(int j=0;j<cuantos;j++)
		net->vecinos[j]->sum = 0;

	borrasums(net->vecinos, capas, cuantos);
	return 0;	
}

int borrasums(NodoPtr *net, int capas, int cuantos){
	int i,j,k,m,p;
	if(net[0]->vecinos[0]->vecinos != NULL){
		for(p=0;p<cuantos;p++)
		net[0]->vecinos[p]->sum = 0;
		borrasums(net[0]->vecinos, capas, cuantos);
	} else {
		net[0]->vecinos[0]->sum = 0;
	}
	return 0;
}

int randopes(){
	int range = RANDRANGE;
	return rand() % (range * 2) - range;
}

void datfree(DatasetPtr data){
	int i;
	for(i = 0; i < data->dataset_l; i ++)
		free(data->dataset[i]);
	free(data->dataset);
	free(data);
	return;
}

void liberared(NodoPtr net, int cuantos, int capas){
	nodfree(net, cuantos, capas);
	free(net->pesos);
	free(net->vecinos);
	free(net->out);
	free(net);

	return;
}

void nodfree(NodoPtr net, int cuantos, int capas){
	int i;

	if(capas > 0){
		nodfree(net->vecinos[0], cuantos, --capas);
		for(i=0;i<cuantos;i++){
			free(net->vecinos[i]->pesos);
			free(net->vecinos[i]->vecinos);
			free(net->vecinos[i]);
		}
	} else {
		free(net->vecinos[0]->pesos);
		free(net->vecinos[0]->vecinos);
		free(net->vecinos[0]);
	}
	return;
}

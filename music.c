#include <stdio.h>
#include <stdlib.h>

#define MAXSAMPLES 5000
#define RANDRANGE 1000
#define NEUROOUTRANGE 10

typedef struct nodo{
	int fitness;
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

// La red ya tiene un rango flexible. Se asigna en NEUROOUTRANGE
// El rango permitido para los pesos de las dendritas es RANDRANGE
// El maximo de datos para el dataset de entrada es MAXSAMPLES

// El codigo actual entrega una lista de fitnesses de 10000 redes neuronales.
// Falta:
//	1 Escribir la funcion copynet para poder generar un set de redes hijas
//	2 Escribir la funcion sexnet para combinar dos redes progenitoras
//	3 Portar el qsort de elmanx.c aqui para ordenar a las redes hijas de acuerdo a su fitness

/*

qsort(nets, population, sizeof(nets[0]), comparenets);

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

*/

int main(){

	int i,j;

	int cuantos = 18; // cantidad de neuronas por cada capa
	int capas = 10; // cantidad de capas
	int capa_salida = 5; // capa cuya primer neurona representa la salida

	int capa_entrada = 3; // capa a donde ingresan los datos de la malla de memoria
	int capa_memoria = 9; // capa de la que salen los datos de la malla de memoria

	if(capa_salida > capas)
		return 1;
	if(capa_entrada > capa_salida)
		return 2;
	if(capa_memoria > capa)
		return 3;
	if(capa_memoria <= capa_entrada)
		return 4;

	int poblacion = 10000;

	int input = 1;
	int output = 1;

//	DatasetPtr data = datalloc(input + output);
	DatasetPtr data = datalloc(1);

	NodoPtr *redes = (NodoPtr *) malloc(sizeof(NodoPtr) * poblacion);

	for(i=0;i<poblacion;i++)
		redes[i] = nodalloc(cuantos,capas);

//	for(i=0;i<data->dataset_l;i++)
//		printf("%d\n",data->dataset[i][0]);

	for(i=0;i<poblacion;i++){

		borrar(redes[i], capas, cuantos);
		int salida = pulsar(redes[i], data->dataset[0][0], capas, cuantos, capa_salida);
		feedback(redes[i], capa_entrada, capa_memoria, capas, cuantos);

		int fitness = 0;

		for(j=1;j<data->dataset_l;j++){
//			printf("Red: %d, Data: %d\n", salida, data->dataset[j][0]);
			int distancia = salida - data->dataset[j][0];
			if(distancia < 0)
				distancia *= -1;
			fitness += distancia;
			salida = pulsar(redes[i], salida, capas, cuantos, capa_salida);
			feedback(redes[i], capa_entrada, capa_memoria, capas, cuantos);
		}
		printf("%d\n", fitness);
	}

	for(i=0;i<poblacion;i++)
		liberared(redes[i], cuantos, capas);
	free(redes);
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

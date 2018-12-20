#include <stdio.h>
#include <stdlib.h>

#define MAXSAMPLES 5000
#define RANDRANGE 30
#define NEUROOUTRANGE 10

/*

BIG MEMORY LEAK

==1808== 
==1808== HEAP SUMMARY:
==1808==     in use at exit: 60 bytes in 4 blocks
==1808==   total heap usage: 55 allocs, 51 frees, 7,124 bytes allocated
==1808== 
==1808== Searching for pointers to 4 not-freed blocks
==1808== Checked 69,376 bytes
==1808== 
==1808== 4 bytes in 1 blocks are definitely lost in loss record 1 of 4
==1808==    at 0x4C2BBAF: malloc (vg_replace_malloc.c:299)
==1808==    by 0x108DF3: malla (music.c:442)
==1808==    by 0x10936A: nodalloc (music.c:525)
==1808==    by 0x108A05: main (music.c:160)
==1808== 
==1808== 8 bytes in 1 blocks are definitely lost in loss record 2 of 4
==1808==    at 0x4C2BBAF: malloc (vg_replace_malloc.c:299)
==1808==    by 0x108DA9: malla (music.c:437)
==1808==    by 0x10936A: nodalloc (music.c:525)
==1808==    by 0x108A05: main (music.c:160)
==1808== 
==1808== 48 (8 direct, 40 indirect) bytes in 1 blocks are definitely lost in loss record 4 of 4
==1808==    at 0x4C2BBAF: malloc (vg_replace_malloc.c:299)
==1808==    by 0x108DE5: malla (music.c:441)
==1808==    by 0x10936A: nodalloc (music.c:525)
==1808==    by 0x108A05: main (music.c:160)
==1808== 
==1808== LEAK SUMMARY:
==1808==    definitely lost: 20 bytes in 3 blocks
==1808==    indirectly lost: 40 bytes in 1 blocks
==1808==      possibly lost: 0 bytes in 0 blocks
==1808==    still reachable: 0 bytes in 0 blocks
==1808==         suppressed: 0 bytes in 0 blocks
==1808== 
==1808== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
==1808== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)


*/

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

NodoPtr netsex(NodoPtr mom, NodoPtr dad, int cuantos, int capas);
int sex(NodoPtr *hijo, NodoPtr *mom, NodoPtr *dad, int capas, int cuantos);

NodoPtr netclone(NodoPtr dad, int cuantos, int capas);
int clon(NodoPtr *hijo, NodoPtr *dad, int capas, int cuantos);

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

int comparenets(const void *net1, const void *net2);

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

int main(int argc, char **argv){
	if(argc>0){
		int seed = atoi(argv[1]);
		printf("seed %d\n",seed);

		srand(seed);
	}

	int i,j;

	int cuantos = 10; // cantidad de neuronas por cada capa
	int capas = 10; // cantidad de capas
//	int cuantos = 1, capas = 1;
	int capa_salida = 5; // capa cuya primer neurona representa la salida

	int capa_entrada = 3; // capa a donde ingresan los datos de la malla de memoria
	int capa_memoria = 9; // capa de la que salen los datos de la malla de memoria

	if(capa_salida > capas)
		return 1;
	if(capa_entrada > capa_salida)
		return 2;
	if(capa_memoria > capas)
		return 3;
	if(capa_memoria <= capa_entrada)
		return 4;

//	cuantos = 2;
//	capas = 1;

	int poblacion = 3;

	int input = 1;
	int output = 1;

//	DatasetPtr data = datalloc(input + output);
	DatasetPtr data = datalloc(1);

	NodoPtr *redes = (NodoPtr *) malloc(sizeof(NodoPtr) * poblacion);
//	NodoPtr *redes = (NodoPtr *) malloc(sizeof(NodoPtr) * 3);



//	redes[0] = nodalloc(9,7);

//	free(redes[0]->vecinos[0]->vecinos[0]);
//	free(redes[0]->vecinos[0]->vecinos[0]->vecinos[0]->vecinos[0]);

	   //redes[0]->vecinos[0]->vecinos[0]->vecinos[0]->sum);
/*
	free(redes[0]->vecinos[0]->vecinos[0]->vecinos[0]->pesos);
	free(redes[0]->vecinos[0]->vecinos[0]->vecinos[0]->vecinos);
-	free(redes[0]->vecinos[0]->vecinos[0]->vecinos[0]);

	free(redes[0]->vecinos[0]->vecinos[0]->pesos);
	free(redes[0]->vecinos[0]->vecinos[0]->vecinos);
	free(redes[0]->vecinos[0]->vecinos[0]);

	free(redes[0]->vecinos[0]->vecinos[1]->pesos);
	free(redes[0]->vecinos[0]->vecinos[1]->vecinos);
	free(redes[0]->vecinos[0]->vecinos[1]);

	free(redes[0]->vecinos[0]->pesos);
	free(redes[0]->vecinos[0]->vecinos);
	free(redes[0]->vecinos[0]);

	free(redes[0]->vecinos[1]->pesos);
	free(redes[0]->vecinos[1]->vecinos);
	free(redes[0]->vecinos[1]);

	

	free(redes[0]->pesos);
	free(redes[0]->vecinos);
//	free(redes[0]->out);
*/	
//	liberared(redes[0], 9, 7);
//	free(redes[0]);

//	free(redes);

//	datfree(data);	

//	return 0;


//	redes[0] = (NodoPtr) malloc(sizeof(Nodo));

//	struct nodo **vecs = (NodoPtr *) malloc(sizeof(NodoPtr) * 2);
//	redes[0]->vecinos = vecs;


//NodoPtr *nodarray2 = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);

//	free(redes[0]->vecinos);

//redes[0]->vecinos = nodarray2;
//	free(redes[0]->vecinos);


	//free(nodarray2);

//	int *pesosarray = (int *) malloc(sizeof(int) * 1);
//	redes[0]->pesos = pesosarray;

//	free(redes[0]->pesos);



//	free(redes[0]);



//	for(i=0;i<poblacion;i++)
//		liberared(redes[i], cuantos, capas);

//	redes[0] = nodalloc(2,2);
//	redes[1] = nodalloc(3,3);


//	free(redes[0]->vecinos[0]->vecinos[0]->vecinos);
//	free(redes[0]->vecinos[1]->vecinos[0]->pesos);

//	free(redes[0]->vecinos[0]->pesos);
//	free(redes[0]->vecinos[1]->pesos);


//	free(redes[0]->vecinos[0]->vecinos);
//	free(redes[0]->vecinos[1]->vecinos);

//	free(redes[0]->vecinos[0]->pesos);
//	free(redes[0]->vecinos[1]->pesos);

//	free(redes[0]->vecinos);
//	free(redes[0]->pesos);

//	free(redes);

//	datfree(data);

//	for(i=0;i<poblacion;i++)
//		liberared(redes[i], cuantos, capas);
//	free(redes);



	for(i=0;i<poblacion;i++)
		redes[i] = nodalloc(cuantos,capas);


/*

	for(i=0;i<poblacion;i++){
		liberared(redes[i], cuantos, capas);
		free(redes[i]);
	}

	free(redes);
	datfree(data);

	return 0;
*/



	
//return 0;
//	for(i=0;i<data->dataset_l;i++)
//		printf("%d\n",data->dataset[i][0]);
NodoPtr *bebes;
int bestanterior = -1;

for(int x=0;x<2;x++){
	for(i=0;i<poblacion;i++){

		borrar(redes[i], capas, cuantos);
		int salida = pulsar(redes[i], data->dataset[0][0], capas, cuantos, capa_salida)+10;
		feedback(redes[i], capa_entrada, capa_memoria, capas, cuantos);

		int fitness = 0;

		for(j=1;j<data->dataset_l;j++){
//			printf("Red: %d, Data: %d\n", salida, data->dataset[j][0]);
			int distancia = salida - data->dataset[j][0];
			if(distancia < 0)
				distancia *= -1;
			fitness += distancia;
			salida = pulsar(redes[i], salida, capas, cuantos, capa_salida)+10;
			feedback(redes[i], capa_entrada, capa_memoria, capas, cuantos);
		}
		redes[i]->fitness = fitness;
//		printf("%d\n", fitness);
	}

	qsort(redes, poblacion, sizeof(redes[0]), comparenets);

	if(bestanterior == -1)
		bestanterior = redes[0]->fitness;
	if(redes[0]->fitness < bestanterior){
		borrar(redes[0], capas, cuantos);
		int salida = pulsar(redes[0], data->dataset[0][0], capas, cuantos, capa_salida)+10;
		feedback(redes[0], capa_entrada, capa_memoria, capas, cuantos);

		for(j=1;j<data->dataset_l;j++){
			printf("Red: %d, Data: %d\n", salida, data->dataset[j][0]);
			salida = pulsar(redes[0], salida, capas, cuantos, capa_salida)+10;
			feedback(redes[0], capa_entrada, capa_memoria, capas, cuantos);
		}
		bestanterior = redes[0]->fitness;
	}

//	for(i=0;i<poblacion;i++)
//		printf("%d\n", redes[i]->fitness);
		printf("%d: %d\n", x, redes[0]->fitness);
//	printf("---\n");
	
	bebes = (NodoPtr *) malloc(sizeof(NodoPtr) * poblacion);

	for(j=0;j<poblacion;j++){
		int choose_dad = rand() % ((poblacion*(poblacion+1)) / 2);
		int choose_mom = rand() % ((poblacion*(poblacion+1)) / 2);
		NodoPtr papa = NULL;
		NodoPtr mama = NULL;

		int ctaregresiv = 0;
		for(i=0;i<poblacion;i++){
			ctaregresiv += poblacion - i;
//			printf("%d %d %d %d ",redes[i]->fitness, ctaregresiv, choose_dad, choose_mom);
			if(ctaregresiv > choose_dad){
//				printf("papa ");
				papa = redes[i];
			}
			if(ctaregresiv > choose_mom){
//				printf("mama ");
				mama = redes[i];
			}
			if(papa != NULL && mama != NULL)
				break;
//			printf("\n");
		}
//			printf("\n");
//		if(j=0){
//			bebes[j] = netclone(redes[0], cuantos, capas);
//		} else { 
			papa = redes[0]; // el papa siempre va a ser la mejor red
			bebes[j] = netsex(mama, papa, cuantos, capas);
//		}
	}

//	printf("%d\n",bebes[1]->pesos[0]);
//return 0;

//	for(i = 0; i < poblacion/3; i++){
//		liberared(bebes[i], cuantos, capas);
//		free(bebes[i]);
//		NodoPtr clon = netclone(redes[i], cuantos, capas);
//		bebes[i] = clon;
//	}

	for(i=0;i<poblacion;i++){
		liberared(redes[i], cuantos, capas);
		free(redes[i]);
	}
	free(redes);

	redes = bebes;

		fflush(NULL);

}
//////////////////////////////////////////////////////////
/*	for(i=0;i<poblacion;i++){

		borrar(redes[i], capas, cuantos);
		int salida = pulsar(redes[i], data->dataset[0][0], capas, cuantos, capa_salida);
		feedback(redes[i], capa_entrada, capa_memoria, capas, cuantos);

		int fitness = 0;

		for(j=1;j<data->dataset_l;j++){
//			printf("Red: %d, Data: %d\n", salida, data->dataset[j][0]);
			int distancia = salida - data->dataset[j][0];
			if(distancia < 0)
*				distancia *= -1;
			fitness += distancia;
			salida = pulsar(redes[i], salida, capas, cuantos, capa_salida);
			feedback(redes[i], capa_entrada, capa_memoria, capas, cuantos);
		}
		redes[i]->fitness = fitness;
//		printf("%d\n", fitness);
	}

	qsort(redes, poblacion, sizeof(redes[0]), comparenets);

	for(i=0;i<poblacion;i++)
		printf("%d\n", redes[i]->fitness);
	printf("---\n");
*/

	for(i=0;i<poblacion;i++){
		liberared(redes[i], cuantos, capas);
		free(redes[i]);
	}
	free(redes);
//	for(i=0;i<poblacion;i++){
//		liberared(bebes[i], cuantos, capas);
//		free(bebes[i]);
//	}
//	free(bebes);
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

		//NodoPtr *net2 = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);

		for(j=0;j<cuantos;j++){
			net[0]->vecinos[j] = (NodoPtr) malloc(sizeof(Nodo));

//			NodoPtr *nodarray3 = (NodoPtr *) malloc(sizeof(NodoPtr) * 1);
//			int *pesosarray3 = (int *) malloc(sizeof(int) * 1);
		
			net[0]->vecinos[j]->vecinos = (NodoPtr *) malloc(sizeof(NodoPtr) * 1);
			net[0]->vecinos[j]->pesos = (int *) malloc(sizeof(int) * 1);
			net[0]->vecinos[j]->sum = 0;
		}

		NodoPtr out = (NodoPtr) malloc(sizeof(Nodo));
		out->vecinos = NULL;
		out->sum = 0;

		for(j=1;j<cuantos;j++)
			for(m=0;m<cuantos;m++)
				net[j]->vecinos[m] = net[0]->vecinos[m];

		for(j=0;j<cuantos;j++){
			net[0]->vecinos[j]->vecinos[0] = out;
			net[0]->vecinos[j]->pesos[0] = randopes();
		}

//		free(net2);
		return out;
	}  else {


//		NodoPtr *net2 = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);
		for(j=0;j<cuantos;j++){
			net[0]->vecinos[j] = (NodoPtr) malloc(sizeof(Nodo));

//			NodoPtr *nodarray3 = (NodoPtr *) malloc(sizeof(NodoPtr) * 1);
//			int *pesosarray3 = (int *) malloc(sizeof(int) * 1);
		
			net[0]->vecinos[j]->vecinos = (NodoPtr *) malloc(sizeof(NodoPtr) * cuantos);
			net[0]->vecinos[j]->pesos = (int *) malloc(sizeof(int) * cuantos);
			net[0]->vecinos[j]->sum = 0;
		}

		for(j=0;j<cuantos;j++)
			for(int n = 0; n < cuantos ; n ++)
				net[0]->vecinos[j]->pesos[n] = randopes();
	
		for(j=1;j<cuantos;j++)
			for(m=0;m<cuantos;m++)
				net[j]->vecinos[m] = net[0]->vecinos[m];
//free(net2);
		malla(net[0]->vecinos, capas, cuantos);
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

	NodoPtr outs = malla( net->vecinos, capas, cuantos);

	net->out = outs;

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
//	free(net);

	return;
}

void nodfree(NodoPtr net, int cuantos, int capas){
	int i;

	if(capas > 0){
//		printf("capassssss         %d\n",capas);

		nodfree(net->vecinos[0], cuantos, --capas);
		for(i=0;i<cuantos;i++){
			free(net->vecinos[i]->pesos);
			free(net->vecinos[i]->vecinos);
			free(net->vecinos[i]);
		}
	} else {
//		printf("%d\n", net->vecinos[0]->sum);
		for(i=0;i<cuantos;i++){
			free(net->vecinos[i]->pesos);
			free(net->vecinos[i]->vecinos);
			free(net->vecinos[i]);
		}
	}
	return;
}

int comparenets(const void *net1, const void *net2){
	NodoPtr *n1 = (NodoPtr *) net1;
	NodoPtr *n2 = (NodoPtr *) net2;
	double a = (*n1)->fitness;
	double b = (*n2)->fitness;
	if(a > b)
		return 1;
	else if(a < b)
		return -1;
	else
		return 0;
}

NodoPtr netsex(NodoPtr mom, NodoPtr dad, int cuantos, int capas){
	NodoPtr hijo = nodalloc(cuantos, capas);
//	net->sum = 0;
	for(int j=0;j<cuantos;j++){
		int god = rand() % 4;
		if(god == 0)
			hijo->pesos[j] = randopes();
		if(god == 1)
			hijo->pesos[j] = mom->pesos[j];
		if(god == 2)
			hijo->pesos[j] = dad->pesos[j];
		if(god == 3)
			hijo->pesos[j] = (mom->pesos[j] + dad->pesos[j])/2;
	}

	sex(hijo->vecinos, mom->vecinos, dad->vecinos, capas, cuantos);
	return hijo;
}

int sex(NodoPtr *hijo, NodoPtr *mom, NodoPtr *dad, int capas, int cuantos){
	int i,j,k,m,p;
	if(hijo[0]->vecinos[0]->vecinos != NULL){
		for(i=0;i<cuantos;i++)
			for(j=0;j<cuantos;j++){
				int god = rand() % 4;
				if(god == 0)
					hijo[i]->pesos[j] = randopes();
				if(god == 1)
					hijo[i]->pesos[j] = mom[i]->pesos[j];
				if(god == 2)
					hijo[i]->pesos[j] = dad[i]->pesos[j];
				if(god == 3)
					hijo[i]->pesos[j] = (mom[i]->pesos[j] + dad[i]->pesos[j])/2;
			}
		sex(hijo[0]->vecinos, mom[0]->vecinos, dad[0]->vecinos, capas, cuantos);
	} else {
		for(i=0;i<cuantos;i++){
			int god = rand() % 4;
			if(god == 0)
				hijo[i]->pesos[0] = randopes();
			if(god == 1)
				hijo[i]->pesos[0] = mom[i]->pesos[0];
			if(god == 2)
				hijo[i]->pesos[0] = dad[i]->pesos[0];
			if(god == 3)
				hijo[i]->pesos[0] = (mom[i]->pesos[0] + dad[i]->pesos[0])/2;
		}
	}
	return 0;
}

NodoPtr netclone(NodoPtr dad, int cuantos, int capas){
	NodoPtr hijo = nodalloc(cuantos, capas);
//	net->sum = 0;
	for(int j=0;j<cuantos;j++){
		hijo->pesos[j] = dad->pesos[j];
	}

	clon(hijo->vecinos, dad->vecinos, capas, cuantos);
	return hijo;
}

int clon(NodoPtr *hijo, NodoPtr *dad, int capas, int cuantos){
	int i,j,k,m,p;
	if(hijo[0]->vecinos[0]->vecinos != NULL){
		for(i=0;i<cuantos;i++)
			for(p=0;p<cuantos;p++){
				hijo[i]->pesos[p] = dad[i]->pesos[p];
			}
		clon(hijo[0]->vecinos, dad[0]->vecinos, capas, cuantos);
	} else {
		for(i=0;i<cuantos;i++){
			hijo[i]->pesos[0] = dad[i]->pesos[0];
		}
	}
	return 0;
}


#include <stdio.h>
#include <stdlib.h>


typedef struct nodo{
	int sum;
	int cuantos;
	int *pesos;
	struct nodo **vecinos;
	struct nodo *out;
} Nodo, *NodoPtr;

NodoPtr nodalloc(int cuantos, int capas);
//void nodfree(NodPtr net);
int printnet(NodoPtr *net, int capas, int cuantos);
int propaganet(NodoPtr *net, int capas, int cuantos);

int borrar(NodoPtr net, int capas, int cuantos);
int borrasums(NodoPtr *net, int capas, int cuantos);

int pulsar(NodoPtr net, int entrada, int capas, int cuantos, int capa_salida);

int main(){
	int cuantos = 2;
	int capas = 4;
	NodoPtr red = nodalloc(cuantos,capas);

	int i;

//	borrasums(red->vecinos, capas, cuantos);

//printnet(red->vecinos, capas, cuantos);

//	borrar(red, capas, cuantos);

	printnet(red->vecinos, capas, cuantos);
	printf("Direct out: %d\n", red->out->sum);
//	printnet(red->vecinos, capas, cuantos);

	borrar(red, capas, cuantos);

	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 2));
	borrar(red, capas, cuantos);

	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 2));
	borrar(red, capas, cuantos);

	printf("Salida caluclada: %d\n", pulsar(red, 5, capas, cuantos, 2));

	return 0;
}


int printnet(NodoPtr *net, int capas, int cuantos){
	int i,j,k,m,p;
	if(net[0]->vecinos[0]->vecinos != NULL){
				for(m=0;m<cuantos;m++){
					for(p=0;p<cuantos;p++){
						printf("sum %d peso %d\n", net[m]->sum, net[m]->pesos[p]);
						net[m]->vecinos[p]->sum += net[m]->sum * net[m]->pesos[p];
					}
				}	

					printf("\n-\n");

		printnet(net[0]->vecinos, capas, cuantos);
	} else {
		for(m=0;m<cuantos;m++){
			printf("sum %d peso ultimo %d\n", net[m]->sum, net[m]->pesos[0]);
			net[m]->vecinos[0]->sum += net[m]->sum * net[m]->pesos[0];
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
			net2[j]->pesos[0] = rand();
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
				net2[j]->pesos[n] = rand();
	
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
			nodarray[j]->pesos[k] = rand();

	net->vecinos = nodarray;
	net->pesos = pesosarray;


	for(j=0;j<cuantos;j++)
		net->pesos[j] = rand();

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
		net->vecinos[j]->sum += net->sum * net->pesos[j];

	 propaganet(net->vecinos, capas, cuantos);

	return net->out->sum;
}

int propaganet(NodoPtr *net, int capas, int cuantos){
	int i,j,k,m,p;
	if(net[0]->vecinos[0]->vecinos != NULL){
				for(m=0;m<cuantos;m++){
					for(p=0;p<cuantos;p++){
						//printf("peso %d\n", net[m]->pesos[p]);
						net[m]->vecinos[p]->sum += net[m]->sum * net[m]->pesos[p];
					}
					//printf("\n");
				}	

		propaganet(net[0]->vecinos, capas, cuantos);
	} else {
		for(m=0;m<cuantos;m++){
			//printf("peso ultimo %d\n", net[m]->pesos[0]);
			net[m]->vecinos[0]->sum += net[m]->sum * net[m]->pesos[0];
		}
		//printf("Salidurria: %d\n", net[0]->vecinos[0]->sum);
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
//				for(m=0;m<cuantos;m++){

					for(p=0;p<cuantos;p++){
//						printf("peso antes %d, ", net[m]->vecinos[p]->sum);
						net[0]->vecinos[p]->sum = 0;
//						printf("peso despues %d\n", net[m]->vecinos[p]->sum);

					}
//				}	
//					printf("\n-\n");

		borrasums(net[0]->vecinos, capas, cuantos);
	} else {
//		for(m=0;m<cuantos;m++){
//			printf("peso ultimo %d,", net[m]->vecinos[0]->sum );
			net[0]->vecinos[0]->sum = 0;
//			printf("peso despues %d\n", net[m]->vecinos[0]->sum);

//		}
//		printf("Donde sssssssssssssssssssssssiria salida de la red\n");
		//printf("Salida de la red: %d\n", net[0]->vecinos[0]->sum);
	}
	return 0;
}



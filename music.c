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
int main(){
	int max_node = 2;
	int capas = 4;
	NodoPtr red = nodalloc(max_node,capas);

	int i;
	for(i=0;i<max_node;i++){
//		printf("%d\n", red->pesos[i]);
//		printf("\t%d\n", red->vecinos[i]->pesos[0]);
	}

	printf("Fine\n");

//	NodoPtr *redarray = red->vecinos;

	printnet(red->vecinos, capas, max_node);
	return 0;
}


int printnet(NodoPtr *net, int capas, int cuantos){
	int i,j,k,m,p;
	k=0;

//		printf("%d\n", net[0]->vecinos[0]->sum);


//	if(net = NULL){
//		return 0;
//	}

//	for(k=0;k<cuantos;k++){
//		if(net[k]->vecinos[0]->vecinos == NULL){
//		for(m=0;m<cuantos;m++){
//			printf("salida %d\n",net[k]->pesos[0]);
		//	net[k]->vecinos[m]->sum += net[k]->sum * net[k]->pesos[m];
//		}
//printf("\n");
			// resultados finales a la salida out

//			;
//}

//}

//printf("Finishe\n");


//if(net[0]->vecinos == NULL)
//		printf("Finishhhhhhhhhh\n");
//else
if(net[0]->vecinos[0]->vecinos != NULL){
//	printf("sum %d\n", net[0]->vecinos[0]->sum);
			for(m=0;m<cuantos;m++){
				for(p=0;p<cuantos;p++){
					printf("peso %d\n", net[m]->pesos[p]);
					net[m]->vecinos[p]->sum += net[m]->sum * net[m]->pesos[p];
				}
				printf("\n");
				// sumarles al acumulador
			}	

	printnet(net[0]->vecinos, capas, cuantos);
} else {
	for(m=0;m<cuantos;m++){
		printf("peso ultimo %d\n", net[m]->pesos[0]);
		net[m]->vecinos[0]->sum += net[m]->sum * net[m]->pesos[0];
	}
	printf("Salida de la red: %d\n", net[0]->vecinos[0]->sum);
}

//printnet(net[k-1]->vecinos, capas, cuantos);

//		} else { 

//	if(net[k]->vecinos[0]->vecinos != NULL){
//		for(i = 0; i < cuantos;i++){
//			printf("%d\n",i);
//			for(j=0;j<cuantos;j++){
//				printf("\t%d\n",net[i]->pesos[j]);
//			}
//		}

//		printnet(net[++k]->vecinos, --capas, cuantos);
//	} else {
//	}
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
	net->sum = -1; // entrada de la red
//	NodoPtr out ;

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
		nodarray[j]->sum += net->sum * net->pesos[j];



	NodoPtr out = malla( net->vecinos, capas, cuantos);

	net->out = out;

	return net;
}




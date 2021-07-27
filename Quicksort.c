/*
* Algoritmo ILS para solução da Mochila 0-1
* Código original por Philippe Leal (IFF campus Campos centro)
* Adaptado por Valmir Monteiro Júnior
*/
#include <stdio.h>
#include "ILS_Header.h"


void OrdenarCandidatos(Item *vet, int quant, Vetor2* vetorMedia){
	Quicksort(vet, 0, quant - 1);
	Vetor2 pMin, pMax;
	pMin.x = pMax.x = vet[0].valor;
	pMin.y = pMax.y = vet[0].peso;
	
	for(int i = 0; i < quant; i++){
		vet[i].indice = i;
		if(vet[i].valor > pMax.x){
			pMax.x = vet[i].valor;
		} else if(vet[i].valor < pMin.x){
			pMin.x = vet[i].valor;
		}

		if(vet[i].peso > pMax.y){
			pMax.y = vet[i].peso;
		} else if(vet[i].peso < pMin.y){
			pMin.y = vet[i].peso;
		}
	}
	vetorMedia->x = (pMax.x-pMin.x)/quant + 1;
	vetorMedia->y = (pMax.y-pMin.y)/quant + 1;
}

void Quicksort(Item *vet, int ini, int fim){
	int meio;
	if (ini < fim){
		meio = Particiona(vet, ini, fim);
		Quicksort(vet, ini, meio - 1);
		Quicksort(vet, meio + 1, fim);
	}
}

int Particiona(Item *vet, int ini, int fim){
	float pivo = vet[ini].proporcao;
	int i = ini, j;
	Item aux;

	for (j = ini + 1; j <= fim; j++){
		if (vet[j].proporcao >= pivo){
			i++;
			CopiarItem(&aux, &vet[i]);
			CopiarItem(&vet[i], &vet[j]);
			CopiarItem(&vet[j], &aux);
		}
	}

	CopiarItem(&aux, &vet[i]);
	CopiarItem(&vet[i], &vet[ini]);
	CopiarItem(&vet[ini], &aux);
	return i;
}

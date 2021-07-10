#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "ILS_Header.h"

Item* LerArquivo(FILE* arquivo, int quantidade){
	Item* itens = (Item*)malloc(sizeof(Item) * quantidade);
	for(int i = 0; i < quantidade; i++){
		// Só atribui índices depois de ordenar
		fscanf(arquivo, " %d", &(itens[i].valor));
		fscanf(arquivo, " %d", &(itens[i].peso));
		itens[i].indice = 0;
		itens[i].proporcao = (float)itens[i].valor/itens[i].peso;
	}
	return itens;
}

void CopiarItem(Item* destino, Item* origem){
	destino->indice = origem->indice;
	destino->valor = origem->valor;
	destino->peso = origem->peso;
	destino->proporcao = origem->proporcao;
} 



void AdicionarItem(Mochila* mochila, Item* item){
	ItemNode* itemNovo = (ItemNode*)malloc(sizeof(ItemNode));
	itemNovo->itemPtr = item;
	itemNovo->proximo = mochila->listaItens;
	mochila->listaItens = itemNovo;

	//Atualiza a mochila
	(mochila->pesoTotal) += item->peso;
	(mochila->valorTotal) += item->valor;
	(mochila->itensTotal)++;
}

void DestruirMochila(Mochila* mochila){
	ItemNode *itemAtual, *itemProximo;
	itemAtual = mochila->listaItens;
	if(itemAtual != NULL){
		itemProximo = itemAtual->proximo;
		while(itemAtual != NULL){
			free(itemAtual->itemPtr);
			free(itemAtual);
			itemAtual = itemProximo;
			if(itemProximo != NULL){
				itemProximo = itemProximo->proximo;
			}
		}
	}
	free(mochila);
}

void ImprimirMochila(Mochila* mochila){
	printf("--\nMochila: \n");
	printf("Valor total: %d Peso total: %d  Total de itens: %d\n",mochila->valorTotal, mochila->pesoTotal, mochila->itensTotal);
	printf("--\n");
	printf("Item a item: (Valor, Peso)\n");
	ItemNode* no = mochila->listaItens;
	while(no != NULL){
		printf("(%d, %d) ", no->itemPtr->valor, no->itemPtr->peso);
		no = no->proximo;
	}
}

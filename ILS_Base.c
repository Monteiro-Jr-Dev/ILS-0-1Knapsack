#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ILS_Header.h"

Item* LerArquivo(FILE* arquivo, int quantidade, float* mediaPeso){
	Item* itens = (Item*)malloc(sizeof(Item) * quantidade);
	if(itens == NULL){
		printf("Malloc falhou em LerArquivo");
		exit(0);
	}
	*mediaPeso = 0;
	for(int i = 0; i < quantidade; i++){
		// Só atribui índices depois de ordenar
		fscanf(arquivo, " %d", &(itens[i].valor));
		fscanf(arquivo, " %d", &(itens[i].peso));
		itens[i].indice = 0;
		itens[i].proporcao = (float)itens[i].valor/itens[i].peso;
		*mediaPeso += itens[i].peso;
	}
	*mediaPeso /= quantidade;
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
	if(itemNovo == NULL){
		printf("Malloc falhou em AdicionarItem");
		exit(0);
	}
	itemNovo->itemPtr = item;
	itemNovo->proximo = mochila->listaItens;
	mochila->listaItens = itemNovo;

	//Atualiza a mochila
	(mochila->pesoTotal) += item->peso;
	(mochila->valorTotal) += item->valor;
	(mochila->itensTotal)++;
}

void DestruirMochila(Mochila* mochila){
	if(mochila != NULL){
		ItemNode *itemAtual, *itemProximo;
		itemAtual = mochila->listaItens;
		if(itemAtual != NULL){
			itemProximo = itemAtual->proximo;
			while(itemAtual != NULL){
				free(itemAtual);
				itemAtual = itemProximo;
				if(itemProximo != NULL){
					itemProximo = itemProximo->proximo;
				}
			}
		}
		
		free(mochila);
	}
}

void DestruirMemoria(Mochila** mem, int quant){
	for(int i = 0; i <  quant; i++){
		free(mem[i]);
	}
	free(mem);
}

Mochila* CriarMochila(int indicePivo){
	Mochila* novaMochila = (Mochila*)malloc(sizeof(Mochila));
	if(novaMochila == NULL){
		printf("Malloc falhou em CriarMohila");
		exit(0);
	}
	novaMochila->id = indicePivo;
	novaMochila->itensTotal = 0;
	novaMochila->listaItens = NULL;
	novaMochila->pesoTotal = 0;
	novaMochila->valorTotal = 0;
	return novaMochila;
}

void ImprimirMochila(Mochila* mochila){
	printf("\n--\nMochila: \n");
	printf("Valor total: %d Peso total: %d  Total de itens: %d\n",mochila->valorTotal, mochila->pesoTotal, mochila->itensTotal);
	printf("--\n");
	printf("Item a item: (Valor, Peso)\n");
	ItemNode* no = mochila->listaItens;
	while(no != NULL){
		printf("(%d, %d) ", no->itemPtr->valor, no->itemPtr->peso);
		no = no->proximo;
	}
}

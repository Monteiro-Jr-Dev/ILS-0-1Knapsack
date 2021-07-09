#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ILS_Header.h"

Item* LerArquivo(FILE* arquivo, int quantidade){
	Item* itens = (Item*)malloc(sizeof(Item) * quantidade);
	for(int i = 0; i < quantidade; i++){
		fscanf(arquivo, "%d", &(itens[i].valor));
		fscanf(arquivo, "%d", &(itens[i].peso));
	}
	return itens;
}

Mochila* SolucaoInicialAleatoria(Item* candidatos, int quantidade, int capacidade){
	int vetor[quantidade], pos;
	Mochila* solucao = (Mochila*)malloc(sizeof(Mochila));
	solucao->itensTotal = 0;
	solucao->valorTotal = 0;
	solucao->pesoTotal = 0;
	solucao->listaItens = NULL;
	srand(time(NULL));
	for(int i = 0; i < quantidade; i++){
		vetor[i] = 0;
	}
	for(int i = 0; i < quantidade; i++){
		pos = rand()%quantidade;
		if(!vetor[pos]){
			if(solucao->pesoTotal + candidatos[pos].peso < capacidade){
				Item* iPtr = (Item*)malloc(sizeof(Item));
				iPtr->peso = candidatos[pos].peso;
				iPtr->valor = candidatos[pos].valor;
				AdicionarItem(solucao, iPtr);
				vetor[pos] = 1;
			}
		}
	}

	return solucao;
}

// Solução inicial com guloso
Mochila* SolucaoInicalGulosa(Item* candidatos, int quantidade, int capacidade){ // Checar se quantidade > 0
	int mochilaTemEspaco = 1, novoPeso = 0;
	ItemNode* itemAtual, *itemAnterior, *tempAtual, *tempAnterior;
	Mochila* candidatosOrdenados; 
	Mochila* solucao = (Mochila*)malloc(sizeof(Mochila));
	solucao->itensTotal = 0;
	solucao->valorTotal = 0;
	solucao->pesoTotal = 0;
	solucao->listaItens = NULL;	

	// Cria uma mochila com itens ordenados de forma decrescente por proporção (valor/peso)
	candidatosOrdenados = CriarMochilaOrdenada(candidatos, quantidade);
	// Preenche a solução inicial a partir dos candidatos ordenados	
	itemAnterior = itemAtual = candidatosOrdenados->listaItens;
	while((itemAtual != NULL) && (solucao->pesoTotal < capacidade)){
		novoPeso = itemAtual->itemPtr->peso + solucao->pesoTotal;

		// Se couber, remove da mochila de candidatos e adiciona à solução
		if(novoPeso < capacidade){
			// Verifica se é o primeiro da lista
			if(itemAtual == itemAnterior){
				candidatosOrdenados->listaItens = tempAtual = tempAnterior = itemAtual->proximo;
			}else{
				tempAnterior = itemAnterior;
				itemAnterior->proximo = tempAtual = itemAtual->proximo;
			}
			
			// Atualizar as mochilas
			(candidatosOrdenados->pesoTotal) -= itemAtual->itemPtr->peso;
			(candidatosOrdenados->valorTotal) -= itemAtual->itemPtr->valor;
			(candidatosOrdenados->itensTotal)--;
			(solucao->pesoTotal) += itemAtual->itemPtr->peso;
			(solucao->valorTotal) += itemAtual->itemPtr->valor;
			(solucao->itensTotal)++;

			// Atualizar ponteiros
			itemAtual->proximo = solucao->listaItens;
			solucao->listaItens = itemAtual;
			itemAtual = tempAtual;
			itemAnterior = tempAnterior;
		} else {
			itemAnterior = itemAtual;
			itemAtual = itemAtual->proximo;
		}
	}

	// Liberar memória
	DestruirMochila(candidatosOrdenados);

	return solucao;	
}

// Ordenar com base na proporção (valor/peso)
Mochila* CriarMochilaOrdenada(Item* candidatos, int quantidade){

	Mochila* ordenados = (Mochila*)malloc(sizeof(Mochila));	
	ordenados->pesoTotal = ordenados->valorTotal = ordenados->itensTotal =0;
	ordenados->listaItens = NULL;
	// Adiciona todos os candidatos à mochila ordenada
	for(int i = 0; i < quantidade; i++){
		Item* itemNovo = (Item*)malloc(sizeof(Item));
		itemNovo->peso = candidatos[i].peso;
		itemNovo->valor = candidatos[i].valor;
		AdicionarItemOrdenado(ordenados, itemNovo);
	}

	return ordenados;
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

void AdicionarItemOrdenado(Mochila* mochila, Item* item){
	int inicio = 0,	meio = 0, fim , inseriuItem = 0;
	float proporcaoItem, proporcaoNovo;
	ItemNode* itemNo = (ItemNode*)malloc(sizeof(ItemNode));
	ItemNode *itemAtual, *itemAnterior;
	itemNo->itemPtr = item;
	proporcaoNovo = (float)item->valor/item->peso;
	// Quando Mochila vazia
	if(mochila->itensTotal == 0){
		itemNo->proximo = mochila->listaItens;
		mochila->listaItens = itemNo;
		inseriuItem = 1;
	}else{
		fim = mochila->itensTotal - 1;
	}

	// Inserir Item de forma ordenada (baseado em Binary search)
	while(!inseriuItem){		
		if(fim - inicio > 0){
			meio = (inicio + fim)/2;
			itemAtual = mochila->listaItens;
			for(int i = 0; i < meio; i++){
				itemAtual = itemAtual->proximo;
			}
			proporcaoItem = (float)itemAtual->itemPtr->valor/itemAtual->itemPtr->peso;
			if(proporcaoNovo > proporcaoItem){
				fim = meio - 1;
			}else if (proporcaoNovo < proporcaoItem){
				inicio = meio + 1;
			}else{
				inicio = fim;
			}

		}else{
			itemAtual = itemAnterior = mochila->listaItens;
			for(int i = 0; i < inicio; i++){
				itemAnterior = itemAtual;
				itemAtual = itemAtual->proximo;
			}
			proporcaoItem = (float)(itemAtual->itemPtr->valor)/(itemAtual->itemPtr->peso);
			// Inserir antes do itemAtual
			if(proporcaoNovo > proporcaoItem){		
				// Checa se está no início da lista		
				if(itemAnterior == itemAtual){
					itemNo->proximo = mochila->listaItens;
					mochila->listaItens = itemNo;
				}else{					
					itemNo->proximo = itemAtual;
					itemAnterior->proximo = itemNo;
				}
				inseriuItem = 1;
				break;
			} else if(proporcaoNovo < proporcaoItem){
				// Inserir depois do itemAtual
				itemNo->proximo = itemAtual->proximo;
				itemAtual->proximo = itemNo;
				inseriuItem = 1;
				break;
			} else {
				// Se itemNo e itemAtual possuiren a mesma proporção, o de maior valor tem precedencia
				if(itemNo->itemPtr->valor > itemAtual->itemPtr->valor){
					if(itemAnterior == itemAtual){
						itemNo->proximo = mochila->listaItens;
						mochila->listaItens = itemNo;
					}else{
						itemNo->proximo = itemAtual;
						itemAnterior->proximo = itemNo;
					}
					inseriuItem = 1;
					break;
				} else {
					itemNo->proximo = itemAtual->proximo;
					itemAtual->proximo = itemNo;
					inseriuItem = 1;
					break;
				}
			}
		}
	}

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
	printf("Valor total: %d Peso total: %d  Total de itens: %d\n",mochila->pesoTotal, mochila->valorTotal, mochila->itensTotal);
	printf("--\n");
	printf("Item a item: (Valor, Peso)\n");
	ItemNode* no = mochila->listaItens;
	while(no != NULL){
		printf("(%d, %d) ", no->itemPtr->valor, no->itemPtr->peso);
		no = no->proximo;
	}
}
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ILS_Header.h"

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
	ItemNode* novoItem;
	Mochila* solucao = (Mochila*)malloc(sizeof(Mochila));
	solucao->itensTotal = 0;
	solucao->valorTotal = 0;
	solucao->pesoTotal = 0;
	solucao->listaItens = NULL;	

	// Preenche a solução inicial a partir dos candidatos ordenados	
	for(int i = 0; i < quantidade; i++){
		if(solucao->pesoTotal >= capacidade){
			break;
		}
		novoPeso = candidatos[i].peso + solucao->pesoTotal;
		if(novoPeso <= capacidade){
			AdicionarItem(solucao, &candidatos[i]);
		}
	}	
	return solucao;	
}

// Retorna a distancia entre dois pontos (ao quadrado, não calcula raiz).
int DistanciaQuadrada(Vetor2 v1, Vetor2 v2){
	Vetor2 dist = {v2.x-v1.x, v2.y-v1.y};
	return (dist.x*dist.x + dist.y*dist.y);
}

// Retorna uma matriz triangular com as relações do grafo
// Usando unsigned char e bit shifting pra economizar memória 
unsigned char** CriarMatrizAdjacencia(int quantidade){
	unsigned char** matriz = (unsigned char**)malloc(sizeof(unsigned char*)*(quantidade - 1));
	for(int i = 0; i < quantidade - 1; i++){
		int tamanho = (quantidade - 1) - i;
		matriz[i] = (char*) malloc(sizeof(char)*(tamanho/8 + 1));
		// Inicializar bits com 0
		for (int j = 0; j < (tamanho/8 + 1); j++){
			matriz[i][j] = 0b0;
		}
	}
	return matriz;
}

// Verifica quais vértices estão dentro do raio estabelecido para a vizinhança
// e preenche a matriz de adjacência
unsigned char** GerarVizinhanca(Item* candidatos, int raioMax, int quant){
    unsigned char** matriz = CriarMatrizAdjacencia(quant);

	// Test
	printf("Antrs de preencher\n");
	_imprimirMatriz(matriz, quant);

	Vetor2 pos1, pos2;
	int indLin, indCol;
	for(int i = 0; i < quant; i++){
		pos1.x = candidatos[i].valor; 
		pos1.y = candidatos[i].peso;
		indCol = 0;
		for(int j = i + 1; j < quant; j++, indCol++){			
			pos2.x = candidatos[j].valor;
			pos2.y = candidatos[j].peso;
			if(DistanciaQuadrada(pos1, pos2) <= raioMax){
				matriz[i][indCol/8] = matriz[i][indCol/8] | 1 << indCol%8;
			}
		}
	}

	// Test
	printf("Depois de preencher\n");
	_imprimirMatriz(matriz, quant);

    return matriz;
}

// Apenas para teste
static void _imprimirMatriz(unsigned char** mat, int quant){
	for(int i = 0; i < quant - 1; i++){
		for(int j = 0; j < (quant - 1) - i; j++){
			printf("%d ", (mat[i][j/8] & 1 << j%8) != 0);
		}
		printf("\n");
	}
}

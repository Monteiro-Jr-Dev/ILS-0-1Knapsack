/*
* Algoritmo ILS para solução da Mochila 0-1
* Autor: Valmir Monteiro Júnior
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ILS_Header.h"

int main(int argc, char *argv[]){
	clock_t fim, inicio;
	int opcao;
	FILE* arquivoEntrada;
	int capacidadeMochila;
	int quantidadeItens;
	Item* conjuntoCandidatos;
	Mochila *solucao;
	Mochila** memoria;
	Vetor2 vetorDistancia;
	int mediaDistancia;
	float mediaPeso;
	float raioVizinhanca;
	ItemNode *atual, *proximo;
	srand(time(NULL));
	

	// Leitura dos argumentos de entrada
	opcao = getopt(argc, argv, "g:a:");
	// Verificar se nome do arquivo foi passado
	if(optarg != NULL){
		// Verificar se arquivo existe
		if((arquivoEntrada = fopen(optarg, "r")) != NULL){	
			// Marcador para tempo de execução
			inicio = clock();

			// Ler mochila e ordenar vetor
			fscanf(arquivoEntrada, "%d", &capacidadeMochila);
			fscanf(arquivoEntrada, "%d", &quantidadeItens);
			conjuntoCandidatos = LerArquivo(arquivoEntrada, quantidadeItens, &mediaPeso);			
			
			// Ordenar Candidatos na proporção valor/peso
			OrdenarCandidatos(conjuntoCandidatos, quantidadeItens, &vetorDistancia);

			// Gerar vizinhança
			Vetor2 vetorZero = {0,0};
			mediaDistancia = DistanciaQuadrada(vetorZero, vetorDistancia);
			raioVizinhanca = (capacidadeMochila/mediaPeso)*mediaDistancia*MODIFICADOR_RAIO_VIZINHANCA;
			unsigned char** matrizAdj = GerarVizinhanca(conjuntoCandidatos, raioVizinhanca, quantidadeItens);
			
			// Cria memória (Vetor de mochilas)
			memoria = (Mochila**) malloc(sizeof(Mochila*)*quantidadeItens);		
			for(int i = 0; i < quantidadeItens; i++){
				memoria[i] = NULL;
			}	

			// Escolhe qual algoritmo rodar: (a) = SIA, BLG; (g) = SIG, BLG;
			// SIA = Solução Inicial Aleatória; SIG = Solução Inicial Gulosa
			// BLG = Busca Local (com Guloso)
			switch(opcao){
				case 'g':	
					// Gerar solução inicial
					solucao = SolucaoInicalGulosa(conjuntoCandidatos, memoria, quantidadeItens, capacidadeMochila);
				break;

				case 'a':	
					// Gerar uma solução inicial aleatória
					solucao = SolucaoInicialAleatoria(conjuntoCandidatos, quantidadeItens, capacidadeMochila);
				break;
			}

			// Gera soluções para todos os vizinhos
			for(int i = 0; i < quantidadeItens; i++){
				BuscaNoVizinho(conjuntoCandidatos, i, matrizAdj, memoria, quantidadeItens, capacidadeMochila);
			}

			// Busca iterada
			solucao =  BuscaLocal(solucao, conjuntoCandidatos, matrizAdj, memoria, quantidadeItens, capacidadeMochila);
		} else {
			// Problema de argumento
			fprintf(stderr, "Arquivo inexistente\n");
			return 1; 
		}
	} else {
		// Problema de argumento
		fprintf(stderr, "Argumento invalido\n");
		return 1; 
	}

	// Mostrar Resultado
	ImprimirMochila(solucao);

	// Liberar recursos
	DestruirMochila(solucao);
	DestruirMemoria(memoria, quantidadeItens);
	free(conjuntoCandidatos);
	fclose(arquivoEntrada);

	// Monitorar tempo de execução
	fim = clock();
	printf("\n--\nTempo de execucao: %.5f segundos\n", (float)(fim - inicio)/CLOCKS_PER_SEC);

	return 0;
}

int NumeroAleatorio(){
	return rand();
}
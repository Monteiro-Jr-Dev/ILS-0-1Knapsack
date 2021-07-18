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
	Mochila* solucao;
	Mochila** memoria;
	Vetor2 vetorDistancia;
	int raioVizinhanca;

	// Leitura dos argumentos de entrada
	opcao = getopt(argc, argv, "s:o:");
	// Verificar se nome do arquivo foi passado
	if(optarg != NULL){
		// Verificar se arquivo existe
		if((arquivoEntrada = fopen(optarg, "r")) != NULL){	

			// Ler mochila e ordenar vetor
			fscanf(arquivoEntrada, "%d", &capacidadeMochila);
			fscanf(arquivoEntrada, "%d", &quantidadeItens);
			conjuntoCandidatos = LerArquivo(arquivoEntrada, quantidadeItens);			
			
			// Marcador para tempo de execução
			inicio = clock();
	
			// Escolhe qual algoritmo rodar: (a) = SIA, BLA; (b) = SIA, BLO; (c) = SIG, BLA; (d) = SIG, BLO;
			// SIA = Solução Inicial Aleatória; SIG = Solução Inicial Gulosa
			// BLA = Busca Local Aleatória; BLG = Busca Local Gulosa
			switch(opcao){
				case 's':	

					// Ordenar Candidatos na proporção valor/peso
					OrdenarCandidatos(conjuntoCandidatos, quantidadeItens, &vetorDistancia);	

					// Gerar vizinhança
					Vetor2 vetorZero = {0,0};
					raioVizinhanca = DistanciaQuadrada(vetorZero, vetorDistancia)*TAMANHO_VIZINHANCA;
					unsigned char** matrizAdj = GerarVizinhanca(conjuntoCandidatos, raioVizinhanca, quantidadeItens);	

					// Cria memória
					memoria = (Mochila**) malloc(sizeof(Mochila*)*quantidadeItens);									
					
					// Gerar solução inicial
					solucao = SolucaoInicalGulosa(conjuntoCandidatos, memoria, quantidadeItens, capacidadeMochila);

					// Gerar uma solução inicial aleatória
					//solucao = SolucaoInicialAleatoria(conjuntoCandidatos, quantidadeItens, capacidadeMochila);	
										
					// Busca iterada
					solucao =  BuscaLocal(solucao, 0, conjuntoCandidatos, matrizAdj, memoria, quantidadeItens, capacidadeMochila, 0);
				break;

				case 'o':					
					fprintf(stderr, "Nao Implementado\n");
					return 1;
				break;
				
			}
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
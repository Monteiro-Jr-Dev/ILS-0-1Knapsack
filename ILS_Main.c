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
	
			// Escolhe qual algoritmo rodar: (s)implificado ou (o)timizado
			switch(opcao){
				case 's':	
					inicio = clock();

					// Ordenar Candidatos na proporção valor/peso
					OrdenarCandidatos(conjuntoCandidatos, quantidadeItens, &vetorDistancia);	

					// Gerar vizinhança
					Vetor2 vetorZero = {0,0};
					raioVizinhanca = DistanciaQuadrada(vetorZero, vetorDistancia)*TAMANHO_VIZINHANCA;
					unsigned char** matrizAdj = GerarVizinhanca(conjuntoCandidatos, raioVizinhanca, quantidadeItens);										
					
					// Gerar solução inicial
					solucao = SolucaoInicalGulosa(conjuntoCandidatos, quantidadeItens, capacidadeMochila);

					// Gerar uma solução inicial aleatória
					//solucao = SolucaoInicialAleatoria(conjuntoCandidatos, quantidadeItens, capacidadeMochila);	
										
					// Busca iterada
					//solucao =  BuscaLocal(solucao, matrizAdj, quantidadeItens, capacidadeMochila, LIMITE_ITERACOES_SEM_MELHORA);

					// Tempo de execução
					fim = clock();
					printf("Solucao Inicial Aleatoria: %.5f segundos\n", (float)(fim - inicio)/CLOCKS_PER_SEC);	
				break;

				case 'o':
					inicio = clock();

					// Monitorar tempo de execução
					fim = clock();
					printf("Solucao Inicial Gulosa: %.5f segundos\n", (float)(fim - inicio)/CLOCKS_PER_SEC);
					//solucao =  BuscaLocalOtimizada(conjuntoCandidatos, solucao, quantidadeItens, capacidadeMochila);
					
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
	free(conjuntoCandidatos);
	fclose(arquivoEntrada);
	return 0;
}
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
	

	// Leitura dos argumentos de entrada
	opcao = getopt(argc, argv, "s:o:");
	// Verificar se nome do arquivo foi passado
	if(optarg != NULL){
		// Verificar se arquivo existe
		if((arquivoEntrada = fopen(optarg, "r")) != NULL){	

			// Ler mochila para vetor
			fscanf(arquivoEntrada, "%d", &capacidadeMochila);
			fscanf(arquivoEntrada, "%d", &quantidadeItens);
			conjuntoCandidatos = LerArquivo(arquivoEntrada, quantidadeItens);

			// Escolhe qual algoritmo rodar: (s)implificado ou (o)timizado
			switch(opcao){
				case 's':	
					inicio = clock();
					// Gerar uma solução inicial aleatória
					solucao = SolucaoInicialAleatoria(conjuntoCandidatos, quantidadeItens, capacidadeMochila);	
					// Tempo de execução
					fim = clock();
					printf("Solucao Inicial Aleatoria: %.5f segundos\n", (float)(fim - inicio)/CLOCKS_PER_SEC);	
					//solucao =  BuscaLocalSimples(conjuntoCandidatos, solucao, quantidadeItens, capacidadeMochila);
				break;

				case 'o':
					inicio = clock();
					// Gerar uma solução inicial com guloso
					solucao = SolucaoInicalGulosa(conjuntoCandidatos, quantidadeItens, capacidadeMochila);
					// Tempo de execução
					fim = clock();
					printf("Solucao Inicial Gulosa: %.5f segundos\n", (float)(fim - inicio)/CLOCKS_PER_SEC);
					//solucao =  BuscaLocalOtimizada(conjuntoCandidatos, solucao, quantidadeItens, capacidadeMochila);
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

	fclose(arquivoEntrada);
	return 0;
}
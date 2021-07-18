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
Mochila* SolucaoInicalGulosa(Item* candidatos, Mochila** memoria, int quantidade, int capacidade){ // Checar se quantidade > 0
	int mochilaTemEspaco = 1, novoPeso = 0;
	ItemNode* novoItem;
	Mochila* solucao = (Mochila*)malloc(sizeof(Mochila));
	solucao->id = quantidade-1;
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
	memoria[solucao->id] = solucao;
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

	Vetor2 pos1, pos2;
	int indLin, indCol;
	for(int i = 0; i < quant; i++){
		pos1.x = candidatos[i].valor; 
		pos1.y = candidatos[i].peso;
		indCol = 0;
		candidatos[i].quantVizinhos = 0;
		for(int j = i + 1; j < quant; j++, indCol++){			
			pos2.x = candidatos[j].valor;
			pos2.y = candidatos[j].peso;
			if(DistanciaQuadrada(pos1, pos2) <= raioMax){
				matriz[i][indCol/8] = matriz[i][indCol/8] | 1 << indCol%8;
				candidatos[i].quantVizinhos++;
			}
		}
	}
	//_imprimirMatriz(matriz, quant);
    return matriz;
}

Mochila* BuscaLocal(Mochila* solucao, int indiceBase, Item* conjuntoCandidatos, unsigned char** matrizAdj, Mochila** memoria, int quantidadeItens, int capacidadeMochila, int iteracoesSemMelhora){
	Mochila* solucaoNova, *solucaoRetorno;
	int iteCont = 0, indice, cont = 0;

	// Verificar se não consegue encontrar uma solução melhor durante um longo período
	if(iteracoesSemMelhora < LIMITE_ITERACOES_SEM_MELHORA){
		int* posVizinhos = (int*)malloc(sizeof(int)*conjuntoCandidatos[indiceBase].quantVizinhos);
		// Faz busca local usando os itens da solução atual como base
		ItemNode* itemAtual = solucao->listaItens;		
		for(int i = 0; i < solucao->itensTotal; i++){
			// Guardar a posição dos vizinhos no vetor de candidatos
            posVizinhos[cont++] = itemAtual->itemPtr->indice;
			// Calcula apenas se solução do item não existe na memória
			if(memoria[itemAtual->itemPtr->indice] == NULL){
				BuscaNoVizinho(conjuntoCandidatos, itemAtual->itemPtr->indice, matrizAdj, memoria, quantidadeItens);
			}
			itemAtual = itemAtual->proximo;
		}		

		// Escolher a melhor entre a melhor anterior e as soluções da vizinhança
		solucaoNova = memoria[indiceBase];
		if(solucaoNova->valorTotal > solucao->valorTotal){
			solucaoNova = solucao;
		}
		for(int i = 0; i < conjuntoCandidatos[indiceBase].quantVizinhos; i++){
			if(solucaoNova->valorTotal > memoria[posVizinhos[i]]->valorTotal){
				solucaoNova = memoria[posVizinhos[i]];
			}
		}			

		// Atualiza contador de iterações sem melhora
		if(solucaoNova == solucao){
			iteCont = iteracoesSemMelhora + 1;
		}else{
			iteCont = 0;
			solucao = solucaoNova;
		}

		free(posVizinhos);
	
		// Perturbação
		solucaoNova = Pertubacao(solucao, memoria, capacidadeMochila);

		// Seleciona o menor indice (onde valor/peso é maior) da solução nova para usar como base
		ItemNode* itemAtual = solucao->listaItens;	
		for (int i = 0; i < solucao->itensTotal; i++){
			if(i == 0){
				indice = itemAtual->itemPtr->indice;
			}else{
				if(indice > itemAtual->itemPtr->indice){
					indice = itemAtual->itemPtr->indice;
				}
			}			
			itemAtual = itemAtual->proximo;
		}
		
		solucaoRetorno = BuscaLocal(solucaoNova, indice, conjuntoCandidatos, matrizAdj, memoria, quantidadeItens, capacidadeMochila, iteCont);
		if(solucaoRetorno->valorTotal < solucao->valorTotal){
			solucao = solucaoRetorno;
		}else if(solucaoNova->valorTotal < solucao->valorTotal){
			solucao = solucaoNova;
		}
	}
	return solucao;
}

// Encontra (e guarda na memória) a solução de cada vizinho utilizando guloso
// Usa a matriz de adjacência para montar a solução
void BuscaNoVizinho(Item* conjuntoCandidatos, int indiceBase, unsigned char** matrizAdj, Mochila** memoria, int quantidadeItens){
	/*
	// Verificar se não consegue encontrar uma solução melhor durante um longo período
    if(iteracoesSemMelhora < LIMITE_ITERACOES_SEM_MELHORA){
        int* posVizinhos = (int*)malloc(sizeof(int)*conjuntoCandidatos[indiceBase].quantVizinhos);

        // Calcular a solução do nó atual e todos os vizinhos e guarda a posição de todos 
        if(memoria[indiceBase] == NULL){
            BuscaVizinhos(conjuntoCandidatos, indiceBase, matrizAdj, memoria, quantidadeItens);
        }
        // Buscar vizinhos na coluna do item
        for(int i = 0; i < indiceBase; i++){
            int col = indiceBase - 1 - i;
            if(matrizAdj[i][col/8] & 1 << col%8){
                // Guardar a posição dos vizinhos no vetor de candidatos
                posVizinhos[cont++] = i;
                // Se vizinho não existe na memória, calcula
                if(memoria[i] == NULL){
                    BuscaVizinhos(conjuntoCandidatos, i, matrizAdj, memoria, quantidadeItens);
                }
            }
        }
        // Buscar vizinhos na linha do item
        for(int i = 0; i < (quantidadeItens - 1) - indiceBase; i++){
            if((matrizAdj[indiceBase][i/8] & 1 << i%8)){
                // Guardar a posição dos vizinhos no vetor de candidatos
                posVizinhos[cont++] = (i + 1 + indiceBase);
                // Se vizinho não existe na memória, calcula
                if(memoria[i] == NULL){
                    BuscaVizinhos(conjuntoCandidatos, (i + 1 + indiceBase), matrizAdj, memoria, quantidadeItens);
                }
            }
        }
	*/
}

Mochila* Pertubacao(Mochila* solucao, Mochila** memoria, int capacidade){ 
	srand(time(NULL));
	int pertTotal = (int)(solucao->itensTotal * GRAU_PERTURBACAO);	
	int pertCont = 0;
	int* indicesNaoPerturbados = (int*)malloc(sizeof(int)*(solucao->itensTotal - pertTotal));
	int indCont = 0;
	ItemNode* itemAtual, *vizinhoAtual;
	Mochila* novaSolucao;

	// Escolhe índices que não serão perturbados
	for(int i = 0; i < (solucao->itensTotal - pertTotal); i++){
		int indice = rand()%solucao->itensTotal;
		for(int j = i - 1; j >= 0; j--){
			if(indicesNaoPerturbados[j] == indice){
				i--;
				break;
			}
		}
		indicesNaoPerturbados[i] = indice;
	}

	// Criar uma cópia com os índices não perturbados
	for(int i = 0; i < solucao->itensTotal; i++){
		itemAtual = solucao->listaItens;
		for(int j = 0; j < indicesNaoPerturbados[i]; j++){
			itemAtual = itemAtual->proximo;
		}
		AdicionarItem(novaSolucao, itemAtual);
	}	

	int naoEncontrou = 0;
	// Adiciona itens aleatórios da vizinhança 
	// Sai se não conseguir encontrar itens para adicionar duranco N iterações
	while(naoEncontrou < LIMITE_ITERACOES_SEM_MELHORA){ 
		
		// Escolhe qual vizinho usar
		int vizinhoAleatorio = rand()%solucao->itensTotal;
		vizinhoAtual = solucao->listaItens;
		for(int j = 0; j < vizinhoAleatorio; j++){
			vizinhoAtual = vizinhoAtual->proximo;
		}		

		// Escolhe um item do vizinho
		int totalItensVizinho = memoria[vizinhoAtual->itemPtr->indice]->itensTotal;
		for(int j = 0; j < totalItensVizinho; j++){
			int itemAleatorio = rand()%(memoria[vizinhoAtual->itemPtr->indice]->itensTotal);
			itemAtual = memoria[vizinhoAtual->itemPtr->indice]->listaItens;
			for(int k = 0; k < itemAleatorio; k++){
				itemAtual = itemAtual->proximo;
			}
			if((novaSolucao->pesoTotal + itemAtual->itemPtr->peso) < capacidade){
				AdicionarItem(novaSolucao, itemAtual);
				naoEncontrou = 0;
				break;
			}			
		}
		naoEncontrou++;		
	}
	
}

// Apenas para teste
static void _imprimirMatriz(unsigned char** mat, int quant){
	printf("    ");
	for(int i = quant - 1; i > 0; i--){
		printf("(%3d)", i);
	}
	printf("\n");
	for(int i = 0; i < quant - 1; i++){
		printf("(%2d)", i);
		for(int j = 0; j < (quant - 1) - i; j++){
			printf("%4d ", (mat[i][j/8] & 1 << j%8) != 0);
		}
		printf("\n");
	}
}

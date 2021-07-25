#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "ILS_Header.h"

Mochila* SolucaoInicialAleatoria(Item* candidatos, int quantidade, int capacidade){
	int pos;
	int *vetor = (int*)malloc(sizeof(int)*quantidade);
	Mochila* solucao = CriarMochila(-1);
	srand(time(NULL));
	for(int i = 0; i < quantidade; i++){
		vetor[i] = 0;
	}
	for(int i = 0; i < quantidade; i++){
		pos = rand()%quantidade;
		if(!vetor[pos]){
			if(solucao->pesoTotal + candidatos[pos].peso < capacidade){
				AdicionarItem(solucao, &candidatos[pos]);
				vetor[pos] = 1;
			}
		}
	}
	free(vetor);
	return solucao;
}

// Solução inicial com guloso
Mochila* SolucaoInicalGulosa(Item* candidatos, Mochila** memoria, int quantidade, int capacidade){ // Checar se quantidade > 0
	int mochilaTemEspaco = 1, novoPeso = 0;
	ItemNode* novoItem;
	//Mochila* solucao = CriarMochila(quantidade - 1); // Porque quantidade - 1? Não deveria ser 0?
	// A solução tem índice -1 pois não pertence à memória, não será salva 
	Mochila* solucao = CriarMochila(-1);

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
	//memoria[solucao->id] = solucao;
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
	if(matriz == NULL){
		printf("Malloc falhou em CriarMatrizAdjacencia");
		exit(0);
	}
	for(int i = 0; i < quantidade - 1; i++){
		int tamanho = (quantidade - 1) - i;
		matriz[i] = (char*) malloc(sizeof(char)*ceil(tamanho/8));
		if(matriz[i] == NULL){
			printf("Malloc falhou em CriarMatrizAdjacencia");
			exit(0);
		}
		
		// Inicializar bits com 0
		for (int j = 0; j < ceil(tamanho/8); j++){
			matriz[i][j] = 0b0;
		}
	}
	return matriz;
}

// Verifica quais vértices estão dentro do raio estabelecido para a vizinhança
// e preenche a matriz de adjacência
unsigned char** GerarVizinhanca(Item* candidatos, float raioMax, int quant){
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

Mochila* BuscaLocal(Mochila* solucaoMelhorGeral, int indicePivo, Item* conjuntoCandidatos, unsigned char** matrizAdj, Mochila** memoria, int quantidadeItens, int capacidadeMochila){
	Mochila *solucaoPerturbada, *solucaoMelhorLocal;
	int iteCont = 0, indicePerturbado, cont = 0, iteracoesSemMelhora = 0;	
	ItemNode* itemAtual;
	int controlePerturbacao = LIMITE_ITERACOES_SEM_MELHORA*0.5;
	int contPert = 0;

	indicePerturbado = indicePivo;
	solucaoMelhorLocal = solucaoMelhorGeral;
	solucaoPerturbada = solucaoMelhorGeral;
	
	


	// Verificar se não consegue encontrar uma solução melhor durante um longo período
	while(iteracoesSemMelhora < LIMITE_ITERACOES_SEM_MELHORA){

		int* posVizinhos = (int*)malloc(sizeof(int)*(solucaoPerturbada->itensTotal + 1));	
		if(posVizinhos == NULL){
			printf("Malloc falhou em BuscaLocal");
			exit(0);
		}

		// Faz busca local usando os itens da solução perturbada como base
		cont = 0;
		itemAtual = solucaoPerturbada->listaItens;	
		posVizinhos[cont++] = indicePerturbado;	
		for(int i = 0; i < solucaoPerturbada->itensTotal; i++){
			// Guardar a posição dos vizinhos no vetor de candidatos
            posVizinhos[cont++] = itemAtual->itemPtr->indice;
			/*
			// Calcula apenas se solução do item não existe na memória
			if(memoria[itemAtual->itemPtr->indice] == NULL){
				BuscaNoVizinho(conjuntoCandidatos, itemAtual->itemPtr->indice, matrizAdj, memoria, quantidadeItens, capacidadeMochila);
			}
			*/
			itemAtual = itemAtual->proximo;
		}			
		
		/*
		// Calcula a solução do índice pivô, caso já não tenha feito (A vizinhança inclui o pivô)
		if(memoria[indicePivo] == NULL){
			BuscaNoVizinho(conjuntoCandidatos, indicePivo, matrizAdj, memoria, quantidadeItens, capacidadeMochila);
		}
		*/
		
		
		// Escolher a melhor entre a as soluções da vizinhança (incluindo o pivo) da solução perturbada
		solucaoMelhorLocal = memoria[indicePerturbado];
		if(solucaoMelhorLocal->valorTotal < solucaoPerturbada->valorTotal){
			if(solucaoMelhorLocal->id == -1){
				//DestruirMochila(solucaoMelhorLocal);
			}
			solucaoMelhorLocal = solucaoPerturbada;
		}
	


		for(int i = 0; i < solucaoPerturbada->itensTotal + 1; i++){
			if(solucaoMelhorLocal->valorTotal < memoria[posVizinhos[i]]->valorTotal){
				if(solucaoMelhorLocal->id == -1){
					//DestruirMochila(solucaoMelhorLocal);
				}
				solucaoMelhorLocal = memoria[posVizinhos[i]];
			}
		}			

		free(posVizinhos);
		

		// Atualiza contador de iterações sem melhora
		if(solucaoMelhorLocal->valorTotal <= solucaoMelhorGeral->valorTotal){
			iteracoesSemMelhora++;
			contPert++;
		}else{
			iteracoesSemMelhora = 0;
			contPert = 0;
			if(solucaoMelhorLocal->id == -1){
				//DestruirMochila(solucaoMelhorLocal);
			}
			solucaoMelhorGeral = solucaoMelhorLocal;
		}

		// Perturbação
		if(contPert == controlePerturbacao){
			contPert = 0;
			solucaoPerturbada = Pertubacao(solucaoMelhorGeral, memoria, capacidadeMochila);
		}else{
			solucaoPerturbada = Pertubacao(solucaoMelhorLocal, memoria, capacidadeMochila);
		}
		
		// Seleciona o menor indice (onde valor/peso é maior) da solução perturbada para usar como base
		itemAtual = solucaoPerturbada->listaItens;	
		for (int i = 0; i < solucaoPerturbada->itensTotal; i++){
			if(i == 0){
				indicePerturbado = itemAtual->itemPtr->indice;
			}else{
				if(indicePerturbado > itemAtual->itemPtr->indice){
					indicePerturbado = itemAtual->itemPtr->indice;
				}
			}			
			itemAtual = itemAtual->proximo;
		}
		

		//solucaoPerturbada = BuscaLocal(solucaoNova, indicePerturbado, conjuntoCandidatos, matrizAdj, memoria, quantidadeItens, capacidadeMochila, iteCont);
		if(solucaoPerturbada->valorTotal > solucaoMelhorLocal->valorTotal){
			if(solucaoMelhorLocal->id == -1){
				//DestruirMochila(solucaoMelhorLocal);
			}
			solucaoMelhorLocal = solucaoPerturbada;
		}
	}
	return solucaoMelhorGeral;
}

Mochila* Pertubacao(Mochila* solucao, Mochila** memoria, int capacidade){ 	

	srand(time(NULL));
	//int pertTotal = (int)(solucao->itensTotal * GRAU_PERTURBACAO) + 1;	
	int pertTotal = ceil(solucao->itensTotal * GRAU_PERTURBACAO);	
	int naoPerturbados = solucao->itensTotal - pertTotal;
	int repetido;
	ItemNode *itemAtual, *vizinhoAtual, *itemSolucao;
	// A solução tem índice -1 pois não pertence à memória, não será salva 
	Mochila* novaSolucao = CriarMochila(-1);

	
	// Escolhe índices que não serão perturbados
	int* indicesNaoPerturbados = (int*)malloc(sizeof(int)*naoPerturbados);
	if(!indicesNaoPerturbados){
		printf("Malloc falhou em Perturcao");
		exit(0);
	}
	for(int i = 0; i < naoPerturbados; i++){
		int indice = rand()%solucao->itensTotal;
		repetido = 0;
		for(int j = i - 1; j >= 0; j--){
			if(indicesNaoPerturbados[j] == indice){
				i--;
				repetido = 1;
				break;
			}
		}
		if(!repetido){
			indicesNaoPerturbados[i] = indice;
		}
	}
	
	// Criar uma solução com os índices não perturbados
	for(int i = 0; i < naoPerturbados; i++){
		itemAtual = solucao->listaItens;
		for(int j = 0; j < indicesNaoPerturbados[i]; j++){
			itemAtual = itemAtual->proximo;
		}
		AdicionarItem(novaSolucao, itemAtual->itemPtr);
	}	
	free(indicesNaoPerturbados);

	// Adiciona itens aleatórios da vizinhança 
	// Sai do loop se não conseguir encontrar itens para adicionar durante N iterações
	int naoEncontrou = 0;
	while(naoEncontrou <= LIMITE_ITERACOES_SEM_MELHORA){ 

		// Escolhe qual vizinho usar
		int vizinhoAleatorio = rand()%solucao->itensTotal;
		vizinhoAtual = NULL; // Só pra ter certeza que estava vazio
		vizinhoAtual = solucao->listaItens;
		for(int j = 0; j < vizinhoAleatorio; j++){
			vizinhoAtual = vizinhoAtual->proximo;
		}		

		// Escolhe um item do vizinho
		int totalItensVizinho = memoria[vizinhoAtual->itemPtr->indice]->itensTotal;		
		for(int j = 0; j < totalItensVizinho; j++){ // (j < totalItensVizinho) pra evitar loop infinito
			int itemAleatorio = rand()%totalItensVizinho;
			
			itemAtual = memoria[vizinhoAtual->itemPtr->indice]->listaItens;
			for(int k = 0; k < itemAleatorio; k++){
				itemAtual = itemAtual->proximo;
			}

			// Verifica se a solução já tem o item escolhido
			repetido = 0;
			itemSolucao = novaSolucao->listaItens;
			for(int k = 0; k < novaSolucao->itensTotal; k++){
				if(itemSolucao->itemPtr->indice == itemAtual->itemPtr->indice){
					repetido = 1;
					break;
				}
				itemSolucao = itemSolucao->proximo;
			}

			if(repetido){
				continue;
			}else{
				// Verifica se a capacidade da mochila comporta o novo item antes de adicionar			
				if((novaSolucao->pesoTotal + itemAtual->itemPtr->peso) <= capacidade){
					AdicionarItem(novaSolucao, itemAtual->itemPtr);
					naoEncontrou = 0;
					break;
				}		
			}
		}
		naoEncontrou++;		
	}
	return novaSolucao;	
}

// Encontra (e guarda na memória) a solução de cada vizinho utilizando guloso
// Usa a matriz de adjacência para montar a solução
void BuscaNoVizinho(Item* conjuntoCandidatos, int indicePivo, unsigned char** matrizAdj, Mochila** memoria, int quantidadeItens, int capacidadeMochila){
	/* 	LEMBRAR:
	* 	- INCLUIR O PIVO NA SOLUÇÃO 
	* 	- COLUNA -> PIVO -> LINHA (Maior valor agregado)
	*/

	// Criar solução e adicionar à memória
	Mochila* solucaoPivo = CriarMochila(indicePivo);
	memoria[indicePivo] = solucaoPivo;

	// Buscar vizinhos na coluna do item
	for(int i = 0; i < indicePivo; i++){
		int col = indicePivo - 1 - i;
		if(matrizAdj[i][col/8] & 1 << col%8){
			// Se couber na mochila, adiciona o item
			if((solucaoPivo->pesoTotal + conjuntoCandidatos[i].peso) < capacidadeMochila){
				AdicionarItem(solucaoPivo, &conjuntoCandidatos[i]);
			}			
		}
	}
	
	// Testa se o pivo é viável
	if((solucaoPivo->pesoTotal + conjuntoCandidatos[indicePivo].peso) < capacidadeMochila){
		AdicionarItem(solucaoPivo, &conjuntoCandidatos[indicePivo]);
	}	

	// Buscar vizinhos na linha do item
	for(int i = 0; i < (quantidadeItens - 1) - indicePivo; i++){
		if((matrizAdj[indicePivo][i/8] & 1 << i%8)){
			// Se couber na mochila, adiciona o item
			if((solucaoPivo->pesoTotal + conjuntoCandidatos[i + 1 + indicePivo].peso) < capacidadeMochila){
				AdicionarItem(solucaoPivo, &conjuntoCandidatos[i + 1 + indicePivo]);
			}			
		}
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

#define TAMANHO_VIZINHANCA 9 // Está ao quadrado, na prática equivale a sqrt(TAMANHO_VIZINHANCA)
#define LIMITE_ITERACOES_SEM_MELHORA 20

typedef struct _item Item;
typedef struct _itemNode ItemNode;
typedef struct _mochila Mochila;
typedef struct _vetor2 Vetor2;

struct _item{
	int indice; 
	int peso;
	int valor;	
	float proporcao;
};

struct _itemNode{
	Item* itemPtr;
	ItemNode* proximo;
};

struct _mochila{
	int itensTotal;
	int valorTotal;
	int pesoTotal;
	ItemNode* listaItens;
};

struct _vetor2{
	int x;
	int y;
};

// Protótipos ILS_Base
Item* LerArquivo(FILE* arquivo, int quantidade);
void CopiarItem(Item* destino, Item* origem);
void AdicionarItem(Mochila* mochila, Item* item);
void DestruirMochila(Mochila* mochila);
void ImprimirMochila(Mochila* mochila);

//Protótipos Quicksort
void OrdenarCandidatos(Item *vet, int quant, Vetor2* vetorMedia);
void Quicksort(Item *vet, int ini, int fim);
int Particiona(Item *vet, int ini, int fim);

// Protótipos ILS_Simples
int DistanciaQuadrada(Vetor2 v1, Vetor2 v2);
Mochila* SolucaoInicalGulosa(Item* candidatos, int quantidade, int capacidade);
Mochila* SolucaoInicialAleatoria(Item* candidatos, int quantidade, int capacidade);
unsigned char** CriarMatrizAdjacencia(int quantidade);
unsigned char** GerarVizinhanca(Item* candidatos, int raioMax, int quant);
static void _imprimirMatriz(unsigned char** mat, int quant); // Somente para teste

// Protótipos ILS_Otimizado

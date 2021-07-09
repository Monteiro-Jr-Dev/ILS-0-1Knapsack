typedef struct _item Item;
typedef struct _itemNode ItemNode;
typedef struct _mochila Mochila;

struct _item{
	int peso;
	int valor;	
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

// Protótipos ILS_Base
Item* LerArquivo(FILE* arquivo, int quantidade);
Mochila* SolucaoInicialAleatoria(Item* candidatos, int quantidade, int capacidade);
Mochila* SolucaoInicalGulosa(Item* candidatos, int quantidade, int capacidade);
Mochila* CriarMochilaOrdenada(Item* candidatos, int quantidade);
void AdicionarItem(Mochila* mochila, Item* item);
void AdicionarItemOrdenado(Mochila* mochila, Item* item);
void DestruirMochila(Mochila* mochila);
void ImprimirMochila(Mochila* mochila);

// Protótipos ILS_Simples

// Protótipos ILS_Otimizado

/****************/
/* SUFFIX TREES */
/****************/

// estrutura para a árvore de sufixos
typedef struct _treenode {
	char label;						// rótulo do nó
	int depth;						// tamanho do prefixo desde a raíz até este nó (profundidade)
	int count;						// número de vezes que este sufixo aparece no texto
	struct _treenode *suffixlink;	// apontador para o nó cujo rótulo é o sufixo da label deste nó
	struct _treenode **branches;	// ramos do nó
	int numbranches;				// número de ramos do nó
	struct _treenode *sidelink;		// apontador para o próximo nó criado na mesma profundidade
	//struct _treenode *backlink;		// apontador para o nó anterior acima
} treenode;

// devolve um apontador para a raiz da árvore de sufixos
treenode * getSuffixTreeRoot();

// devolve um apontador para o vector de sidelinks
treenode ** getSideLinksVector();

// devolve o nó no qual termina o caminho descrito pela string e profundidade dadas
treenode *followInTree(char *string, int depth);

// devolve a contagem (...)
int getCountByFollowInTree(char *string, int depth);

// devolve o vector das contagens de um ramo inteiro na árvore
treenode *getCountsVector(int pos, int *countsvector, treenode *start);

// devolve o vector das contagens dos nós desde o nó actual até à raíz (seguindo os suffixlinks)
void getBackCountsVector(treenode *startnode, int *countsvector);

// cria um novo nó
treenode * newNode(char label,int depth);

// adiciona um novo ramo a um nó já existente
treenode * addBranch(treenode *node, char label);

// devolve o ramo que tem o rótulo c
treenode * getBranchOfChar(treenode *node, char c);

// actualiza os contadores dos sufixos desde o nó actual (inclusivé) até à raíz
void updatePreviousCounts(treenode *node);

// constrói a árvore dos sufixos de tamanho máximo igual ao definido
treenode * buildTree(char *txt, int limit, int *stepsoutput);

// devolve o número de vezes que a substring aparece na string
int getSubStringCount(treenode *node, char *substring);

// imprime a estrutura da árvore em formato reduzido
void printSuffixTree(treenode *node,int pos);

// desenha a árvore de sufixos em formato detalhado
void drawSuffixTree(FILE *output, treenode *node, int totalnodes, int nodenumber, char *linestart, char *countslinestart);

// salva o desenho da árvore num ficheiro
int printTreeToFile(treenode *node, char * filename);

// guarda a árvore para um ficheiro
void saveTreeToFile(char *filename);

// carrega a árvore de um ficheiro
int loadTreeFromFile(char *filename);

// define o apontador para o texto que originou a árvore
void setTreeText(char *txt);

// liberta a memória alocada pela árvore
void freeTreeNode(treenode *node);

// devolve o tamanho da memória ocupada pela árvore
int getTreeSize(treenode *node);

// devolve o número total de nós da árvore
int getNumberOfTreeNodes(treenode *node);

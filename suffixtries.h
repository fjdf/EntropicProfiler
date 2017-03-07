/****************/
/* SUFFIX TREES */
/****************/

// estrutura para a �rvore de sufixos
typedef struct _treenode {
	char label;						// r�tulo do n�
	int depth;						// tamanho do prefixo desde a ra�z at� este n� (profundidade)
	int count;						// n�mero de vezes que este sufixo aparece no texto
	struct _treenode *suffixlink;	// apontador para o n� cujo r�tulo � o sufixo da label deste n�
	struct _treenode **branches;	// ramos do n�
	int numbranches;				// n�mero de ramos do n�
	struct _treenode *sidelink;		// apontador para o pr�ximo n� criado na mesma profundidade
	//struct _treenode *backlink;		// apontador para o n� anterior acima
} treenode;

// devolve um apontador para a raiz da �rvore de sufixos
treenode * getSuffixTreeRoot();

// devolve um apontador para o vector de sidelinks
treenode ** getSideLinksVector();

// devolve o n� no qual termina o caminho descrito pela string e profundidade dadas
treenode *followInTree(char *string, int depth);

// devolve a contagem (...)
int getCountByFollowInTree(char *string, int depth);

// devolve o vector das contagens de um ramo inteiro na �rvore
treenode *getCountsVector(int pos, int *countsvector, treenode *start);

// devolve o vector das contagens dos n�s desde o n� actual at� � ra�z (seguindo os suffixlinks)
void getBackCountsVector(treenode *startnode, int *countsvector);

// cria um novo n�
treenode * newNode(char label,int depth);

// adiciona um novo ramo a um n� j� existente
treenode * addBranch(treenode *node, char label);

// devolve o ramo que tem o r�tulo c
treenode * getBranchOfChar(treenode *node, char c);

// actualiza os contadores dos sufixos desde o n� actual (inclusiv�) at� � ra�z
void updatePreviousCounts(treenode *node);

// constr�i a �rvore dos sufixos de tamanho m�ximo igual ao definido
treenode * buildTree(char *txt, int limit, int *stepsoutput);

// devolve o n�mero de vezes que a substring aparece na string
int getSubStringCount(treenode *node, char *substring);

// imprime a estrutura da �rvore em formato reduzido
void printSuffixTree(treenode *node,int pos);

// desenha a �rvore de sufixos em formato detalhado
void drawSuffixTree(FILE *output, treenode *node, int totalnodes, int nodenumber, char *linestart, char *countslinestart);

// salva o desenho da �rvore num ficheiro
int printTreeToFile(treenode *node, char * filename);

// guarda a �rvore para um ficheiro
void saveTreeToFile(char *filename);

// carrega a �rvore de um ficheiro
int loadTreeFromFile(char *filename);

// define o apontador para o texto que originou a �rvore
void setTreeText(char *txt);

// liberta a mem�ria alocada pela �rvore
void freeTreeNode(treenode *node);

// devolve o tamanho da mem�ria ocupada pela �rvore
int getTreeSize(treenode *node);

// devolve o n�mero total de n�s da �rvore
int getNumberOfTreeNodes(treenode *node);

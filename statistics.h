
/**************/
/* STATISTICS */
/**************/

// inicializa as variáveis estatísticas para o texto dado
void initializeStatisticsForText(char *txt, int n);

// inicializa as variáveis estatísticas usando a árvore de sufixos dada
void initializeStatisticsFromTree();

// liberta a memória alocada pelas variáveis estatísticas
void freeStatistics();

// mostra os detalhes dos valores das estatísticas do texto
void printTextStatistics();

// calcula o p-value para a palavra dada
double getPValueForWord(char *w, int wn, int noccs);

// calcula o z-score para a palavra dada
double getZScoreForWord(char *w, int wn, int noccs);

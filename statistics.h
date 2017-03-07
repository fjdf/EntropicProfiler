
/**************/
/* STATISTICS */
/**************/

// inicializa as vari�veis estat�sticas para o texto dado
void initializeStatisticsForText(char *txt, int n);

// inicializa as vari�veis estat�sticas usando a �rvore de sufixos dada
void initializeStatisticsFromTree();

// liberta a mem�ria alocada pelas vari�veis estat�sticas
void freeStatistics();

// mostra os detalhes dos valores das estat�sticas do texto
void printTextStatistics();

// calcula o p-value para a palavra dada
double getPValueForWord(char *w, int wn, int noccs);

// calcula o z-score para a palavra dada
double getZScoreForWord(char *w, int wn, int noccs);

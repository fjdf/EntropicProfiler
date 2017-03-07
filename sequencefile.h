/************************/
/* SEQUENCE FILE FORMAT */
/************************/

// estrutura para guardar a sequência
typedef struct _sequence {
	char * description;	// informação acerca da sequência
	char * data;		// sequência de caracteres
	long size;			// número de caracteres
} sequence;

// cria uma nova sequência
sequence * newSequence(char *desc, char *data);

// carrega a sequência a partir de um ficheiro
sequence * loadSequence(char * path);

// mostra os dados da sequência
void printSequence(sequence *seq);

// liberta a memória alocada pela sequência
void freeSequence(sequence *seq);

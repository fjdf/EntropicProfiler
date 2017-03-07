/************************/
/* SEQUENCE FILE FORMAT */
/************************/

// estrutura para guardar a sequ�ncia
typedef struct _sequence {
	char * description;	// informa��o acerca da sequ�ncia
	char * data;		// sequ�ncia de caracteres
	long size;			// n�mero de caracteres
} sequence;

// cria uma nova sequ�ncia
sequence * newSequence(char *desc, char *data);

// carrega a sequ�ncia a partir de um ficheiro
sequence * loadSequence(char * path);

// mostra os dados da sequ�ncia
void printSequence(sequence *seq);

// liberta a mem�ria alocada pela sequ�ncia
void freeSequence(sequence *seq);

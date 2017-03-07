#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "statistics.h"
#include "suffixtries.h"
//#include "sequencefile.h"

#define NUMPOS 4
#define APOS 0
#define CPOS 1
#define GPOS 2
#define TPOS 3


/*********************/
/* VARIÁVEIS GLOBAIS */
/*********************/

static char *text = NULL; // sequência
static char *word = NULL; // palavra
static int textsize = 0; // número de letras na sequência
static int wordsize = 0; // número de letras na palavra

static int *lettercounts = NULL; // número de ocorrências das letras no texto
static int **paircounts = NULL; //  número de ocorrências dos pares de letras no texto
static double *staticprobs = NULL; // probabilidades estáticas
static double **transitionprobs = NULL; // probabilidades de transição

static double mean = 0.0; // valor esperado do número de ocorrências da palavra
static double var = 1.0; // variância do número de ocorrências da palavra

static int **countsinsideword = NULL; // número de ocorrências dos pares de letras dentro da palavra
static int *periodset = NULL; // conjunto dos períodos da palavra
static int numberofperiods = 0; // número de períodos da palavra


// inicializa as tabelas de contagens e probabilidades a zero
void initializeStatistics(){
	int i;
	lettercounts=(int *)calloc(4,sizeof(int));
	paircounts=(int **)malloc(4*sizeof(int *));
	staticprobs=(double *)calloc(4,sizeof(double));
	transitionprobs=(double **)malloc(4*sizeof(double *));
	countsinsideword=(int **)malloc(4*sizeof(int *));
	for(i=0;i<4;i++){
		paircounts[i]=(int *)calloc(4,sizeof(int));
		transitionprobs[i]=(double *)calloc(4,sizeof(double));
		countsinsideword[i]=(int *)calloc(4,sizeof(int));
	}
}

// liberta a memória alocada pelas variáveis estatísticas
void freeStatistics(){
	int i;
	//free(word);
	//free(periodset);
	free(lettercounts);
	free(staticprobs);
	for(i=0;i<4;i++){
		free(paircounts[i]);
		free(transitionprobs[i]);
		free(countsinsideword[i]);
	}
	free(paircounts);
	free(transitionprobs);
	free(countsinsideword);
}

// devolve a posição da letra nas tabelas
int letterPos(char x){
	if(x=='a' || x=='A') return 0;
	if(x=='c' || x=='C') return 1;
	if(x=='g' || x=='G') return 2;
	if(x=='t' || x=='T') return 3;
	return -1;
}

// devolve a letra da posição nas tabelas
char posLetter(int x){
	if(x==0) return 'A';
	if(x==1) return 'C';
	if(x==2) return 'G';
	if(x==3) return 'T';
	return '\0';
}

// incrementa a contagem da letra
void updateLetterCount(char a){
	int pos=letterPos(a);
	if(pos!=-1) lettercounts[pos]++;
}

// incrementa a contagem do par de letras
void updatePairCount(char a, char b){
	int posa,posb;
	posa=letterPos(a);
	posb=letterPos(b);
	if(posa==-1 || posb==-1) return;
	(paircounts[posa][posb])++;
}

// incrementa a contagem da segunda letra e do par de letras
void updateCount(char a, char b){
	int posa,posb;
	posa=letterPos(a);
	posb=letterPos(b);
	if(posb==-1) return;
	(lettercounts[posb])++;
	if(posa==-1) return;
	(paircounts[posa][posb])++;
}

// incrementa a contagem da letra dada a sua posição
void updateLetterCountByPos(int pos){
	if(pos!=-1) lettercounts[pos]++;
}

// incrementa a contagem da segunda letra e do par de letras dadas as suas posições
void updateCountByPos(int posa, int posb){
	if(posb==-1) return;
	(lettercounts[posb])++;
	if(posa==-1) return;
	(paircounts[posa][posb])++;
	
}

// preenche as tabelas de probabilidades estáticas e de transição
void calculateProbabilityTables(){
	int i,j;
	double sum,ntotal;
	ntotal=(double)textsize;
	for(i=0;i<4;i++){
		staticprobs[i]=(lettercounts[i])/(ntotal);
		sum=0.0;
		for(j=0;j<4;j++)
			sum+=(double)(paircounts[i][j]);
		for(j=0;j<4;j++)
			transitionprobs[i][j]=(paircounts[i][j])/(sum);
	}
}

// faz um estudo estatístico do texto (ocorrências de letras e de pares de letras)
void studyText(char *sequence, int length){
	int i,curr,next;
	text=sequence;
	textsize=length;
	curr=letterPos(text[0]);
	updateLetterCountByPos(curr);
	for(i=0;i<=(textsize-2);i++){
		next=letterPos(text[i+1]);
		//if(curr==-1 || next==-1) printf("text[%d,%d]='%c%c'\n",i,i+1,text[i],text[i+1]);
		updateCountByPos(curr,next);
		curr=next;
	}
	calculateProbabilityTables();
}

// devolve a sobreposição da palavra sobre si própria na posição p
char *overlappedWord(int p){
	char *overlap;
	overlap=(char *)malloc((wordsize+p+1)*sizeof(char));
	strncpy(overlap,word,p);
	overlap[p]='\0';
	strcat(overlap,word);
	return overlap;
}

// mostra os detalhes dos valores das estatísticas do texto
void printTextStatistics(){
	int i,j;
	char *chars;
	int *intsum;
	double *doublesum;
	intsum=(int *)calloc(4,sizeof(int));
	doublesum=(double *)calloc(4,sizeof(double));
	chars=(char *)malloc(4*sizeof(char));
	chars[0]='A';
	chars[1]='C';
	chars[2]='G';
	chars[3]='T';
	printf("size = %d\n\n",textsize);
	printf("[A]\t[C]\t[G]\t[T]\n");
	intsum[0]=0;
	for(i=0;i<4;i++){
		printf("%d\t",lettercounts[i]);
		intsum[0]+=lettercounts[i];
	}
	printf("= %d\n\n",intsum[0]);
	intsum[0]=0;
	printf("[A]\t[C]\t[G]\t[T]\n");
	doublesum[0]=0.0;
	for(i=0;i<4;i++){
		printf("%.5f\t",staticprobs[i]);
		doublesum[0]+=staticprobs[i];
	}
	printf("= %f\n\n",doublesum[0]);
	doublesum[0]=0.0;
	printf("\t[A]\t[C]\t[G]\t[T]\n");
	for(i=0;i<4;i++){
		printf("[%c]\t",chars[i]);
		for(j=0;j<4;j++){
			printf("%d\t",paircounts[i][j]);
			intsum[i]+=paircounts[i][j];
		}
		printf("= %d\n",intsum[i]);
	}
	printf("\t\t\t\t\t= %d\n",(intsum[0]+intsum[1]+intsum[2]+intsum[3]));
	printf("\t[A]\t[C]\t[G]\t[T]\n");
	for(i=0;i<4;i++){
		printf("[%c]\t",chars[i]);
		for(j=0;j<4;j++){
			printf("%.5f\t",transitionprobs[i][j]);
			doublesum[i]+=transitionprobs[i][j];
		}
		printf("= %f\n",doublesum[i]);
	}
	printf("\t\t\t\t\t= %f\n",(doublesum[0]+doublesum[1]+doublesum[2]+doublesum[3]));
}


/************/
/*          */
/************/


// FALTA: reestruturar código
// calcula a função de distribuição de uma Normal(0,1)
double NormalCDF(double x){
	const double b1 =  0.319381530;
	const double b2 = -0.356563782;
	const double b3 =  1.781477937;
	const double b4 = -1.821255978;
	const double b5 =  1.330274429;
	const double p  =  0.2316419;
	const double c  =  0.39894228;
	double t = 0.0;
	if(x >= 0.0){
		t = 1.0 / ( 1.0 + p * x );
		return (1.0 - c * exp( -x * x / 2.0 ) * t *	( t *( t * ( t * ( t * b5 + b4 ) + b3 ) + b2 ) + b1 ));
	}
	t = 1.0 / ( 1.0 - p * x );
	return ( c * exp( -x * x / 2.0 ) * t * ( t *( t * ( t * ( t * b5 + b4 ) + b3 ) + b2 ) + b1 ));
}

void testNormalCDF(){
	double i;
	for(i=-4.0;i<=4.0;i+=0.1) printf("NormalCDF( %.2f ) = %f\n",i,NormalCDF(i));
}

// calcula o p-value
double PValue(int t){
	if(var<=0.0) return 0.0;
	return ( 1.0 - NormalCDF( ( ((double)t) - mean ) / sqrt(var) ) );
}

// calcula o z-score
double ZScore(int t){
	if(var<=0.0) return 0.0;
	return ( ( ((double)t) - mean ) / sqrt(var) );
}

double N(int pos){
	//return staticprobs[pos]; // Miu
	return ( (double)(lettercounts[pos]) );
}

double NOfPair(int pos1, int pos2){
	//return ((staticprobs[pos1])*(transitionprobs[pos1][pos2])); // MiuMOfPair
	return ( (double)(paircounts[pos1][pos2]) );
}

double EMN(char *w, int wn){
	/*
	int i,pos,nextpos;
	double result;
	pos=letterPos(w[0]);
	if(pos==-1) return 0.0;
	result=staticprobs[pos];
	for(i=0;i<=(wn-2);i++){
		nextpos=letterPos(w[i+1]);
		if(nextpos==-1) return 0.0;
		result=result*(transitionprobs[pos][nextpos]);
		pos=nextpos;
	}
	return result; // MiuM
	*/
	int i,pos,nextpos;
	double up,down;
	up=1.0;
	nextpos=letterPos(w[0]);
	for(i=0;i<=(wn-2);i++){
		pos=nextpos;
		nextpos=letterPos(w[i+1]);
		up= up * ( (double)(paircounts[pos][nextpos]) );
	}
	down=1.0;
	for(i=1;i<=(wn-2);i++){
		pos=letterPos(w[i]);
		down= down * ( (double)(lettercounts[pos]) );
	}
	return ( up / down );
}

int NW(int pos1, int pos2){ // NumOccInsW
	return countsinsideword[pos1][pos2];
}

int NWPlus(int pos){ // NumOccInsWDot
	int i,total;
	total=0;
	for(i=0;i<4;i++) total+=countsinsideword[pos][i];
	return total;
}

double ExpectedValue(){
	int i,pos,nextpos;
	double up,down;
	//return ((double)(textsize-wordsize+1))*MiuM(word,wordsize); // EN
	up=1.0;
	nextpos=letterPos(word[0]);
	for(i=0;i<=(wordsize-2);i++){
		pos=nextpos;
		nextpos=letterPos(word[i+1]);
		up= up * ( (double)(paircounts[pos][nextpos]) );
	}
	down=1.0;
	for(i=1;i<=(wordsize-2);i++){
		pos=letterPos(word[i]);
		down= down * ( (double)(lettercounts[pos]) );
	}
	return ( up / down );
}

double Variance(){
	int i,j,p,ncount;
	double miumw,sumperiods,sumnmdot,sumnmplusone,fractionnmdot,n;
	char *overlap;
	miumw=EMN(word,wordsize); // MiuM
	sumperiods=0.0;
	for(i=0;i<numberofperiods;i++){
		p=periodset[i];
		overlap=overlappedWord(p);
		sumperiods+=EMN(overlap,(wordsize+p)); // MiuM
		free(overlap);
	}
	sumnmdot=0.0;
	for(i=0;i<4;i++){
		ncount=NWPlus(i); // NumOccInsWDot
		n=N(i); // Miu
		if(n!=0) sumnmdot+=((double)(ncount*ncount))/n;
	}
	sumnmplusone=0.0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			ncount=NW(i,j); // NumOccInsW
			n=NOfPair(i,j); // MiuMOfPair
			if(n!=0) sumnmplusone+=((double)(ncount*ncount))/n;
		}
	}
	fractionnmdot=0.0;
	i=letterPos(word[0]);
	if(i==-1) return 0.0;
	ncount=NWPlus(i); // NumOccInsWDot
	fractionnmdot=((double)(1-2*ncount))/(N(i)); // Miu
	return ( miumw + 2.0*sumperiods + (miumw*miumw)*(sumnmdot - sumnmplusone + fractionnmdot) );
}

// faz um estudo estatístico da palavra dentro do texto (períodos e ocorrências de pares)
void studyWord(char *pattern, int length){
	int i,j,p,pos,nextpos,charcmp;
	word=(char *)malloc((length+1)*sizeof(char));
	strncpy(word,pattern,length);
	word[length]='\0';
	wordsize=length;
	mean=0.0;
	var=1.0;
	numberofperiods=0;
	periodset=(int *)malloc(wordsize*sizeof(int));
	for(p=1;p<=(wordsize-3)+1;p++){
		i=0;
		while((charcmp=(word[p+i]==word[i]))){
			i++;
			if(i>(wordsize-p-1)) break;
		}
		if(charcmp){
			periodset[numberofperiods]=p;
			numberofperiods++;
		}
	}
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			countsinsideword[i][j]=0;
	pos=letterPos(word[0]);
	if(pos==-1) return;
	for(i=0;i<=(wordsize-2);i++){
		nextpos=letterPos(word[i+1]);
		if(nextpos==-1) return;
		countsinsideword[pos][nextpos]++;
		pos=nextpos;
	}
	mean=ExpectedValue();
	var=Variance();
}

// mostra os detalhes dos valores das estatísticas da palavra
void printWordStatistics(){
	int i,j;
	char *chars;
	chars=(char *)malloc(4*sizeof(char));
	chars[0]='A';
	chars[1]='C';
	chars[2]='G';
	chars[3]='T';
	printf("word = %s\nsize = %d\n\n",word,wordsize);
	for(i=0;i<numberofperiods;i++)
		printf("p = %d : %s\n",periodset[i],overlappedWord(periodset[i]));
	printf("\n");
	for(i=0;i<4;i++){
		for(j=0;j<4;j++)
			printf("%c%c = %d\t",chars[i],chars[j],countsinsideword[i][j]);
		printf("\n");
	}
	printf("\n");
	printf("mean     = %f\n",mean);
	printf("variance = %f\n",var);
}

// inicializa as variáveis estatísticas usando a árvore de sufixos dada
void initializeStatisticsFromTree(){
	int i,j;
	char *string;
	treenode *tree=getSuffixTreeRoot();
	string=(char *)malloc(3*sizeof(char));
	string[2]='\0';
	text=NULL;
	textsize=tree->count;
	initializeStatistics();
	for(i=0;i<4;i++){
		string[0]=posLetter(i);
		string[1]='\0';
		lettercounts[i]=getSubStringCount(tree,string);
		//printf("#'%s' =%d\n",string,lettercounts[i]);
		for(j=0;j<4;j++){
			string[1]=posLetter(j);
			paircounts[i][j]=getSubStringCount(tree,string);
			//printf("#'%s'=%d\n",string,paircounts[i][j]);
		}
	}
	free(string);
	calculateProbabilityTables();
}

// inicializa as variáveis estatísticas para o texto dado
void initializeStatisticsForText(char *txt, int n){
	initializeStatistics();
	studyText(txt,n);
}

// calcula o p-value para a palavra dada
double getPValueForWord(char *w, int wn, int noccs){
	studyWord(w,wn);
	free(word);
	free(periodset);
	return PValue(noccs);
}

// calcula o z-score para a palavra dada
double getZScoreForWord(char *w, int wn, int noccs){
	studyWord(w,wn);
	free(word);
	free(periodset);
	return ZScore(noccs);
}

/********/
/* MAIN */
/********/

/*
int main(int argc, char *argv[]){
	sequence *s;
	treenode *tree;
	int slen;
	char file[255]="";
	char string[255]="";
	if(argc<3){
		printf("> file : ");
		scanf("%255s",file);
		printf("> word : ");
		scanf("%255s",string);
	}
	else{
		strcpy(file,argv[1]);
		strcpy(string,argv[2]);
	}
	s=loadSequence(file);
	if(s==NULL) return 0;

	//tree=buildTree(s->data,10,&slen);
	//initializeStatisticsFromTree(tree);
	//printTextStatistics();
	//freeStatistics();

	initializeStatistics();
	studyText(s->data,s->size);
	printTextStatistics();
	while(1){
		slen=0;
		while(string[slen]!='\0' && string[slen]!='\n') slen++;
		if(slen<=1) break;
		studyWord(string,slen);
		printWordStatistics();
		printf("\n> word : ");
		scanf("%255s",string);

	}
	freeStatistics();
}
*/

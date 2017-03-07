#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sequencefile.h"
#include "suffixtries.h"
#include "bitmap.h"
#include "graphics.h"
#include "3dgraphics.h"
#include "statistics.h"

#define LIMIT 10

static char type = '\0';
static char *filename = NULL;
static char *sequencetext = NULL;
static sequence *sequencefromfile = NULL;
static int sequencesize = 0;
static int lvalue = 0;
static int lmaxvalue = 0;
static int phivalue = 0;
static int position = 0;
static int positionwindow = 0;

static double *fvalueslist = NULL;
static double mteta = 0.0;
static double steta = 0.0;

static treenode *suffixtree = NULL;
static int *substringscount = NULL;
static double *xpointslist = NULL;
static double *ypointslist = NULL;
static int numberofpoints = 0;

static double *pvalueslist = NULL;
static double *zscoreslist = NULL;
static int *countslist = NULL;
static char **substringslist = NULL;

static double *averageslist = NULL;
static double *varianceslist = NULL;


// converte um double para um int
int d2i(double n){
	return (int)floor(n);
}

// devolve o m�imo de dois inteiros
int intmax(int x, int y){
	if(x>y) return x;
	return y;
}

// devolve o m�imo de dois inteiros
int intmin(int x, int y){
	if(x<y) return x;
	return y;
}

// devolve uma mensagem de erro e sai do programa
void exitError(char *errordesc){
	printf("Error: %s\n",errordesc);
	exit(-1);
}

// liberta a mem�ia alocada pelas vari�eis usadas no c�culo dos perfis entr�icos
void freeVariables(){
	int i;
	if(xpointslist!=NULL) free(xpointslist);
	if(ypointslist!=NULL) free(ypointslist);
	if(substringscount!=NULL) free(substringscount);
	if(pvalueslist!=NULL) free(pvalueslist);
	if(zscoreslist!=NULL) free(zscoreslist);
	if(countslist!=NULL) free(countslist);
	if(substringslist!=NULL) {
		for(i=0;i<numberofpoints;i++) free(substringslist[i]);
		free(substringslist);
	}
}

// mostra a lista de pontos
void printPoints(){
	int i;
	for(i=0;i<numberofpoints;i++){
		printf("(%d,%f)\n",d2i(xpointslist[i]),ypointslist[i]);
	}
}

// salva os valores da m�ia e vari�cia para um ficheiro
void saveDataToFile(){
	int i,n;
	char *datafilename;
	FILE *fileout;
	if(averageslist==NULL || varianceslist==NULL) return;
	n=(int)strlen(filename);
	datafilename=(char *)malloc((n+6)*sizeof(char));
	strncpy(datafilename,filename,n);
	datafilename[n]='\0';
	strncat(datafilename,".data",5);
	datafilename[n+5]='\0';
	fileout=fopen(datafilename,"w");
	if(fileout==NULL) return;
	for(i=1;i<=LIMIT;i++) fprintf(fileout,"%f %f \n",averageslist[i],varianceslist[i]);
	fclose(fileout);
	free(datafilename);
}

// carrega os valores da m�ia e vari�cia a partir de um ficheiro
void loadDataFromFile(){
	int i,n;
	char *datafilename;
	FILE *filein;
	n=(int)strlen(filename);
	datafilename=(char *)malloc((n+6)*sizeof(char));
	strncpy(datafilename,filename,n);
	datafilename[n]='\0';
	strncat(datafilename,".data",5);
	datafilename[n+5]='\0';
	filein=fopen(datafilename,"r");
	if(filein==NULL) return;
	averageslist=(double *)calloc((LIMIT+1),sizeof(double));
	varianceslist=(double *)calloc((LIMIT+1),sizeof(double));
	for(i=1;i<=LIMIT;i++){
		fscanf(filein,"%lf %lf",&averageslist[i],&varianceslist[i]);
		//printf("%lf %lf\n",averageslist[i],varianceslist[i]);
	}
	fclose(filein);
	free(datafilename);
}

// salva a descri�o da sequ�cia para um ficheiro
void saveDescriptionToFile(){
	int n;
	char *descfilename;
	FILE *fileout;
	if(sequencefromfile==NULL) return;
	n=(int)strlen(filename);
	descfilename=(char *)malloc((n+6)*sizeof(char));
	strncpy(descfilename,filename,n);
	descfilename[n]='\0';
	strncat(descfilename,".desc",5);
	descfilename[n+5]='\0';
	fileout=fopen(descfilename,"w");
	if(fileout==NULL) return;
	fprintf(fileout,"%s",sequencefromfile->description);
	fclose(fileout);
	free(descfilename);
}
void printBinary(unsigned int x){
	int i;
	unsigned int zero;
	unsigned int one;
	zero=(unsigned int)0;
	one=(unsigned int)1;
	for(i=(LIMIT-1);i>=0;i--){
		if( (( x >> i) & one ) == zero) printf("0");
		else printf("1");
	}
	printf("\n");
}

// Algoritmo de procura Shift-And
int findMotifPosition(char *P, int m, int startpos, int endpos){
	FILE *fileout;
	double *xlist, *ylist;
	int nk, k, interval;
	int size;
	int start, end;
	
	int i, n, foundpos;
	unsigned int zero;
	unsigned int one;
	unsigned int SA, SC, SG, ST;
	unsigned int mask, lastposmask;
	unsigned int R;
	char *T;
	char c;

	start=startpos;
	end=endpos;
	if(start<1) start=1;
	if(end>sequencesize) end=sequencesize;
	if((sequencesize-start)<8) start=(sequencesize-8+1);
	size=(end-start)+1;
	if(size<8) size=8;

	nk=(int)ceil(log(size)/log(2.0)+1.0);
	//nk=8;
	//interval=sequencesize/nk;
	interval=size/nk;
	fileout=fopen("positions.txt","w");
	xlist=(double *)calloc(nk,sizeof(double));
	ylist=(double *)calloc(nk,sizeof(double));
	//for(i=0;i<nk;i++) xlist[i]=(i+1)*interval;
	//xlist[nk-1]=sequencesize;
	for(i=0;i<nk;i++) xlist[i]=start+(i+1)*interval;
	xlist[nk-1]=end;

	zero=(unsigned int)0;
	one=(unsigned int)1;
	SA=zero;
	SC=zero;
	SG=zero;
	ST=zero;
	for(i=(m-1);i>=0;i--){
		SA = SA << 1;
		SC = SC << 1;
		SG = SG << 1;
		ST = ST << 1;
		c=P[i];
		if(c=='A' || c=='a') {SA = SA | one;}
		else if(c=='C' || c=='c') {SC = SC | one;}
		else if(c=='G' || c=='g') {SG = SG | one;}
		else if(c=='T' || c=='t') {ST = ST | one;}
	}
	lastposmask = one << (m-1);
	T=sequencetext;
	//n=sequencesize;
	n=size;
	R=zero;
	foundpos=0;
	for(i=0;i<n;i++){
		//c=T[i];
		c=T[start-1+i];
		if(c=='A' || c=='a') mask=SA;
		else if(c=='C' || c=='c') mask=SC;
		else if(c=='G' || c=='g') mask=SG;
		else if(c=='T' || c=='t') mask=ST;
		R = ( ( R << 1 ) | one ) & mask;
		if(( R & lastposmask) != zero){
			//foundpos=(i+1);
			//k=foundpos/interval;
			foundpos=start+i;
			k=(foundpos-start)/interval;
			//if(k>=nk) {printf("foundpos=%d start=%d interval=%d k=%d nk=%d ylist[k]=%f\n",foundpos,start,interval,k,nk,ylist[k]);fflush(stdout);}
			if(k==nk) k--;
			ylist[k]++;
			fprintf(fileout,"%d\n",foundpos);
			//fflush(fileout);
		}
	}
	fclose(fileout);
	initializeGraphics(440,280);
	drawHistogram(xlist,ylist,nk,start);
	saveBitmap("distplot.bmp");
	free(xlist);
	free(ylist);
	return foundpos;
}


// FALTA: implementar QuickSort
// ordena as listas de pontos por p-value
void sortPointsLists();

// salva a lista de pontos num ficheiro de texto
void savePointsToFile(int sort){
	int i,j;
	int *done;
	int k;
	double maxvalue;
	FILE *fileout;
	if((fileout=fopen("values.txt","w"))==NULL)
		return;
	/*
	for(i=0;i<numberofpoints;i++)
		fprintf(fileout,"%d %s %d %f %f %f\n",d2i(xpointslist[i]),substringslist[i],countslist[i],ypointslist[i],pvalueslist[i],zscoreslist[i]);
	*/
	if(sort>0){
		done=(int *)calloc(numberofpoints,sizeof(int));
		for(i=0;i<numberofpoints;i++){
			k=0;
			maxvalue=(double)(-sequencesize);
			for(j=0;j<numberofpoints;j++){
				if(done[j]==1) continue;
				if(ypointslist[j]>=maxvalue){
					maxvalue=ypointslist[j];
					k=j;
				}
			}
			done[k]=1;
			fprintf(fileout,"%d %s %d %f %.10f %f \n",d2i(xpointslist[k]),substringslist[k],countslist[k],ypointslist[k],pvalueslist[k],zscoreslist[k]);
		}
		free(done);
	}
	else{
		for(k=(numberofpoints-1);k>=2;k--){
			fprintf(fileout,"%d %s %d %f %.10f %f \n",d2i(xpointslist[k]),substringslist[k],countslist[k],ypointslist[k],pvalueslist[k],zscoreslist[k]);
		}
	}
	fclose(fileout);
}

// devolve a substring de tamanho L que termina na posi�o i do texto
char *getSubstring(int i, int l){
	char *substring;
	int length;
	int j=(i-l+1);
	length=l;
	substring=(char *)calloc((length+1),sizeof(char));
	if(i<1 || j<1 || length<1 || i>sequencesize) return substring;
	strncpy(substring,sequencetext+(j-1),length);
	substring[length]='\0';
	return substring;
}

// devolve o nmero de ocorr�cias da substring (Si...Sj) na sequ�cia total
int slowcount(int i, int j){
	char *substring;
	int length,count;
	int m,k;
	length=(j-i)+1;
	//printf("S[%d,%d]=",i,j);
	if(i<1 || j<1 || length<1 || j>sequencesize){
		//printf("\n");
		return 0;
	}
	substring=(char *)malloc((length+1)*sizeof(char));
	strncpy(substring,sequencetext+(i-1),length);
	substring[length]='\0';
	count=0;
	for(m=0;m<sequencesize;m++){
		for(k=0;k<length && (m+k)<sequencesize;k++){
			if(substring[k]!=sequencetext[(m+k)]) break;
			if(k==(length-1)) count++;
		}
	}
	//printf("'%s'(%d)\n",substring,count);
	return count;
}

// FALTA: optimizar "substring" e "getSubStringCount" com "substringscount"
// devolve o nmero de ocorr�cias da substring (Si...Sj) na sequ�cia total usando �vores de sufixos
int count(int i, int j){
	//char *substring;
	int length,count;
	length=(j-i)+1;
	//printf("S[%d,%d]=",i,j);
	if(i<1 || j<1 || length<1 || j>sequencesize){
		//printf("\n");
		return 0;
	}
	//substring=(char *)malloc((length+1)*sizeof(char));
	//strncpy(substring,sequencetext+(i-1),length);
	//substring[length]='\0';
	//count=getSubStringCount(suffixtree,substring);
	//printf("'%s'(%d)\n",substring,count);
	count=getCountByFollowInTree(sequencetext+(i-1),length);
	return count;
}

// f�mula dos perfis entr�icos
double f(double L, double phi, int i, int N){
	long double aux1,aux2;
	long double k;
	aux1 = 0.0;
	for( k = 1.0 ; k <= ( (long double) L ) ; k++ ){
		aux1 += pow( 4.0 , k ) * pow( (long double) phi , k ) * ( (long double) count( i - d2i(k) + 1 , i ) );
	}
	aux1 = 1.0 + ( 1.0 / ( (long double) N ) ) * aux1;
	aux2 = 0.0;
	for( k = 0.0 ; k <= ( (long double) L ) ; k++ ){
		aux2 += pow( (long double) phi , k );
	}
	return (double)( aux1 / aux2 );
}


// f�mula dos perfis entr�icos normalizada
double g(int L, int phi, int i, int N){
	return ( ( f((double)L,(double)phi,i,N) - averageslist[L] ) / varianceslist[L] );
}

// FALTA: ver se �preciso "long double"
// FALTA: melhorar o "getCountByFollowInTree"
double getSumFSquared(){
	int i,j;
	int pos;
	int iN=sequencesize;
	int iL=lvalue;
	int iPhi=phivalue;
	long double *fourphikvector;
	long double totalnumerator,numerator,denominator;
	int *countsvector;
	
	//int *totalcounts;
	//totalcounts=(int *)calloc((iL+1),sizeof(int));
	
	fourphikvector=(long double *)malloc((iL+1)*sizeof(long double));
	fourphikvector[0]=1;
	for(i=1;i<=iL;i++) fourphikvector[i] = (fourphikvector[i-1]) * ((long double)(4 * iPhi));
	//printf("4phikvector=%f\n",fourphikvector[iL]);
	
	countsvector=(int *)calloc((iL+1),sizeof(int));
	totalnumerator = 0.0;
	//for( i=0; i<(iN-iL+1); i++ ){
	for( i=(iL-1); i<iN; i++ ){
		numerator = 0.0;
		for(j=1;j<=iL;j++){
			//countsvector[j]=getCountByFollowInTree((char *)(sequencetext+i),j);
			pos=i-(j-1);
			countsvector[j]=getCountByFollowInTree((char *)(sequencetext+pos),j); // MUDAR PARA MAIS R�IDO!
			numerator += (fourphikvector[j]) * ((long double)(countsvector[j]));
			//if(i<3 || i==(iN-1)) printf("countsvector[%d,%d]=%d\n",i,j,countsvector[j]);
			//totalcounts[j]+=countsvector[j];
		}
		numerator = ((long double)(1.0)) + ( numerator / ((long double)iN) );
		numerator = numerator * numerator;
		totalnumerator += numerator;
	}
	
	//printf("totalcounts=%d\n",totalcounts[iL]);
	//printf("SumFSquared=%f\n",totalnumerator);
	denominator = ( pow( ((long double)iPhi) , ((long double)(iL + 1.0)) ) - ((long double)(1.0)) ) / ( (long double)(iPhi - 1.0) );
	//printf("denominator=%f\n",denominator);
	denominator = denominator * denominator;

	return ( totalnumerator / denominator );
}


// FALTA: meter a funcionar r�ido
// calcula os valores de M e de S
void slowCalculateMS(){
	int i,n;
	double fvalue;
	//double *fvalueslist;
	n=sequencesize;
	//fvalueslist=(double *)malloc(n*sizeof(double));
	mteta=0.0;
	for(i=0;i<n;i++){
		fvalue=f((double)lvalue,(double)phivalue,(i+1),n);
		//fvalueslist[i]=fvalue;
		mteta+=fvalue;
		//if(i==(i-lvalue)) break;
	}
	//printf("FSum=%f\n",mteta);
	mteta=mteta/((double)n);
	steta=0.0;
	for(i=0;i<n;i++){
		//fvalue=fvalueslist[i];
		fvalue=f((double)lvalue,(double)phivalue,(i+1),n);
		steta+=(fvalue-mteta)*(fvalue-mteta);
		//if(i==(i-lvalue)) break;
	}
	steta=steta/((double)(n-1));
	steta=sqrt(steta);
}


// FALTA: fazer free's
void fastCalculateMS(){
	int i;
	int iN=sequencesize;
	int iL=lvalue;
	int iPhi=phivalue;
	double N=(double)sequencesize;
	double L=(double)lvalue;
	double Phi=(double)phivalue;
	//double mnumerator,mdenominator;
	//int *fourphikvector;
	//int *squaredcountsvector;
	long double lN=(long double)N;
	long double lPhi=(long double)Phi;
	long double mnumerator,mdenominator;
	long double *fourphikvector;
	long double *squaredcountsvector;
	int nodecount;
	treenode *currentnode;
	treenode **sidelinksvector;
	//squaredcountsvector=(int *)calloc((iL+1),sizeof(int));
	squaredcountsvector=(long double *)calloc((iL+1),sizeof(long double));
	sidelinksvector=getSideLinksVector();
	for(i=1;i<=iL;i++){
		currentnode=sidelinksvector[i];
		while(currentnode!=NULL){
			nodecount=(currentnode->count);
			squaredcountsvector[i]+=(long double)(nodecount*nodecount);
			currentnode=currentnode->sidelink;
		}
		//printf("squaredcountsvector[%Lf]=%d\n",i,squaredcountsvector[i]);
	}
	fourphikvector=(long double *)malloc((iL+1)*sizeof(long double));
	fourphikvector[0]=1.0;
	for(i=1;i<=iL;i++){
		fourphikvector[i] = (fourphikvector[i-1]) * ( (long double)(4 * iPhi) );
		//printf("fourphikvector[%d]=%Lf\n",i,fourphikvector[i]);
	}
	//mnumerator=0.0;
	//for(i=1;i<=iL;i++) mnumerator+=(double)((fourphikvector[i])*(squaredcountsvector[i]));
	//mnumerator = N + ( mnumerator / N );
	//mdenominator = ( pow( Phi , L + 1.0 ) - 1.0 ) / ( Phi - 1.0 );
	//mteta = ( mnumerator / ( N * mdenominator ) );
	mnumerator=0.0;
	for(i=1;i<=iL;i++){
		mnumerator+=(fourphikvector[i])*(squaredcountsvector[i]);
		//printf("(4phik*c^2)[%d]=%Lf\n",i,mnumerator);
	}
	mnumerator = lN + ( mnumerator / lN );
	//printf("mnumerator[10]=%Lf\n",mnumerator);
	mdenominator = (long double)( pow( lPhi , (long double)(L + 1.0) ) - (long double)(1.0) ) / ( (long double)(Phi - 1.0) );
	//printf("mdenominator[10]=%Lf\n",mdenominator);
	//printf("FSum=%f\n",(double)(mnumerator/mdenominator));
	mteta = (double)( mnumerator / ( lN * mdenominator ) );
	steta = 0.0;
	steta = getSumFSquared();
	//printf("SumFSquared=%f\n",steta);
	//steta = steta - ( 2.0 * mteta * (double)(mnumeratormdenominator) ) + ( N * mteta * mteta );
	steta = steta - ( mteta * mteta * N );
	steta = steta / ( N - 1.0 );
	steta = sqrt( steta );
	if(mteta<0 || steta<0) exitError("Possible overflow on normalization of f.");
}

void progressiveCalculateMS(){
	int i, j, iL;
	long double zero, one, two, four;
	long double L, Phi, N;
	long double tempsum;
	long double middlesum;
	long double nodecount;
	long double depthcounts;
	long double *averageauxvector;
	long double *varianceauxvector;
	long double *fourphiknvector;
	long double *squaredcountsvector;
	long double *cubiccountsvector;
	long double *denominatorsvector;
	int *backcountsvector;
	treenode *node;
	treenode **sidelinksvector;
	iL=(int)10;
	zero=(long double)0;
	one=(long double)1;
	two=(long double)2;
	four=(long double)4;
	L=(long double)10;
	Phi=(long double)10;
	N=(long double)sequencesize;
	averageslist=(double *)calloc((iL+1),sizeof(double));
	varianceslist=(double *)calloc((iL+1),sizeof(double));
	averageauxvector=(long double *)calloc((iL+1),sizeof(long double));
	varianceauxvector=(long double *)calloc((iL+1),sizeof(long double));
	fourphiknvector=(long double *)calloc((iL+1),sizeof(long double));
	squaredcountsvector=(long double *)calloc((iL+1),sizeof(long double));
	cubiccountsvector=(long double *)calloc((iL+1),sizeof(long double));
	denominatorsvector=(long double *)calloc((iL+1),sizeof(long double));
	backcountsvector=(int *)calloc((iL+1),sizeof(int));
	fourphiknvector[0]=one/N;
	sidelinksvector=getSideLinksVector();
	averageauxvector[0]=N;
	varianceauxvector[0]=N;
	for(i=1;i<=iL;i++){
		//fourphiknvector[i] = fourphiknvector[i-1] * ( four * Phi );
		fourphiknvector[i] = pow( four * Phi, i ) / N;
		//if(i==lvalue) printf("4phikvector[%d]=%Lf\n",i,fourphiknvector[i]*N);
		denominatorsvector[i] = ( pow( Phi, (long double)(i+1) ) - one ) / ( Phi - one );
		//if(i==lvalue) printf("denominator[%d]=%Lf\n",lvalue,denominatorsvector[i]);
		middlesum = zero;
		depthcounts = zero;
		node = sidelinksvector[i];
		squaredcountsvector[i]=zero;
		cubiccountsvector[i]=zero;
		while(node!=NULL){
			nodecount = (long double)(node->count);
			depthcounts += nodecount;
			squaredcountsvector[i] += ( nodecount * nodecount );
			cubiccountsvector[i] += ( nodecount * nodecount * nodecount );
			tempsum = one;
			getBackCountsVector(node,backcountsvector);
			for(j=1;j<=(i-1);j++)
				tempsum += fourphiknvector[j] * backcountsvector[j];
			middlesum += tempsum * ( fourphiknvector[i] * nodecount ) * nodecount;
			node = (node->sidelink);
		}
		averageauxvector[i] = averageauxvector[i-1] + ( fourphiknvector[i] * squaredcountsvector[i] );
		//printf("c^2[%d]=%Lf\n",i,(long double)(depthcounts * depthcounts));
		//printf("(4phik*c^2)[%d]=%Lf\n",i,averageauxvector[i]*N-N);
		varianceauxvector[i] = varianceauxvector[i-1] + ( two * middlesum );
		varianceauxvector[i] += ( fourphiknvector[i] * fourphiknvector[i] ) * ( cubiccountsvector[i] );
		//if(i==lvalue) printf("totalcounts[%d]=%Lf\n",lvalue,depthcounts);
		//if(i==lvalue) printf("SumFSquared[%d]=%f\n",lvalue,varianceauxvector[i]);

		averageslist[i] = (double) ( averageauxvector[i] / ( N * denominatorsvector[i] ) );
		varianceslist[i] = (double) ( varianceauxvector[i] / ( denominatorsvector[i] * denominatorsvector[i] ) );
		//if(i==lvalue) printf("SumFSquared[%d]=%f\n",lvalue,varianceslist[i]);
		varianceslist[i] += - ( ( averageslist[i] * averageslist[i] ) * ((double)N) );
		varianceslist[i] = sqrt( varianceslist[i] / ((double)( N - one )) );
	}
	//printf("averageauxvector=%Lf\n",averageauxvector[iL]-N);
	mteta=averageslist[lvalue];
	steta=varianceslist[lvalue];
	free(backcountsvector);
	free(denominatorsvector);
	free(fourphiknvector);
	free(varianceauxvector);
	free(averageauxvector);
	free(squaredcountsvector);
	free(cubiccountsvector);
}

// constr� as listas de valores dos perfis entr�icos
void initializePointsLists(){
	int i,startpoint,endpoint;
	numberofpoints=positionwindow;
	if((numberofpoints%2)==0) numberofpoints++; // n par
	startpoint=position-(numberofpoints-1)/2;
	startpoint=intmax(1,startpoint);
	endpoint=position+(numberofpoints-1)/2;
	endpoint=intmin(sequencesize,endpoint);
	numberofpoints=(endpoint-startpoint)+1;
	xpointslist=(double *)malloc(numberofpoints*sizeof(double));
	ypointslist=(double *)malloc(numberofpoints*sizeof(double));
	pvalueslist=(double *)malloc(numberofpoints*sizeof(double));
	zscoreslist=(double *)malloc(numberofpoints*sizeof(double));
	countslist=(int *)malloc(numberofpoints*sizeof(int));
	substringslist=(char **)malloc(numberofpoints*sizeof(char *));
	//initializeStatisticsForText(sequencetext,sequencesize);
	initializeStatisticsFromTree();
	for(i=0;i<numberofpoints;i++){
		//printf("x=%d\n",(startpoint+i));
		xpointslist[i]=(double)(startpoint+i);
		//ypointslist[i]=f((double)lvalue,(double)phivalue,(startpoint+i),sequencesize);
		ypointslist[i]=g(lvalue,phivalue,(startpoint+i),sequencesize);
		//ypointslist[i]=fvalueslist[(startpoint+i)];
		substringslist[i]=getSubstring(startpoint+i,lvalue);
		countslist[i]=getSubStringCount(suffixtree,substringslist[i]);
		pvalueslist[i]=getPValueForWord(substringslist[i],lvalue,countslist[i]);
		zscoreslist[i]=getZScoreForWord(substringslist[i],lvalue,countslist[i]);
	}
	freeStatistics();
	//printPoints();
}

// constr� as listas de valores dos perfis entr�icos para o estudo de um motivo
void initializePointsListsForMotifStudy(){
	int i;
	numberofpoints=lvalue;
	xpointslist=(double *)malloc(numberofpoints*sizeof(double));
	ypointslist=(double *)malloc(numberofpoints*sizeof(double));
	pvalueslist=(double *)malloc(numberofpoints*sizeof(double));
	zscoreslist=(double *)malloc(numberofpoints*sizeof(double));
	countslist=(int *)malloc(numberofpoints*sizeof(int));
	substringslist=(char **)malloc(numberofpoints*sizeof(char *));
	initializeStatisticsFromTree();
	for(i=0;i<numberofpoints;i++){
		xpointslist[i]=(double)(position);
		ypointslist[i]=g((i+1),phivalue,position,sequencesize);
		substringslist[i]=getSubstring(position,(i+1));
		countslist[i]=getSubStringCount(suffixtree,substringslist[i]);
		pvalueslist[i]=getPValueForWord(substringslist[i],(i+1),countslist[i]);
		zscoreslist[i]=getZScoreForWord(substringslist[i],(i+1),countslist[i]);
	}
	freeStatistics();
}

// encontra o valor de L que maximiza a fun�o F para a posi�o dada
void findMaxValues(){
	int lval,foundl,lmax;
	double fval,maxf;
	foundl=0;
	maxf=(double)(-sequencesize);
	lmax=LIMIT;
	for(lval=1;lval<=lmax;lval++){
		fval=g(lval,phivalue,position,sequencesize);
		//printf("L=%d Phi=%d f=%.3f ",lval,phival,fval);
		if(fval>maxf){
			maxf=fval;
			foundl=lval;
			//printf("*");
		}
		//printf("\n");
	}
	lmaxvalue=foundl;
	lvalue=lmaxvalue;
	//printf("f(L_max=%d,Phi_max=%d)=%f\n",lvalue,phivalue,maxf);
}


/***************/
/* BUILD PLOTS */
/***************/

void createEPPlot(){
	initializeGraphics(440,280);
	drawPlot(xpointslist,ypointslist,numberofpoints,1,"x","ep",position);
	drawLabel(lvalue,phivalue,0,"");
	saveBitmap("plot.bmp");
}

// cria o gr�ico dos valores de (f) para (phi) e (i) fixos e (L) vari�el
void createLPlot(){
	int i;
	double *xlist,*ylist;
	xlist=(double *)malloc(LIMIT*sizeof(double));
	ylist=(double *)malloc(LIMIT*sizeof(double));
	for(i=1;i<=LIMIT;i++){
		xlist[i-1]=(double)i;
		ylist[i-1]=g(i,phivalue,position,sequencesize);
	}
	initializeGraphics(320,240);
	drawPlot(xlist,ylist,LIMIT,1,"L","ep",0);
	drawLabel(0,phivalue,position,"");
	saveBitmap("lplot.bmp");
	free(xlist);
	free(ylist);
}

void createLMaxAnd3DPlot(){
	int i,j,startpoint,endpoint,npoints,ny;
	double **pointslist,*lmaxslist,*poslist,gvalue;
	int lforgmax, maxl, maxi;
	double gmax, maxep;
	ny=LIMIT;
	npoints=positionwindow;
	if((npoints%2)==0) npoints++;
	startpoint=position-(npoints-1)/2;
	startpoint=intmax(1,startpoint);
	endpoint=position+(npoints-1)/2;
	endpoint=intmin(sequencesize,endpoint);
	npoints=(endpoint-startpoint)+1;
	pointslist=(double **)malloc(npoints*sizeof(double *));
	poslist=(double *)malloc(npoints*sizeof(double));
	lmaxslist=(double *)malloc(npoints*sizeof(double));
	maxl=0;
	maxi=0;
	maxep=(double)(-sequencesize);
	for(i=0;i<npoints;i++){
		pointslist[i]=(double *)malloc(ny*sizeof(double));
		gmax=(double)(-sequencesize);
		for(j=0;j<ny;j++){
			//gvalue=g((j+1),phivalue,(startpoint+i),sequencesize); // x normal, y normal
			//gvalue=g((ny-j),phivalue,(endpoint-i),sequencesize); // x trocado, y trocado
			gvalue=g((j+1),phivalue,(endpoint-i),sequencesize); // x trocado, y normal
			pointslist[i][j]=gvalue;
			if(gvalue>=gmax) {gmax=gvalue;lforgmax=(j+1);}
		}
		//poslist[i]=(startpoint+i);
		poslist[i]=(endpoint-i);
		lmaxslist[i]=lforgmax;
		if(gmax>maxep) { maxep=gmax; maxl=lforgmax; maxi=(endpoint-i); }
	}
	//initialize3DGraphics(440,280);
	initialize3DGraphics(512,320);
	drawPlot3D(pointslist,npoints,ny,startpoint);
	draw3DLabel(maxi,maxl);
	saveBitmap("3dplot.bmp");
	initializeGraphics(320,240);
	drawPlot(poslist,lmaxslist,npoints,1,"x","L",position);
	drawLabel(0,phivalue,0,"");
	saveBitmap("lmaxplot.bmp");
	for(i=0;i<npoints;i++) free(pointslist[i]);
	free(pointslist);
	free(poslist);
	free(lmaxslist);
}

// cria o gr�ico dos valores de -log(p-value) para cada posi�o
void createLogPValuePlot(){
	int i;
	double *logpvalueslist,max;
	if(pvalueslist==NULL) return;
	logpvalueslist=(double *)malloc(numberofpoints*sizeof(double));
	max=-1.0;
	for(i=0;i<numberofpoints;i++){
		if(pvalueslist[i]==0) logpvalueslist[i]=-1.0;
		else {
			logpvalueslist[i]=-log(pvalueslist[i]);
			if(logpvalueslist[i]>max) max=logpvalueslist[i];
		}
	}
	max=1.25*max;
	for(i=0;i<numberofpoints;i++){
		//printf("( %d , %f ,%f )\n",i,pvalueslist[i],logpvalueslist[i]);
		if(logpvalueslist[i]==-1.0) logpvalueslist[i]=max;
	}
	initializeGraphics(320,240);
	drawPlot(xpointslist,logpvalueslist,numberofpoints,1,"x","p-v",0);
	//drawLabel(0,phivalue,position,"L");
	saveBitmap("pvplot.bmp");
	//freeBitmap();
	free(logpvalueslist);
}

// cria o gr�ico dos valores do z-score para cada posi�o
void createZScorePlot(){
	if(zscoreslist==NULL) return;
	initializeGraphics(320,240);
	drawPlot(xpointslist,zscoreslist,numberofpoints,1,"","",0);
	//drawLabel(0,phivalue,position,"L");
	saveBitmap("zsplot.bmp");
	//freeBitmap();
}

// cria o gr�ico de correla�o entre o p-value e f
void createCorrelationPlot(){
	//int i;
	if(ypointslist==NULL || pvalueslist==NULL) return;
	initializeGraphics(320,240);
	//for(i=0;i<numberofpoints;i++) printf("points[%4d] = ( %f , %f )\n",i,ypointslist[i],pvalueslist[i]);
	drawPlot(ypointslist,pvalueslist,numberofpoints,0,"ep","p-v",0);
	//drawLabel(0,phivalue,position,"L");
	saveBitmap("corrplot.bmp");
	//freeBitmap();
}


/*********/
/* INPUT */
/*********/


// devolve o argumento com nome key do tipo string
char * parseStringArg(char **argslist, int argssize, char key){
	int i;
	for(i=0;i<argssize;i++)
		if(argslist[i][1]==key || ((char)((int)(argslist[i][1])+32))==key)
			if((int)strlen(argslist[i])>2) return (char *)(argslist[i]+2);
	return NULL;
}

// devolve o argumento com nome key do tipo char
char parseCharArg(char **argslist, int argssize, char key){
	int i;
	for(i=0;i<argssize;i++)
		if(argslist[i][1]==key || ((char)((int)(argslist[i][1])+32))==key)
			if((int)strlen(argslist[i])>2) return (char)(argslist[i][2]);
	return '\0';
}

// devolve o argumento com nome key do tipo int
int parseIntArg(char **argslist, int argssize, char key){
	int i;
	for(i=0;i<argssize;i++)
		if(argslist[i][1]==key || ((char)((int)(argslist[i][1])+32))==key)
			if((int)strlen(argslist[i])>2) return atoi(argslist[i]+2);
	return 0;
}

// FALTA: argumento gr�icos a desenhar
// FALTA: fazer free das vari�eis
// FALTA: fazer a valida�o e substitui�o de todas as letras da sequ�cia
// FALTA: command line arguments do tipo "ep -x1 -y2 -z3 ..."
// FALTA: fazer verifica�es dos argumentos
// FALTA: garantir que n� h�espa�s nem outros caracteres estranhos no texto enviado
// FALTA: garantir que as vari�eis nmericas t� valores num�icos
// FALTA: retirar espa�s da descri�o do ficheiro
// FALTA: verificar limites da substring devolvida

// INPUT: entropicprofiles 1<tipo da sequ�cia> 2<texto ou nome do ficheiro> 3<L m�> 4<phi m�> 5<i> 6<find m�?> 7<pos window>
// OUTPUT: 1<L> 2<Phi> 3<i> 4<tamanho da sequ�cia> 5<limite> 6<nmero de passos> 7<nmero de n�> 8<nmero de bytes>
// OUTPUT: 9<subseq. de comprimento L na posi�o i> 10<texto ou descri�o da seq. no ficheiro>
int main(int argc, char *argv[]){

	int n,steps,limit,nbytes,nnodes,findmax;
	char *subsequence, *description;
	char *treefilename;
	char *motif;
	int loadedfromfile;

	if(argc<8)
		exitError("Not enough arguments.\nUsage: ep -t<type> -f<file> -l<l> -p<phi> -i<position> -m<findmax> -w<window>");
	type=parseCharArg(argv,argc,'t');
	if(type=='\0') exitError("Invalid sequence type.");
	/*
	if(type=='T' || type=='t'){
		sequencetext=parseStringArg(argv,argc,'f');
		sequencesize=(int)strlen(sequencetext);
		filename=(char *)malloc(sizeof(char));
		strcpy(filename,"");
		sequencefromfile=NULL;
		description=sequencetext;
	}
	*/
	// if(type=='F' || type=='f' || type=='E' || type=='e'){
	filename=parseStringArg(argv,argc,'f');
	if(filename==NULL) exitError("Invalid sequence file name.");
	//sequencefromfile=(sequence *)malloc(sizeof(sequence));
	sequencefromfile=loadSequence(filename);
	if(sequencefromfile==NULL) exitError("Invalid sequence file name.");
	sequencetext=sequencefromfile->data;
	sequencesize=sequencefromfile->size;
	description=sequencefromfile->description;

	lvalue=parseIntArg(argv,argc,'l');
	if(sequencesize<lvalue) exitError("Sequence size too small.");
	//phivalue=parseIntArg(argv,argc,'p');
	phivalue=10;
	position=parseIntArg(argv,argc,'i');
	findmax=parseIntArg(argv,argc,'m');
	positionwindow=parseIntArg(argv,argc,'w');
	//if(lvalue==0 || phivalue==0 || position==0 || positionwindow==0) exitError("Invalid or missing arguments.");

	if(lvalue<1 || lvalue>10) lvalue=10;
	if(phivalue<1 || phivalue>10) phivalue=10;
	if(position<1) position=1;
	if(position>sequencesize) position=sequencesize;
	//if(positionwindow<1) positionwindow=1;
	//if(positionwindow>sequencesize) positionwindow=sequencesize;
	//if( ((position-positionwindow/2)<1) || ((position+positionwindow/2)>sequencesize) ) positionwindow=2*intmin(position,sequencesize-position);

	n=(int)strlen(filename);
	treefilename=(char *)malloc((n+7)*sizeof(char));
	strcpy(treefilename,filename);
	strcat(treefilename,".tree");

	steps=0;
	limit=LIMIT;
	loadedfromfile=parseIntArg(argv,argc,'x');
	if(loadedfromfile==0){
		suffixtree=buildTree(sequencetext,limit,&steps);
		saveTreeToFile(treefilename);
		saveDescriptionToFile();
		progressiveCalculateMS();
		saveDataToFile();
	} else {
		//printf("Loading: '%s'\n",treefilename);
		if(loadTreeFromFile(treefilename)==0) exitError("Tree file not found.");
		suffixtree=getSuffixTreeRoot();
		setTreeText(sequencetext);
		loadDataFromFile();
	}
	nnodes=getNumberOfTreeNodes(suffixtree);
	nbytes=getTreeSize(suffixtree);
	//printf("Tree done: %d nodes\n",nnodes);

	//slowCalculateMS();
	//fastCalculateMS();
	//printf("M=%f\nS=%f\n\n",mteta,steta);
	//printf("M[%d]=%f\nS[%d]=%f\n\n",lvalue,averageslist[lvalue],lvalue,varianceslist[lvalue]);

	motif=parseStringArg(argv,argc,'y');
	if(motif!=NULL){
		lvalue=(int)strlen(motif);
		if(lvalue>10) lvalue=10;
		if(lvalue<3) lvalue=3;
		if(findmax==0) position=findMotifPosition(motif,lvalue,position,position+positionwindow);
		else position=findMotifPosition(motif,lvalue,1,sequencesize);
		if(position==0) exitError("Motif not found on sequence.");
		initializePointsListsForMotifStudy();
		savePointsToFile(0);
		createLPlot();
	} else{
		if(findmax!=0) findMaxValues();
		initializePointsLists();
		savePointsToFile(1);
		createEPPlot();
		createLPlot();
		createLMaxAnd3DPlot();
		createCorrelationPlot();
		buildCGRMap(4);
	}

	subsequence=(char *)malloc((lvalue+1)*sizeof(char));
	n=intmax(0,(position-lvalue));
	strncpy(subsequence,sequencetext+n,lvalue);
	subsequence[lvalue]='\0';

	printf("%d %d %d %d %d %d %d %d %s %s\n",lvalue,phivalue,position,sequencesize,limit,steps,nnodes,nbytes,subsequence,description);
	
	if(sequencefromfile!=NULL) freeSequence(sequencefromfile);
	freeTreeNode(suffixtree);
	freeVariables();

	free(treefilename);
	free(subsequence);
	freeAlphabet();

	//system("pause");
	return 1;
}

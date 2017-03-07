#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "suffixtries.h"

#define ALPHABETSIZE 4
#define LIMIT 10
#define BUFFERLENGTH 8

/****************/
/* SUFFIX TREES */
/****************/

static treenode *root; // n� ra�z da �rvore
static char *text; // texto que vai dar origem � �rvore
static int textsize; // tamanho do texto
static int numberofnodes; // n�mero total de n�s da �rvore
static treenode **sidelinksvector; // lista de n�s que est�o na mesma profundidade
static int loadedfromfile; // se a �rvore foi carregada a partir de um ficheiro ou n�o
static int maxdepth = LIMIT; // m�xima profundidade da �rvore

static unsigned char fileiobuffer;
static int fileiobuffersize;
static unsigned char bitmask;
const unsigned char masklast = (unsigned char)1;
const unsigned char maskfirst = (unsigned char)128;
const unsigned char zero = (unsigned char)0;
const unsigned char one = (unsigned char)1;


// devolve um apontador para a raiz da �rvore de sufixos
treenode * getSuffixTreeRoot(){
	return root;
}


// devolve um apontador para o vector de sidelinks
treenode ** getSideLinksVector(){
	return sidelinksvector;
}


// inicializa o vector de sidelinks
void initializeSideLinksVector(int limit){
	int i,n;
	n=(limit+1);
	sidelinksvector=(treenode **)malloc(n*sizeof(treenode *));
	for(i=0;i<n;i++) sidelinksvector[i]=NULL;
}


// devolve o caracter da base correspondente ao c�digo num�rico dado
int getCharCode(char c){
	if(c=='A') return 0;
	if(c=='C') return 1;
	if(c=='G') return 2;
	if(c=='T') return 3;
	if(c=='a') return 0;
	if(c=='c') return 1;
	if(c=='g') return 2;
	if(c=='t') return 3;
	return -1;
}


// devolve o c�digo num�rico correspondente ao caracter da base dado
char getCodeChar(unsigned char c){
	if(((int)c)==3) return 'T';
	if(((int)c)==2) return 'G';
	if(((int)c)==1) return 'C';
	//if(((int)c)==0) return 'A';
	return 'A';
}


// cria um novo n�
treenode * newNode(char label,int depth){
	treenode *newnode=(treenode *)malloc(sizeof(treenode));
	newnode->label=label;
	newnode->count=1;
	newnode->depth=depth;
	newnode->suffixlink=NULL;
	//newnode->branches=NULL;
	newnode->branches=(treenode **)calloc((ALPHABETSIZE),sizeof(treenode *));
	newnode->numbranches=0;
	newnode->sidelink=sidelinksvector[depth];
	sidelinksvector[depth]=newnode;
	//newnode->backlink=NULL;
	numberofnodes++;
	return newnode;
}


// adiciona um novo ramo a um n� j� existente
treenode * addBranch(treenode *node, char label){
	int pos=getCharCode(label);
	treenode *newbranch;
	//if((node->numbranches)>=ALPHABETSIZE) return NULL;
	if(pos==-1) return NULL;
	//if((node->branches[pos])!=NULL) return NULL;
	newbranch=newNode(label,(node->depth)+1);
	//if(node->branches==NULL)
	//	node->branches=(treenode **)malloc((ALPHABETSIZE)*sizeof(treenode *));
	//node->branches[node->numbranches]=newbranch;
	//newbranch->backlink=node;
	node->branches[pos]=newbranch;
	node->numbranches++;
	return newbranch;
}


// devolve o ramo que tem o r�tulo c
treenode * getBranchOfChar(treenode *node, char c){
	//int i,n;
	//treenode *branch;
	//n=(node->numbranches);
	//for(i=0;i<n;i++){
	//	branch=node->branches[i];
	//	if((branch->label)==c)
	//		return branch;
	//}
	//return NULL;
	int pos=getCharCode(c);
	if(pos==-1) return NULL;
	return (node->branches[pos]);
}


// devolve o (n+1)-�simo ramo
treenode * getNthBranch(treenode *node, int n){
	int i,k;
	treenode *branch;
	k=-1;
	for(i=0;i<ALPHABETSIZE;i++){
		branch=node->branches[i];
		if(branch!=NULL) k++;
		if(k==n) return branch;
	}
	return NULL;
}


// actualiza os contadores dos sufixos desde o n� actual (inclusiv�) at� � ra�z
void updatePreviousCounts(treenode *node){
	treenode *current;
	current=node;
	while(current!=root){
		current->count++;
		//branch=getBranchOfChar(current,c);
		//if(branch!=NULL) branch->count++;
		current=current->suffixlink;
	}
}


// FALTA: dar logo o tamanho da sequ�ncia para n�o fazer sizeof
// FALTA: melhorar actualiza��o das contagens para tr�s (updatePreviousCounts)
// FALTA: implementar constru��o de Ukkonnen
// constr�i a �rvore dos sufixos de tamanho m�ximo igual ao definido
treenode * buildTree(char *txt, int limit, int *stepsoutput){
	treenode *nextstart,*current,*new,*previousnew,*found;
	int i,n,steps;
	char ti;
	n=(int)strlen(txt);
	//text=(char *)malloc((n+1)*sizeof(char));
	//strcpy(text,txt);
	text=txt;
	textsize=n;
	maxdepth=limit;
	steps=0;
	numberofnodes=0;
	loadedfromfile=0;
	initializeSideLinksVector(limit);
	root=newNode('#',0);
	root->count=n;
	nextstart=root;
	for(i=0;i<n;i++){
		ti=text[i];
		current=nextstart;
		nextstart=NULL;
		previousnew=NULL;
		found=NULL;
		new=NULL;
		while(1){
			steps++;
			if((current->depth)==limit) // se se chegou ao tamanho limite de profundidade
				current=current->suffixlink; // continuar pelo sufixo
			found=getBranchOfChar(current,ti); // ir buscar o ramo com o r�tulo t[i]
			if(found!=NULL){ // se j� existe o ramo com o r�tulo t[i]
				found->count++; // incrementar o contador
				new=found; // mudar o nome � vari�vel
			}
			else // se ainda n�o existe o ramo com o r�tulo t[i]
				new=addBranch(current,ti); // adicionar o ramo com esse r�tulo
			if(nextstart==NULL) // se ainda n�o se definiu o come�o da pr�xima itera��o
				nextstart=new; // definir pr�ximo n� inicial
			if(previousnew!=NULL) // se j� se tinha criado um novo n� antes deste
				previousnew->suffixlink=new; // actualizar o suffixlink desse n� para apontar para este
			if(found!=NULL){ // se o ramo j� existia
				updatePreviousCounts(found->suffixlink); // actualizar os contadores anteriores at� � ra�z
				break; // passar ao pr�ximo caracter do texto
			}
			if(current==root){ // se estamos na ra�z da �rvore
				new->suffixlink=root; // definir o suffixlink do novo n� a apontar para a ra�z
				break; // passar ao pr�ximo caracter
			}
			previousnew=new; // actualizar a vari�vel do �ltimo n� criado
			current=current->suffixlink; // seguir pelo suffixlink
		}
	}
	if(stepsoutput!=NULL) (*stepsoutput)=steps;
	return root;
}


// devolve o n�mero de vezes que a substring aparece na string
int getSubStringCount(treenode *node, char *substring){
	treenode *currentnode=NULL;
	int i,n=(int)strlen(substring);
	currentnode=node;
	for(i=0;i<n;i++){
		currentnode=getBranchOfChar(currentnode,substring[i]);
		if(currentnode==NULL) return 0;
	}
	return (currentnode->count);
}


// devolve o vector das contagens dos n�s desde o n� actual at� � ra�z (seguindo os suffixlinks)
void getBackCountsVector(treenode *startnode, int *countsvector){
	int depth;
	treenode *currentnode;
	depth=startnode->depth;
	currentnode=startnode;
	while(currentnode!=root){
		countsvector[depth]=currentnode->count;
		depth--;
		currentnode=currentnode->suffixlink;
	}
}


// devolve o n� no qual termina o caminho descrito pela string e profundidade dadas
treenode *followInTree(char *string, int depth){
	int i;
	treenode *currentnode;
	currentnode=root;
	for(i=0;i<depth;i++){
		currentnode=getBranchOfChar(currentnode,string[i]);
		if(currentnode==NULL) return NULL;
	}
	return currentnode;
}


// devolve a contagem (...)
int getCountByFollowInTree(char *string, int depth){
	treenode *node;
	node=followInTree(string,depth);
	if(node==NULL) return 0;
	return node->count;
}


// devolve o vector das contagens de um ramo inteiro na �rvore
treenode *getCountsVector(int pos, int *countsvector, treenode *start){
	int i;
	treenode *startnode,*currentnode;
	if(start!=NULL) startnode=start;
	else startnode=followInTree((char *)(text+pos),maxdepth);
	currentnode=startnode;
	for(i=1;i<=maxdepth;i++){
		countsvector[i]=(currentnode->count);
		currentnode=currentnode->suffixlink;
	}
	if((pos+maxdepth+1)>=textsize) return NULL;
	return getBranchOfChar(startnode->suffixlink,(char)(text[pos+maxdepth+1]));
}

// imprime a estrutura da �rvore em formato reduzido
void printSuffixTree(treenode *node,int pos){
	int i=0;
	int n=node->numbranches;
	//printf("(%d)(%d)",pos,n);
	printf("[%d]",n);
	//printf("#%d %d ",pos,node->numbranches);
	for(i=0;i<pos;i++)	printf("\t");
	//if(node!=root){
		printf("%c (%d)",node->label,node->count);
	//}
	printf("\n");
	for(i=0;i<n;i++){
		if((node->branches[i])!=NULL) printSuffixTree(node->branches[i],pos+1);
	}
}


// FALTA: lidar com n�meros >999
// converte um n�mero para uma string no formato "(123)" para ser mostrado no desenho da �rvore
char *countToString(int n){
	int nn;
	char *countstring, *tmp;
	nn=(n%1000);
	countstring=(char *)malloc(8*sizeof(char));
	tmp=(char *)malloc(4*sizeof(char));
	strcpy(countstring,"(");
	if(nn<100) strcat(countstring,"0");
	if(nn<10) strcat(countstring,"0");
	//itoa(nn,tmp,10);
	sprintf(tmp,"%d",nn);
	strcat(countstring,tmp);
	strcat(countstring,")");
	return countstring;
}


// FALTA: optimizar aloca��o de mem�ria das strings com free's e realloc's
// desenha a �rvore de sufixos em formato detalhado
void drawSuffixTree(FILE *output, treenode *node, int totalnodes, int nodenumber, char *linestart, char *countslinestart){
	int i,j,n;
	char *newlinestart, *newcountslinestart, *countstring;
	n=node->numbranches;
	if(nodenumber!=1)
		fprintf(output,"%s|\n%s",linestart,linestart);
	if(nodenumber==totalnodes && nodenumber!=1) fprintf(output,"\'--");
	else fprintf(output,"|--");
	fprintf(output,"%c",node->label);
	
	newcountslinestart=(char *)malloc(((int)strlen(countslinestart)+7+1)*sizeof(char));
	strcpy(newcountslinestart,countslinestart);
	if(nodenumber!=totalnodes) strcat(newcountslinestart,"|");
	else strcat(newcountslinestart," ");
	countstring=countToString(node->count);
	strcat(newcountslinestart,countstring);
	strcat(newcountslinestart," ");
	free(countstring);

	if(n==0){
		fprintf(output,"\n");
		fprintf(output,"%s\n",newcountslinestart);
		//free(newcountslinestart);
		//free(countslinestart);
	}
	else{
		fprintf(output,"-->");
		newlinestart=(char *)malloc(((int)strlen(linestart)+7+1)*sizeof(char));
		strcpy(newlinestart,linestart);
		if(nodenumber!=totalnodes) strcat(newlinestart,"|      ");
		else strcat(newlinestart,"       ");
		i=1;
		for(j=0;j<ALPHABETSIZE;j++){
			if((node->branches[j])!=NULL){
				if(i==1) drawSuffixTree(output,node->branches[j],n,1,newlinestart,newcountslinestart);
				else drawSuffixTree(output,node->branches[j],n,i,newlinestart,newlinestart);
				i++;
			}
		}
		//drawSuffixTree(output,node->branches[0],n,1,newlinestart,newcountslinestart);
		//for(i=2;i<=n;i++)
		//	drawSuffixTree(output,node->branches[(i-1)],n,i,newlinestart,newlinestart);
		//free(newlinestart);
		//free(linestart);
	}
}


// salva o desenho da �rvore num ficheiro
int printTreeToFile(treenode *node, char * filename){
	FILE *file;
	file=fopen(filename,"w");
	if(file==NULL) return 0;
	drawSuffixTree(file,node,1,1,"","");
	if(fclose(file)!=0) return 0;
	return 1;
}

// devolve o n�mero de bits de um n�mero
int logtwo(int x){
	int twopower = 1;
	int i = 1;
	if(x==1) return 1;
	while(twopower<=x){
		twopower = twopower << 1;
		i++;
	}
	return (i-1);
}

// escreve a informa��o de um n� para um ficheiro em modo bin�rio
void writeNodeBits(FILE *file, int numbranches, char label, int count){
	char mode;
	unsigned char data,tempdata;
	unsigned int countdata;
	int i,n,nzeros;
	mode='N';
	while(1){
		if(mode=='c'){
			countdata = countdata >> 1;
			data=(unsigned char)countdata;
			tempdata =  data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			i++;
			if(tempdata==zero) nzeros++;
			else nzeros=0;
			if(i==n) mode='F';
			if(nzeros==3) mode='Z';
		}
		else if(mode=='C'){
			n=logtwo(count);
			i=1;
			nzeros=0;
			countdata=(unsigned int)count;
			data=(unsigned char)countdata;
			tempdata =  data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='c';
			if(tempdata==zero) nzeros++;
			if(n==1) mode='F';
		}
		else if(mode=='N'){
			i=1;
			data=(unsigned char)numbranches;
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='n';
		}
		else if(mode=='n'){
			data = data >> 1;
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			i++;
			if(i==3) mode='L';
		}
		else if(mode=='L'){
			data=(unsigned char)getCharCode(label);
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='l';
		}
		else if(mode=='l'){
			data = data >> 1;
			tempdata = data & masklast;
			fileiobuffer = fileiobuffer | tempdata;
			mode='C';
		}
		else if(mode=='F'){
			i=1;
			//tempdata=(unsigned char)0;
			mode='f';
		}
		else if(mode=='f'){
			if(i==4) break;
			i++;
		}
		else if(mode=='Z'){
			fileiobuffer = fileiobuffer | one;
			nzeros=0;
			mode='c';
			if(i==n) mode='F';
		}
		fileiobuffersize++;
		if(fileiobuffersize==BUFFERLENGTH){
			//printf("%u\n",(int)fileiobuffer);
			fwrite(&fileiobuffer,1,1,file);
			fileiobuffer=(unsigned char)0;
			fileiobuffersize=0;
		}
		fileiobuffer = fileiobuffer << 1;	
	}
}


// l� a informa��o de um n� de um ficheiro em modo bin�rio
void readNodeBits(FILE *file, int *numbranches, char *label, int *count){
	char mode;
	unsigned char bit,data;
	unsigned int countdata, intbit;
	int i,nzeros;
	mode='N';
	i=1;
	while(1){
		if(fileiobuffersize==0){
			fread(&fileiobuffer,1,1,file);
			//printf("%u\n",(int)fileiobuffer);
			fileiobuffersize=8;
			bitmask=maskfirst;
		}
		bit = fileiobuffer & bitmask;
		bit = bit >> (fileiobuffersize-1);
		//printf("%d",(int)bit);
		if(mode=='c'){
			if(bit==zero) {intbit=(unsigned int)0; nzeros++;}
			else {intbit=(unsigned int)1; nzeros=0;}
			intbit = intbit << i;
			countdata = countdata | intbit;
			i++;
			if(nzeros==3) mode='Z';
		}
		else if(mode=='C'){
			i=1;
			nzeros=0;
			countdata=(unsigned int)0;
			if(bit==zero){intbit=(unsigned int)0; nzeros++;}
			else intbit=(unsigned int)1;
			countdata = countdata | intbit;
			mode='c';
		}
		else if(mode=='N'){
			i=1;
			data = zero;
			data = data | bit;
			mode='n';
		}
		else if(mode=='n'){
			bit = bit << i;
			data = data | bit;
			i++;
			if(i==3){
				(*numbranches)=(int)data;
				//printf("(%d)",(*numbranches));
				mode='L';
			}
		}
		else if(mode=='L'){
			data = zero;
			data = data | bit;
			mode='l';
		}
		else if(mode=='l'){
			bit = bit << 1;
			data = data | bit;
			(*label)=getCodeChar(data);
			//printf("(%c)",(*label));
			mode='C';
		}
		else if(mode=='F'){
			break;
		}
		else if(mode=='Z'){
			if(bit==zero){
				(*count)=(int)countdata;
				//printf("(%d)",(*count));
				mode='F';
			} else{
				nzeros=0;
				mode='c';
			}
		}
		fileiobuffersize--;
		bitmask = bitmask >> 1;
	}
	//printf("\n");
}


// guarda a �rvore para um ficheiro
void saveTreeToFile(char *filename){
	int depth,branchnumber,*totalbranches,*currentbranch;
	treenode *node,**nodeslist;
	FILE *file;
	//file=fopen(filename,"w");
	file=fopen(filename,"wb");
	if(file==NULL) return;
	fileiobuffer=(unsigned char)0;
	fileiobuffersize=0;
	nodeslist=(treenode **)malloc((maxdepth+1)*sizeof(treenode *));
	totalbranches=(int *)calloc((maxdepth+1),sizeof(int));
	currentbranch=(int *)calloc((maxdepth+1),sizeof(int));
	depth=0;
	node=root;
	nodeslist[depth]=node;
	totalbranches[depth]=node->numbranches;
	currentbranch[depth]=0;
	//fprintf(file,"%d %c %d\n",node->numbranches,node->label,node->count);
	//printf("%d %c %d\n",node->numbranches,node->label,node->count);
	writeNodeBits(file,node->numbranches,node->label,node->count);
	while(1){
		if(currentbranch[depth]<totalbranches[depth]){
			branchnumber=currentbranch[depth];
			//node=node->branches[branchnumber];
			node=getNthBranch(node,branchnumber);
			depth++;
			nodeslist[depth]=node;
			currentbranch[depth]=0;
			totalbranches[depth]=node->numbranches;
			//fprintf(file,"%d %c %d\n",node->numbranches,node->label,node->count);
			//printf("%d %c %d\n",node->numbranches,node->label,node->count);
			writeNodeBits(file,node->numbranches,node->label,node->count);
		}
		else{
			depth--;
			if(depth==-1) break;
			node=nodeslist[depth];
			currentbranch[depth]++;
		}
	}
	if(fileiobuffersize!=0){
		while(fileiobuffersize<(BUFFERLENGTH-1)){
			fileiobuffer = fileiobuffer << 1;
			fileiobuffersize++;
		}
		fwrite(&fileiobuffer,1,1,file);
	}
	fclose(file);
	free(nodeslist);
	free(totalbranches);
	free(currentbranch);
}


// carrega a �rvore de um ficheiro
int loadTreeFromFile(char *filename){
	int depth,*totalbranches,*currentbranch;
	int nodenumbranches, nodecount;
	char nodelabel;
	int n;
	treenode *node,**nodeslist;
	FILE *file;
	//file=fopen(filename,"r");
	file=fopen(filename,"rb");
	if(file==NULL) return 0;
	fileiobuffersize=0;
	nodeslist=(treenode **)malloc((maxdepth+1)*sizeof(treenode *));
	totalbranches=(int *)calloc((maxdepth+1),sizeof(int));
	currentbranch=(int *)calloc((maxdepth+1),sizeof(int));
	nodenumbranches=0;
	nodelabel='X';
	nodecount=0;
	n=0;
	//fscanf(file,"%d %c %d\n",&nodenumbranches,&nodelabel,&nodecount);
	readNodeBits(file,&nodenumbranches,&nodelabel,&nodecount);
	//printf("%2d: %d %c %d\n",n++,nodenumbranches,nodelabel,nodecount);
	text=NULL;
	textsize=nodecount;
	maxdepth=LIMIT;
	numberofnodes=0;
	loadedfromfile=1;
	initializeSideLinksVector(maxdepth);
	root=newNode('@',0);
	root->count=nodecount;
	depth=0;
	node=root;
	nodeslist[depth]=node;
	totalbranches[depth]=nodenumbranches;
	currentbranch[depth]=0;
	while(1){
		if(currentbranch[depth]<totalbranches[depth]){
			//fscanf(file,"%d %c %d\n",&nodenumbranches,&nodelabel,&nodecount);
			readNodeBits(file,&nodenumbranches,&nodelabel,&nodecount);
			//printf("%2d: %d %c %d\n",n++,nodenumbranches,nodelabel,nodecount);
			node=addBranch(node,nodelabel);
			node->count=nodecount;
			depth++;
			nodeslist[depth]=node;
			currentbranch[depth]=0;
			totalbranches[depth]=nodenumbranches;
		}
		else{
			depth--;
			if(depth==-1) break;
			node=nodeslist[depth];
			currentbranch[depth]++;
		}
		/*
		if((n%100)==0) system("pause");
		if(n>10){
			for(i=0;i<=maxdepth;i++) printf("%d\t",i);
			printf("\n");
			for(i=0;i<=maxdepth;i++) printf("%d\t",totalbranches[i]);
			printf("\n");
			for(i=0;i<=maxdepth;i++) printf("%d\t",currentbranch[i]);
			printf("\n");
			for(i=0;i<depth;i++) printf("\t");
			printf("^\n");
			printf("n = %d\n",n);
		}
		*/
		//if(n>10) break;
	}
	fclose(file);
	free(nodeslist);
	free(totalbranches);
	free(currentbranch);
	return root->count;
}

// define o apontador para o texto que originou a �rvore
void setTreeText(char *txt){
	text=txt;
}

// mostra os bits de um ficheiro bin�rio
void printBinaryFile(char *filename){
	int i,n,k,nzeros,l;
	unsigned char c,res,mask;
	FILE *file;
	file=fopen(filename,"rb");
	if(file==NULL) return;
	n=0;
	l=0;
	nzeros=0;
	printf("%2d: ",n);
	while(!feof(file)){
		k=(int)fread(&c,1,1,file);
		if(k!=1) break;
		//printf("%d:",n);
		mask=(unsigned char)128;
		for(i=0;i<8;i++){
			l++;
			//if((i%4)==0) printf(" ");
			res = c & mask;
			if(res==zero) {printf("0");if(l>5) nzeros++;}
			else {printf("1");nzeros=0;}
			mask = mask >> 1;
			if(l==3 || l==5) printf("|");
			if(nzeros==4) {n++;printf("\n%2d: ",n);nzeros=0;l=0;}
		}
		//printf("\n");
		//n++;
		//if(n>10) break;
	}
	printf("\n");
}

// liberta a mem�ria alocada pela �rvore
void freeTreeNode(treenode *node){
	int i;
	for(i=0;i<ALPHABETSIZE;i++)
		if((node->branches[i])!=NULL){
			freeTreeNode(node->branches[i]);
			node->numbranches--;
		}
	//while((node->numbranches)>0){
	//	freeTreeNode(node->branches[node->numbranches-1]);
	//	node->numbranches--;
	//}
	free(node->branches);
	free(node);
	node=NULL;
}


// devolve o n�mero total de n�s da �rvore
int getNumberOfTreeNodes(treenode *node){
	return numberofnodes;
/*	int num=1;
	int i=0;
	int n=node->numbranches;
	for(i=0;i<n;i++)
		num+=getNumberOfTreeNodes(node->branches[i]);
	return num; */
}


// devolve o tamanho da mem�ria ocupada pela �rvore
int getTreeSize(treenode *node){
	return numberofnodes*sizeof(treenode);
/*	int size=0;
	int i=0;
	int n=node->numbranches;
	size+=sizeof(node);
	for(i=0;i<n;i++)
		size+=getTreeSize(node->branches[i]);
	return size; */
}


/********/
/* MAIN */
/********/
/*
int main(int argc, char *argv[]) {
	char string[255]="";
	char *treefile;
	int limit,steps;
	treenode *tree;
	//treenode *temp;
	time_t starttime,endtime;
	clock_t startclock,endclock;
	double timecount;
	clock_t clockcount;
	if(argc==2){
		if(argc<2) return 0;
		treefile=(char *)malloc(255*sizeof(char));
		strcpy(treefile,argv[1]);
		printf("file = <%s>\n",treefile);
		fflush(stdout);
		loadTreeFromFile(treefile);
		//printBinaryFile(treefile);
		//drawSuffixTree(stdout,root,1,1,"","");
		printf("number of nodes = %d\n",root->count);
		freeTreeNode(root);
		return 1;
	}
	while(1){
		printf("> text : ");
		if(argc==3){
			strcpy(string,argv[1]);
			printf("%s\n",string);}
		else scanf("%255s",string);
		printf("> limit: ");
		if(argc==3){
			limit=atoi(argv[2]);
			printf("%d\n",limit);
		}
		else scanf("%d",&limit);
		printf("\n");
		time(&starttime);
		startclock=clock();
		tree=buildTree(string,limit,&steps); // buildTree
		//temp=addBranch(tree,'G');
		//temp->count=275;
		//temp=addBranch(tree,'T');
		//temp->count=512;
		time(&endtime);
		endclock=clock();
		timecount=difftime(endtime,starttime);
		clockcount=(endclock-startclock)/CLOCKS_PER_SEC;
		//printSuffixTree(tree,0);
		drawSuffixTree(stdout,tree,1,1,"","");
		//printTreeToFile(tree,"tree.txt");
		saveTreeToFile("tree.txt");
		printBinaryFile("tree.txt");
		//printf(":: Tree saved.\n");
		freeTreeNode(tree);
		//printf(":: Tree cleaned.\n");
		//system("pause");
		loadTreeFromFile("tree.txt");
		tree=root;
		//printSuffixTree(tree,0);
		printf("\n");
		drawSuffixTree(stdout,root,1,1,"","");
		printf("\n");
		//printf(":: Tree loaded.\n\n");
		//printf("> time  = %d seconds\n",timecount);
		//printf("> clock = %d seconds\n",clockcount);
		//printf("> work  = %d steps\n",steps);
		//printf("> size  = %d bytes\n",getTreeSize(root));
		//printf("> space = %d nodes\n",getNumberOfTreeNodes(root));
		//printf("> length= %d chars\n",(int)strlen(string));
		//printf("\n");
		//system("pause");
		freeTreeNode(tree);
		if(argc==3) return 0;
	}
	system("pause");
}
*/

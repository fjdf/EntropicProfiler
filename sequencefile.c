#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sequencefile.h"

#define MAXDESCSIZE 80 // tamanho máximo da descrição
#define MAXDATASIZE 5000000 // tamanho máximo da sequência

/************************/
/* SEQUENCE FILE FORMAT */
/************************/


// cria uma nova sequência
sequence * newSequence(char *desc, char *data){
	sequence *new=(sequence *)malloc(sizeof(sequence));
	if(desc!=NULL){
		new->description=(char *)malloc(((int)strlen(desc)+1)*sizeof(char));
		strcpy(new->description,desc);
	}
	else new->description=NULL;
	if(data!=NULL){
		new->size=(long)strlen(data);
		new->data=(char *)malloc((new->size+1)*sizeof(char));
		strcpy(new->data,data);
	}
	else{
		new->size=0L;
		new->data=NULL;
	}
	return new;
}


// devolve uma string com a hora e data actuais
char * getTimestamp(){
	time_t timevalue;
	time(&timevalue);
	return asctime(localtime(&timevalue));
}


// FALTA: verificar se há mais do que uma sequência no ficheiro
// carrega a sequência a partir de um ficheiro
sequence * loadSequence(char * path){
	FILE *fich;
	sequence * seq;
	char ch, *desc, *data;
	//fpos_t k, datasize, pos, posend;
	long k, datasize;
	long startpos, endpos;
	if((fich=fopen(path,"r"))==NULL)
		return NULL;
	desc=NULL;
	k=0;
	desc=(char *)malloc(MAXDESCSIZE*sizeof(char));
	if((ch=fgetc(fich))=='>'){
		while((ch=fgetc(fich))!='\n' && ch!='>' && k<MAXDESCSIZE){
			desc[k]=ch;
			k++;
		}
		desc[k]='\0';
	}
	else{
		//desc=(char *)malloc(1*sizeof(char));
		//desc[0]='\0';
		strcpy(desc,"Sequence submitted on: ");
		strcat(desc,getTimestamp());
		desc[((int)strlen(desc)-1)]='\0';
		fseek(fich,0L,SEEK_SET);
	}
	//fgetpos(fich,&pos);
	startpos=ftell(fich);
	fseek(fich,0L,SEEK_END);
	endpos=ftell(fich);
	datasize=(endpos-startpos);
	//fgetpos(fich,&posend);
	//datasize=(posend-pos);
	//fsetpos(fich,&pos);
	fseek(fich,startpos,SEEK_SET);
	data=(char *)malloc((datasize+1)*sizeof(char));
	//for(k=0;((ch=fgetc(fich))!=EOF && ch!='>' && ch!='\n' && k<datasize);k++)
	//	data[k]=ch;
	k=0;
	while((ch=fgetc(fich))!=EOF && ch!='>' && ch!='\n' && ch!='\r')
		if(ch=='A' || ch=='C' || ch=='G' || ch=='T' || ch=='a' || ch=='c' || ch=='g' || ch=='t'){
			data[k]=ch;
			k++;
		}
	//k=(long)fread(data,1,datasize,fich);
	//if(data[k-1]=='\n' || data[k-1]=='>') data[k-1]='\0';
	data[k]='\0';
	if(fclose(fich)!=0) return NULL;
	//seq=newSequence(desc,data);
	seq=(sequence *)malloc(sizeof(sequence));
	seq->description=desc;
	seq->data=data;
	seq->size=k;
	return seq;
}


// mostra os dados da sequência
void printSequence(sequence *seq){
	printf("> %s\n",seq->description);
	printf("> %d\n",seq->size);
	if((seq->size)<100) printf("> %s\n",seq->data);
	else printf("> ...\n");
}


// liberta a memória alocada pela sequência
void freeSequence(sequence *seq){
	free(seq->data);
	free(seq->description);
	free(seq);
}

/*
int main(int argc, char *argv[]){
	sequence *s;
	char *filename;
	while(1){
		filename=(char *)malloc(32*sizeof(char));
		printf("file: ");
		if(argc==2) {
			strcpy(filename,argv[1]);
			printf("%s\n",filename);
		}
		else scanf("%32s",filename);
		//gets(filename);
		s=loadSequence(filename);
		if(s==NULL) return 0;
		printf("<%s>\n",filename);
		printSequence(s);
		//system("pause");
		freeSequence(s);
		free(filename);
		if(argc==2) return 1;
		printf("\n");
	}
}
*/

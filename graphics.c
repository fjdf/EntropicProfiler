#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <stdint.h>
#include "suffixtries.h"
#include "bitmap.h"
#include "graphics.h"

#define ALPHABETSIZE 31


// *****************
// VARI�EIS GLOBAIS
// *****************

static int **alphabet = NULL; // alfabeto
static int alphabetsize = 32; // tamanho do alfabeto
static int digitsize = 30; // tamanho dos caracteres
static uint8_t black,white,red,green,blue,yellow,purple,cyan,grey; // cores
static int bmpwidth = 320; // largura da imagem
static int bmpheight = 240; // altura da imagem


// ******************
// FUN�ES AUXILIARES
// ******************

// m�imo
int xymax(int x, int y){
	//return ( (x>y) ? x : y );
	if(x>y) return x;
	return y;
}

// m�imo
int xymin(int x, int y){
	//return ( (x<y) ? x : y );
	if(x<y) return x;
	return y;
}

// m�ulo positivo
// ex.: posmod(-12,10)=2
int posmod(int x, int y){
	int modres=x%y;
	if(modres<0) modres+=y;
	return modres;
}

// m�ulo negativo
// ex.: posmod(-12,10)=8
int negmod(int x, int y){
	int modres=x%y;
	if(modres>0) modres-=y;
	if(modres<0) modres=-modres;
	return modres;
}

// dist�cia entre dois pontos
int distance(int x, int y){
	return (int)floor(sqrt(pow((double)x,2.0)+pow((double)y,2.0)));
}

// troca o contedo de duas vari�eis
void switchint(int *i1, int *i2){
	int aux;
	aux=(*i1);
	(*i1)=(*i2);
	(*i2)=aux;
}

// compara dois valores inteiros (para usar no 'qsort')
int comp(void *a, void *b) {
	return ( ( (*(int *)a) > (*(int *)b) ) ? 1 : 0 );
}

// converte um double para um int (arredonda para baixo)
int double2int(double d){
	return (int)floor(d);
}

// converte um double para um int (arredonda para cima)
int double2intup(double d){
	//if(d<0) return -(int)floor(-d);
	return (int)ceil(d);
}

// converte uma lista de inteiros para uma lista de doubles
double * intlist2doublelist(int *intlist, int n){
	int i=0;
	double *doublelist;
	doublelist=(double *)malloc(n*sizeof(double));
	for(i=0;i<n;i++) doublelist[i]=((double)(intlist[i]));
	return doublelist;
}


// *********************
// PRIMITIVAS DE DESENHO
// *********************

// inicializa as vari�eis das cores
void initializeColors(){
	black=getColorFromPalette(0,0,0);
	white=getColorFromPalette(255,255,255);
	red=getColorFromPalette(255,0,0);
	green=getColorFromPalette(0,255,0);
	blue=getColorFromPalette(0,0,255);
	yellow=getColorFromPalette(255,255,0);
	purple=getColorFromPalette(255,0,255);
	cyan=getColorFromPalette(0,255,255);
	grey=getColorFromPalette(128,128,128);
}

// desenha uma linha horizontal (da esquerda para a direita)
int drawHLine(int x, int y, int size, uint8_t color){
	int i,pos,limit;
	pos=x;
	limit=bmpwidth;
	for(i=1;(i<=size && pos<limit);i++)
		drawPoint(pos++,y,color);	
	return i;
}

// desenha uma linha vertical (de cima para baixo / Y menor para Y maior)
int drawVLine(int x, int y, int size, uint8_t color){
	int i,pos,limit;
	pos=y;
	limit=bmpheight;
	for(i=1;(i<=size && pos<limit);i++)
		drawPoint(x,pos++,color);
	return i;
}

// desenha uma linha diagonal (da esquerda para a dreita)
int drawDLine(int x, int y, double m, int size, uint8_t color){
	int i,xpos,ypos,xlimit,ylimit,prevypos,step;
	xpos=x;
	ypos=y;
	xlimit=bmpwidth;
	ylimit=bmpheight;
	prevypos=y;
	if(m>0) step=1;
	else step=-1;
	for(i=0;(i<=size && xpos<=xlimit && ypos<=ylimit);i++){
		ypos=(int)floor(m*(double)i)+y;
		drawPoint(xpos,ypos,color);
		while(prevypos!=ypos){
			drawPoint(xpos-1,prevypos,color);
			prevypos=prevypos+step;
		}
		prevypos=ypos;
		xpos++;
	}
	return i;
}

// FALTA: alterar VLine e HLine e DLine para suportarem recta de sentido contr�io
// desenha uma linha (geral)
int drawLine(int x1, int y1, int x2, int y2, uint8_t color){
	int xdif,ydif,size;
	int xx1,xx2,yy1,yy2,xswitch,yswitch;
	double m,signal;
	xx1=x1;
	xx2=x2;
	yy1=y1;
	yy2=y2;
	xswitch=0;
	yswitch=0;
	if(x2<x1){
		xx2=x1;
		xx1=x2;
		xswitch=1;
	}
	if(y2<y1){
		yy2=y1;
		yy1=y2;
		yswitch=1;
	}
	xdif=xx2-xx1;
	ydif=yy2-yy1;
	if(xdif==0) return drawVLine(xx1,yy1,ydif,color);
	if(ydif==0) return drawHLine(xx1,yy1,xdif,color);
	if(xswitch==yswitch) signal=1.0;
	else signal=-1.0;
	if(xswitch==1) yy1=y2;
	else yy1=y1;
	m=((double)ydif/(double)xdif)*signal;
	size=xdif;
	return drawDLine(xx1,yy1,m,size,color);
}

// desenha um rect�gulo
void drawRectangle(int x, int y, int w, int h, uint8_t color){
	int i,j;
	for(i=0;i<w;i++)
		for(j=0;j<h;j++)
			drawPoint(x+i,y+j,color);
}

// desenha um c�culo
void drawCircle(int x, int y, int r, uint8_t color){
	int xx,yy,i;
	for(xx=1;xx<r;xx++){
		yy=(int)floor(sqrt(pow((double)r,2.0)-pow((double)xx,2.0)));
		//drawPoint(x+xx,y+yy,color);
		//drawPoint(x+xx,y-yy,color);
		//drawPoint(x-xx,y+yy,color);
		//drawPoint(x-xx,y-yy,color);
		for(i=-xx;i<=xx;i++) drawPoint(x+i,y+yy,color);
		for(i=-xx;i<=xx;i++) drawPoint(x+i,y-yy,color);
		for(i=-yy;i<=yy;i++) drawPoint(x+xx,y+i,color);
		for(i=-yy;i<=yy;i++) drawPoint(x-xx,y+i,color);
	}
	drawVLine(x,y-r+1,2*r-1,color);
}

// desenha uma estrela
void drawStar(int x, int y, int r, uint8_t color){
	int i;
	for(i=0;i<r;i++){
		drawPoint(x+i,y+i,color);
		drawPoint(x-i,y-i,color);
		drawPoint(x-i,y+i,color);
		drawPoint(x+i,y-i,color);
		drawPoint(x+i,y,color);
		drawPoint(x-i,y,color);
		drawPoint(x,y+i,color);
		drawPoint(x,y-i,color);
	}
}


// ********************
//  PRIMITIVAS DE TEXTO
// ********************

// cria um caracter de 6x5 pixels
int *newChar(int i1,int i2,int i3,int i4,int i5,int i6,int i7,int i8,int i9,int i10,int i11,int i12,int i13,int i14,int i15,int i16,int i17,int i18,int i19,int i20,int i21,int i22,int i23,int i24,int i25,int i26,int i27,int i28,int i29,int i30){
	int *digit=(int *)malloc(30*sizeof(int));
	digit[0]=i1;
	digit[1]=i2;
	digit[2]=i3;
	digit[3]=i4;
	digit[4]=i5;
	digit[5]=i6;
	digit[6]=i7;
	digit[7]=i8;
	digit[8]=i9;
	digit[9]=i10;
	digit[10]=i11;
	digit[11]=i12;
	digit[12]=i13;
	digit[13]=i14;
	digit[14]=i15;
	digit[15]=i16;
	digit[16]=i17;
	digit[17]=i18;
	digit[18]=i19;
	digit[19]=i20;
	digit[20]=i21;
	digit[21]=i22;
	digit[22]=i23;
	digit[23]=i24;
	digit[24]=i25;
	digit[25]=i26;
	digit[26]=i27;
	digit[27]=i28;
	digit[28]=i29;
	digit[29]=i30;
	return digit;
}

// inicializa o conjunto dos 32 caracteres a usar:
// '0' '1' '2' '3' '4' '5' '6' '7' '8' '9' '-' '.' '=' ' '
// 'N' 'X' 'i' 'L' 'f' 'A' 'C' 'G' 'T' 'U'
// '(phi)' '(delta)' '#' ':' 'P' 'V' 'E' 'M'
void initializeAlphabet(){
	int **digits;
	if(alphabet!=NULL) return;
	digits=(int **)malloc(32*sizeof(int *));
	
	digits[0]=newChar // '0'
	(0,1,1,1,0
	,1,0,0,0,1
	,1,0,0,0,1
	,1,0,0,0,1
	,1,0,0,0,1
	,0,1,1,1,0);
	digits[1]=newChar // '1'
	(0,0,1,0,0
	,0,1,1,0,0
	,1,0,1,0,0
	,0,0,1,0,0
	,0,0,1,0,0
	,1,1,1,1,1);
	digits[2]=newChar // '2'
	(0,1,1,1,0
	,1,0,0,0,1
	,0,0,0,0,1
	,0,0,1,1,0
	,0,1,0,0,0
	,1,1,1,1,1);
	digits[3]=newChar // '3'
	(1,1,1,1,1
	,0,0,0,1,0
	,0,0,1,1,0
	,0,0,0,0,1
	,1,0,0,0,1
	,0,1,1,1,0);
	digits[4]=newChar // '4'
	(0,0,0,1,0
	,0,0,1,1,0
	,0,1,0,1,0
	,1,1,1,1,1
	,0,0,0,1,0
	,0,0,0,1,0);
	digits[5]=newChar // '5'
	(1,1,1,1,1
	,1,0,0,0,0
	,1,1,1,1,0
	,0,0,0,0,1
	,0,0,0,0,1
	,1,1,1,1,0);
	digits[6]=newChar // '6'
	(0,1,1,1,1
	,1,0,0,0,0
	,1,1,1,1,0
	,1,0,0,0,1
	,1,0,0,0,1
	,0,1,1,1,0);
	digits[7]=newChar // '7'
	(1,1,1,1,1
	,0,0,0,0,1
	,0,0,0,1,0
	,0,0,1,0,0
	,0,1,0,0,0
	,1,0,0,0,0);
	digits[8]=newChar // '8'
	(0,1,1,1,0
	,1,0,0,0,1
	,0,1,1,1,0
	,1,0,0,0,1
	,1,0,0,0,1
	,0,1,1,1,0);
	digits[9]=newChar // '9'
	(0,1,1,1,0
	,1,0,0,0,1
	,1,0,0,0,1
	,0,1,1,1,1
	,0,0,0,0,1
	,1,1,1,1,0);

	digits[10]=newChar // '-'
	(0,0,0,0,0
	,0,0,0,0,0
	,1,1,1,1,1
	,0,0,0,0,0
	,0,0,0,0,0
	,0,0,0,0,0);
	digits[11]=newChar // '.'
	(0,0,0,0,0
	,0,0,0,0,0
	,0,0,0,0,0
	,0,0,0,0,0
	,0,0,1,1,0
	,0,0,1,1,0);	
	digits[12]=newChar // '='
	(0,0,0,0,0
	,1,1,1,1,1
	,0,0,0,0,0
	,1,1,1,1,1
	,0,0,0,0,0
	,0,0,0,0,0);
	digits[13]=newChar // ' '
	(0,0,0,0,0
	,0,0,0,0,0
	,0,0,0,0,0
	,0,0,0,0,0
	,0,0,0,0,0
	,0,0,0,0,0);

	digits[14]=newChar // 'N'
	(1,0,0,0,1
	,1,1,0,0,1
	,1,0,1,0,1
	,1,0,1,0,1
	,1,0,0,1,1
	,1,0,0,0,1);
	digits[15]=newChar // 'X'
	(1,1,0,1,1
	,1,1,0,1,1
	,0,1,1,1,0
	,0,1,1,1,0
	,1,1,0,1,1
	,1,1,0,1,1);
	digits[16]=newChar // 'i'
	(0,1,1,0,0
	,0,1,1,0,0
	,0,0,0,0,0
	,0,1,1,0,0
	,0,1,1,0,0
	,0,1,1,0,0);
	digits[17]=newChar // 'L'
	(1,1,0,0,0
	,1,1,0,0,0
	,1,1,0,0,0
	,1,1,0,0,0
	,1,1,0,0,0
	,1,1,1,1,1);
	digits[18]=newChar // 'f'
	(0,0,0,1,1
	,0,0,1,0,0
	,0,1,1,1,0
	,0,0,1,0,0
	,0,0,1,0,0
	,1,1,0,0,0);
	digits[19]=newChar // 'A'
	(0,0,1,0,0
	,0,1,1,1,0
	,1,1,0,1,1
	,1,1,1,1,1
	,1,1,0,1,1
	,1,1,0,1,1);
	digits[20]=newChar // 'C'
	(0,1,1,1,1
	,1,1,1,1,1
	,1,1,0,0,0
	,1,1,0,0,0
	,1,1,1,1,1
	,0,1,1,1,1);
	digits[21]=newChar // 'G'
	(0,1,1,1,1
	,1,1,0,0,0
	,1,1,0,1,1
	,1,1,0,0,1
	,1,1,1,1,1
	,0,1,1,1,1);
	digits[22]=newChar // 'T'
	(1,1,1,1,1
	,1,1,1,1,1
	,0,0,1,0,0
	,0,0,1,0,0
	,0,0,1,0,0
	,0,0,1,0,0);
	digits[23]=newChar // 'U'
	(1,1,0,0,1
	,1,1,0,0,1
	,1,1,0,0,1
	,1,1,0,0,1
	,1,1,1,1,1
	,0,1,1,1,0);

	digits[24]=newChar // '[phi]'
	(0,0,1,0,0
	,0,1,1,1,0
	,1,0,1,0,1
	,1,0,1,0,1
	,0,1,1,1,0
	,0,0,1,0,0);
	digits[25]=newChar // '[delta]'
	(0,0,1,0,0
	,0,1,0,1,0
	,0,1,0,1,0
	,1,0,0,0,1
	,1,0,0,0,1
	,1,1,1,1,1);
	digits[26]=newChar // '#'
	(0,1,0,1,0
	,1,1,1,1,1
	,0,1,0,1,0
	,0,1,0,1,0
	,1,1,1,1,1
	,0,1,0,1,0);
	digits[27]=newChar // ':'
	(0,1,1,0,0
	,0,1,1,0,0
	,0,0,0,0,0
	,0,0,0,0,0
	,0,1,1,0,0
	,0,1,1,0,0);
	digits[28]=newChar // 'P'
	(1,1,1,1,0
	,1,1,0,1,1
	,1,1,0,1,1
	,1,1,1,1,0
	,1,1,0,0,0
	,1,1,0,0,0);
	digits[29]=newChar // 'V'
	(1,1,0,1,1
	,1,1,0,1,1
	,1,1,0,1,1
	,1,1,0,1,1
	,0,1,1,1,0
	,0,0,1,0,0);
	digits[30]=newChar // 'E'
	(1,1,1,1,1
	,1,1,0,0,0
	,1,1,1,1,0
	,1,1,0,0,0
	,1,1,1,1,1
	,1,1,1,1,1);
	digits[31]=newChar // 'M'
	(1,0,0,0,1
	,1,1,0,1,1
	,1,1,1,1,1
	,1,1,0,1,1
	,1,1,0,1,1
	,1,1,0,1,1);

	alphabetsize=32;
	alphabet=digits;
}

// liberta a mem�ia alocada pelas vari�eis do alfabeto
void freeAlphabet(){
	int i,n=alphabetsize;
	if(alphabet==NULL) return;
	for(i=0;i<n;i++) free(alphabet[i]);
	free(alphabet);
	alphabet=NULL;
}

// converte o formato dos caracteres para formato bin�io com 30 bits (1 bit por pixel)
// ex: digit[2]==1 -> 000000000000000000000000000100 (base 2) -> 00000004 (base 10)
int *convertAlphabetToBinary(){
	int i,j,bindigit = 0;
	int *digit=NULL;
	int *bindigits=(int *)malloc(alphabetsize*sizeof(int));
	for(i=0;i<alphabetsize;i++){
		digit=(int *)alphabet[i];
		bindigit=0;
		for(j=29;j>=0;j--){
			bindigit = bindigit | (int)digit[j];
			bindigit = bindigit << 1;
		}
		bindigits[i]=bindigit;
		printf("@[%d] = %d = %u = %#x\n",i,bindigits[i],bindigit,bindigit);
	}
	return bindigits;
}

// devolve o �dice do caracter ASCII dado no alfabeto a usar
int alphabetCode(char c){
	int charcode=(int)c;
	int charpos=13; // (alphabetsize-1)
	if(charcode>=48 && charcode<=57) return charpos=charcode-48; // 0-9
	if(charcode==45) return charpos=10; // '-'
	if(charcode==46) return charpos=11; // '.'
	if(charcode==61) return charpos=12; // '='
	if(charcode==32) return charpos=13; // ' '
	if(charcode==78 || charcode==110) return charpos=14; // 'N','n'
	if(charcode==88 || charcode==120) return charpos=15; // 'X','x'
	if(charcode==73 || charcode==105) return charpos=16; // 'I','i'
	if(charcode==76 || charcode==108) return charpos=17; // 'L','l'
	if(charcode==70 || charcode==102) return charpos=18; // 'F','f'
	if(charcode==65 || charcode==(65+32)) return charpos=19; // 'A','a'
	if(charcode==67 || charcode==(67+32)) return charpos=20; // 'C','c'
	if(charcode==71 || charcode==(71+32)) return charpos=21; // 'G','g'
	if(charcode==84 || charcode==(84+32)) return charpos=22; // 'T','t'
	if(charcode==85 || charcode==(85+32)) return charpos=23; // 'U','u'
	if(charcode==42) return charpos=24; // '*' -> '(phi)'
	if(charcode==38) return charpos=25; // '&' -> '(delta)'
	if(charcode==35) return charpos=26; // '#'
	if(charcode==58) return charpos=27; // ':'
	if(charcode==80 || charcode==(80+32)) return charpos=28; // 'P','p'
	if(charcode==86 || charcode==(86+32)) return charpos=29; // 'V','v'
	if(charcode==69 || charcode==(69+32)) return charpos=30; // 'E','e'
	if(charcode==77 || charcode==(77+32)) return charpos=31; // 'E','e'
	return charpos;
}

// desenha o caracter na posi�o indicada da imagem
void drawChar(int n, int x, int y){
	int i,j,k;
	int *digit;
	int **digits;
	digits=alphabet;
	if(digits==NULL) return;
	digit=digits[n];
	for(i=0;i<6;i++)
		for(j=0;j<5;j++){
			k=5*i+j;
			if(digit[k]!=0)
				drawPoint(x+j,y+i,black);
		}
}

// mostra todos os caracteres do alfabeto no topo da imagem
void printAlphabet(){
	int i,n=alphabetsize;
	int x=2,y=2;
	for(i=0;i<n;i++) drawChar(i,x+6*i,y);
}

// imprime os caracteres ASCII e o seu respectivo c�igo
void printAsciiChars(){
	int i;
	for(i=32;i<=255;i++){ // '0' -> 48
		printf("%c => %d\n",(char)i,(int)i);
	}
}

// devolve o nmero de algarismos de um nmero
int digitCount(int n){
	int left,count;
	if(n==0) return 1;
	left=n;
	count=0;
	if(n<0){
		left=-n;
		count=1;
	}
	while(left!=0){
		left=left/10;
		count++;
	}
	return count;
}

// desenha o nmero na posi�o indicada da imagem
void drawNumber(int n, int x, int y){
	int left,i,k;
	int *digits;
	k=digitCount(n);
	if(n==0) {
		drawChar(0,x,y);
		return;
	}
	digits=(int *)malloc(k*sizeof(int));
	left=n;
	if(n<0){
		left=-n;
		digits[k-1]=10;
	}
	i=0;
	while(left!=0){
		digits[i]=left%10;
		left=left/10;
		i++;
	}
	for(i=0;i<k;i++) drawChar(digits[k-i-1],x+i*5+i,y);
	free(digits);
}

// desenha o nmero decimal na posi�o indicada da imagem
void drawDecimalNumber(double n, int x, int y){
        int intpart,decpart;
	int intpartsize;
	intpart=(int)floor(n);
	decpart=(int)floor((n-(double)intpart)*10);
	drawNumber(intpart,x,y);
	intpartsize=digitCount(intpart);
	drawText(".",x+6*intpartsize,y);
	drawNumber(decpart,x+6*(intpartsize+1),y);
}

// desenha o nmero alinhado �esquerda (termina na posi�o dada)
void drawNumberAtLeft(int n, int x, int y){
	drawNumber(n,x-digitCount(n)*6,y);
}

// desenha o nmero alinhado ao centro (da posi�o dada)
void drawNumberAtCenter(int n, int x, int y){
	drawNumber(n,x-digitCount(n)*3,y);
}

// desenha o texto na posi�o indicada da imagem
void drawText(char *text, int x, int y){
	int i,k;
	int *chars;
	int charpos;
	char currentchar;
	if(alphabet==NULL) initializeAlphabet();
	k=32;
	chars=(int *)malloc(k*sizeof(int));
	currentchar=text[0];
	for(i=0;(i<k && currentchar!='\0');i++){
		charpos=alphabetCode(currentchar);
		drawChar(charpos,x+6*i,y);
		currentchar=text[i+1];
	}
	free(chars);
}

// desenha o texto alinhado �esquerda (termina na posi�o dada)
void drawTextAtLeft(char *text, int x, int y){
	drawText(text,x-(int)strlen(text)*(5+1),y);
}

// desenha o texto alinhado ao centro
void drawTextAtCenter(char *text, int x, int y){
	int n=((int)strlen(text))*(5+1);
	drawText(text,x-(n/2),y);
}

// FALTA: melhorar com strcats
// desenha uma legenda centrada no topo da imagem
void drawLabel(int l, int p, int i, char *seqtext){
	int n;
	char *labeltext;
	n=(int)strlen(seqtext);
	labeltext=(char *)malloc((20+n)*sizeof(char));
	if(i<=0 && l<=0) sprintf(labeltext,"*=%d ",p);
	else if(l<=0) sprintf(labeltext,"*=%d XI=%d ",p,i);
	else if(p<=0) sprintf(labeltext,"L=%d XI=%d ",l,i);
	else if(i<=0) sprintf(labeltext,"L=%d *=%d ",l,p);
	else sprintf(labeltext,"L=%d *=%d XI=%d ",l,p,i);
	if(seqtext!=NULL && n!=0) strcat(labeltext,seqtext);
	n=((int)strlen(labeltext))*(5+1);
	drawText(labeltext,(bmpwidth/2)-(n/2),10);
	free(labeltext);
}


// ********
// GR�ICOS
// ********

// inicializa as vari�eis da imagem, do alfabeto e das cores
void initializeGraphics(int w, int h){
	initializeBitmap(w,h);
	initializeColors();
	initializeAlphabet();
	bmpwidth=getBitmapWidth();
	bmpheight=getBitmapHeight();
}

// desenha uma lista de pontos num gr�ico
void drawPlot(double *xlist, double *ylist, int n, int joinpoints, char *xlabel, char *ylabel, int starat) {
	double dxmin, dymin, dxmax, dymax;
	int xmin, ymin, xmax, ymax;
	int xsize, ysize;
	int xmargin, ymargin;
	int xaxessize, yaxessize;
	int xnmarks, ynmarks;
	int xmarkstart, ymarkstart;
	int xmaxdigits, ymaxdigits;
	int xstep, ystep;
	int xstartpos, ystartpos;
	int xendpos, yendpos;
	int xpoint, ypoint;
	int prevxpoint, prevypoint;
	int dashsize, subdashsize, pointsize, number, i;
	double xunitsize, yunitsize;
	double decnumber;
	dxmin=xlist[0];
	dxmax=xlist[0];
	dymin=ylist[0];
	dymax=ylist[0];
	// identificar pontos m�imos e m�imos
	for(i=0;i<n;i++){
		if(xlist[i]>dxmax) dxmax=xlist[i];
		if(xlist[i]<dxmin) dxmin=xlist[i];
		if(ylist[i]>dymax) dymax=ylist[i];
		if(ylist[i]<dymin) dymin=ylist[i];
	}
	// converter valores double para int
	xmin=double2int(dxmin);
	ymin=double2int(dymin);
	xmax=double2intup(dxmax);
	ymax=double2intup(dymax);
	// nmero m�imo de digitos
	xmaxdigits=xymax(digitCount(xmax),digitCount(xmin));
	ymaxdigits=xymax(digitCount(ymax),digitCount(ymin));
	// margens do gr�ico em rela�o aos extremos da imagem
	xmargin=bmpwidth/12; // horizontal
	ymargin=bmpheight/12; // vertical
	// comprimento dos tra�s das marca�es
	dashsize=xmargin/4;
	subdashsize=dashsize/2;
	// tamanho dos pontos
	pointsize=xymax(bmpwidth,bmpheight)/240+1;
	// verificar se a largura da margem da esquerda �suficiente
	i=6*ymaxdigits+dashsize+2+2;
	if(i>xmargin) xmargin=i;
	// tamanho dos eixos em pixels
	xaxessize=bmpwidth-2*xmargin;
	yaxessize=bmpheight-2*ymargin;
	// tamanho dos eixos em unidades
	xsize=xmax-xmin+1;
	ysize=ymax-ymin+1;
	// comprimento em pixels de uma unidade nos eixos
	xunitsize=((double)xaxessize)/((double)(xsize-1));
	yunitsize=((double)yaxessize)/((double)(ysize-1));
	// desenhar eixos
	drawLine(xmargin,ymargin,xmargin,bmpheight-ymargin,black); // YY esquerda
	drawLine(xmargin,bmpheight-ymargin,bmpwidth-xmargin,bmpheight-ymargin,black); // XX baixo
	drawLine(bmpwidth-xmargin,ymargin,bmpwidth-xmargin,bmpheight-ymargin+1,black); // YY direita
	drawLine(xmargin,ymargin,bmpwidth-xmargin,ymargin,black); // XX cima
	// reduzir o nmero de marca�es at�caberem todas nos eixos
	xstep=1; // espa� em unidades entre as marca�es
	ystep=1;
	xnmarks=xsize; // nmero de marca�es
	ynmarks=ysize;
	/*
	while(((5+2)*xmaxdigits*xnmarks)>xaxessize){ // XX
		xstep=xstep*10; // aumentar intervalo entre as marca�es
		xnmarks=xsize-(negmod(xmin,xstep)+posmod(xmax,xstep)); // tirar espa� n� usado nos extremos
		xnmarks=(xnmarks/xstep)+1; // actualizar nmero de marca�es
	}
	while(((6+2)*ynmarks)>yaxessize){ // YY
		ystep=ystep*10;
		ynmarks=ysize-(negmod(ymin,ystep)+posmod(ymax,ystep));
		ynmarks=(ynmarks/ystep)+1;
	}
	*/
	while(1){ // XX
	        if(((5+2)*xmaxdigits*xnmarks)<=xaxessize) break;
		xstep=xstep*2;
		xnmarks=xsize-(negmod(xmin,xstep)+posmod(xmax,xstep));
		xnmarks=(xnmarks/xstep)+1;
	        if(((5+2)*xmaxdigits*xnmarks)<=xaxessize) break;
		xstep=xstep*5;
		xnmarks=xsize-(negmod(xmin,xstep)+posmod(xmax,xstep));
		xnmarks=(xnmarks/xstep)+1;
	}
	while(1){ // YY
	        if(((6+2)*ynmarks)<=yaxessize) break;
		ystep=ystep*2;
		ynmarks=ysize-(negmod(ymin,ystep)+posmod(ymax,ystep));
		ynmarks=(ynmarks/ystep)+1;
	        if(((6+2)*ynmarks)<=yaxessize) break;
		ystep=ystep*5;
		ynmarks=ysize-(negmod(ymin,ystep)+posmod(ymax,ystep));
		ynmarks=(ynmarks/ystep)+1;
	}
	// desenhar marca�es nos eixos
	xmarkstart=xmin+negmod(xmin,xstep);
	ymarkstart=ymin+negmod(ymin,ystep);
	for(i=0;i<xnmarks;i++) { // XX - verticais
		number=xmarkstart+i*xstep; // nmero na marca�o
		xstartpos=xmargin+double2int(((double)(number-xmin))*xunitsize);
		ystartpos=bmpheight-ymargin;
		xendpos=xstartpos;
		yendpos=ystartpos+dashsize;
		drawLine(xstartpos,ystartpos,xendpos,yendpos,black);
		drawNumber(number,xstartpos-2-6*(digitCount(number)-1)/2,yendpos+2);
		//desenhar submarca�es verticais
		if(xstartpos==(bmpwidth-xmargin) || xstartpos==xmargin) continue;
		ystartpos=ystartpos-1;
		yendpos=ystartpos-subdashsize;
		while(yendpos>(ymargin-subdashsize)){
			yendpos=xymax(yendpos,(ymargin+1));
			drawLine(xstartpos,ystartpos,xendpos,yendpos,grey);
			ystartpos-=2*subdashsize;
			yendpos=ystartpos-subdashsize;
		}
	}
	if(ynmarks==2){ // marca�es decimais se necessario
	  for(i=0;i<=10;i++) {
	        decnumber=(double)ymarkstart+i*0.1;
		xstartpos=xmargin-dashsize;
		ystartpos=bmpheight-ymargin-double2int(i*(((double)yaxessize)/10.0));
		xendpos=xmargin;
		yendpos=ystartpos;
		drawLine(xstartpos,ystartpos,xendpos,yendpos,black);
		drawDecimalNumber(decnumber,xmargin-dashsize-2-6*(ymaxdigits+2)+1,yendpos-2);
		//desenhar submarca�es horizontais
		if(ystartpos==(bmpheight-ymargin) || ystartpos==ymargin) continue;
		xstartpos=xendpos+2;
		xendpos=xstartpos+subdashsize;
		while(xendpos<(bmpwidth-xmargin+subdashsize)){
			xendpos=xymin(xendpos,(bmpwidth-xmargin-1));
			drawLine(xstartpos,ystartpos,xendpos,yendpos,grey);
			xstartpos+=2*subdashsize;
			xendpos=xstartpos+subdashsize;
		}
	  }
	} else for(i=0;i<ynmarks;i++) { // YY - horizontais
		number=ymarkstart+i*ystep;
		xstartpos=xmargin-dashsize;
		ystartpos=bmpheight-ymargin-double2int(((double)(number-ymin))*yunitsize);
		xendpos=xmargin;
		yendpos=ystartpos;
		drawLine(xstartpos,ystartpos,xendpos,yendpos,black);
		drawNumber(number,xmargin-dashsize-2-6*digitCount(number)+1,yendpos-2);
		//desenhar submarca�es horizontais
		if(ystartpos==(bmpheight-ymargin) || ystartpos==ymargin) continue;
		xstartpos=xendpos+2;
		xendpos=xstartpos+subdashsize;
		while(xendpos<(bmpwidth-xmargin+subdashsize)){
			xendpos=xymin(xendpos,(bmpwidth-xmargin-1));
			drawLine(xstartpos,ystartpos,xendpos,yendpos,grey);
			xstartpos+=2*subdashsize;
			xendpos=xstartpos+subdashsize;
		}
	}
	// escrever legendas dos eixos
	drawText(xlabel,bmpwidth-xmargin+dashsize+(2+5),bmpheight-ymargin+dashsize+2); // XX
	drawText(ylabel,xmargin-dashsize-(2+5)-6*((int)strlen(ylabel)-1),ymargin-dashsize-6); // YY
	/*
	drawNumber(ymin,2,10);
	drawNumber(ymax,2,2);
	drawNumber(xmin,bmpwidth-6*digitCount(xmin)-2,10);
	drawNumber(xmax,bmpwidth-6*digitCount(xmax)-2,2);
	drawNumber(ynmarks,2,18);
	drawNumber(xnmarks,bmpwidth-6*digitCount(xnmarks)-2,18);
	*/
	// desenhar pontos
	for(i=0;i<n;i++){
		xpoint=xmargin+double2int((xlist[i]-(double)xmin)*xunitsize);
		ypoint=bmpheight-ymargin-double2int((ylist[i]-(double)ymin)*yunitsize)-1;
		drawCircle(xpoint,ypoint,pointsize,red);
		if((starat>0) && (xlist[i]==starat)) drawStar(xpoint,ypoint,4,red);
		// desenhar linhas a ligar os pontos
		if(joinpoints!=0 && i>0) drawLine(prevxpoint,prevypoint,xpoint,ypoint,blue);
		//drawLine(xmargin,ypoint,xpoint,ypoint,cyan);
		//drawLine(xpoint,ypoint,xpoint,bmpheight-ymargin,cyan);
		prevxpoint=xpoint;
		prevypoint=ypoint;
	}
}

// FALTA: retirar c�igo desnecess�io
// FALTA: passar fun�es para fora
// desenha um gr�ico de barras
void drawHistogram(double *xlist, double *ylist, int n, int startx) {
	double dxmin, dymin, dxmax, dymax;
	int xmin, ymin, xmax, ymax;
	int xsize, ysize;
	int xmargin, ymargin;
	int xaxessize, yaxessize;
	int xnmarks, ynmarks;
	int xmarkstart, ymarkstart;
	int xmaxdigits, ymaxdigits;
	int xstep, ystep;
	int xstartpos, ystartpos;
	int xendpos, yendpos;
	int xpoint, ypoint;
	int dashsize, subdashsize, number, i;
	double xunitsize, yunitsize;
	int barwidth, barheight, barlabelstep;
	unsigned int colorval, color;
	dxmin=(double)startx; // ALTERADO
	dxmax=xlist[0];
	dymin=ylist[0];
	dymax=ylist[0];
	// identificar pontos m�imos e m�imos
	for(i=0;i<n;i++){
		if(xlist[i]>dxmax) dxmax=xlist[i];
		if(xlist[i]<dxmin) dxmin=xlist[i];
		if(ylist[i]>dymax) dymax=ylist[i];
		if(ylist[i]<dymin) dymin=ylist[i];
	}
	// converter valores double para int
	xmin=0; // ALTERADO
	ymin=0; // ALTERADO
	xmax=n; // ALTERADO
	ymax=double2intup(dymax);
	// nmero m�imo de digitos
	xmaxdigits=xymax(digitCount(double2intup(dxmax)),digitCount(double2intup(dxmin))); // ALTERADO
	ymaxdigits=xymax(digitCount(ymax),digitCount(ymin));
	// margens do gr�ico em rela�o aos extremos da imagem
	xmargin=bmpwidth/12; // horizontal
	ymargin=bmpheight/12; // vertical
	// comprimento dos tra�s das marca�es
	dashsize=xmargin/4;
	subdashsize=dashsize/2;
	// tamanho dos pontos
	// verificar se a largura da margem da esquerda �suficiente
	i=6*ymaxdigits+dashsize+2+2;
	if(i>xmargin) xmargin=i;
	// tamanho dos eixos em pixels
	xaxessize=bmpwidth-2*xmargin;
	yaxessize=bmpheight-2*ymargin;
	// tamanho dos eixos em unidades
	xsize=n; // ALTERADO
	ysize=ymax-ymin+1;
	// comprimento em pixels de uma unidade nos eixos
	xunitsize=((double)xaxessize)/((double)(n)); // ALTERADO
	yunitsize=((double)yaxessize)/((double)(ysize-1));
	// desenhar eixos
	drawLine(xmargin,ymargin,xmargin,bmpheight-ymargin,black); // YY esquerda
	drawLine(xmargin,bmpheight-ymargin,bmpwidth-xmargin,bmpheight-ymargin,black); // XX baixo
	drawLine(bmpwidth-xmargin,ymargin,bmpwidth-xmargin,bmpheight-ymargin+1,black); // YY direita
	drawLine(xmargin,ymargin,bmpwidth-xmargin,ymargin,black); // XX cima
	// reduzir o nmero de marca�es at�caberem todas nos eixos
	xstep=1; // espa� em unidades entre as marca�es
	ystep=1;
	xnmarks=n; // nmero de marca�es // ALTERADO
	ynmarks=ysize;
	while(((6+2)*ynmarks)>yaxessize){ // YY
		ystep=ystep*10; // aumentar intervalo entre as marca�es
		ynmarks=ysize-(negmod(ymin,ystep)+posmod(ymax,ystep)); // tirar espa� n� usado nos extremos
		ynmarks=(ynmarks/ystep)+1; // actualizar nmero de marca�es
	}
	barlabelstep=1;
	while(((5+2)*xmaxdigits*(xnmarks/barlabelstep))>xaxessize){ // XX
	        barlabelstep++;
	}
	// desenhar marca�es nos eixos
	xmarkstart=0; // ALTERADO
	ymarkstart=ymin+negmod(ymin,ystep);
	number=startx; // ALTERADO
	for(i=0;i<=xnmarks;i++) { // XX - verticais
		if(i!=0) number=double2int(xlist[i-1]); // nmero na marca�o // ALTERADO
		xstartpos=xmargin+double2int(((double)(i))*xunitsize); // ALTERADO
		ystartpos=bmpheight-ymargin;
		xendpos=xstartpos;
		yendpos=ystartpos+dashsize/2;
		if( ( i==xnmarks ) || ( (i%barlabelstep)==0 && 
		     ( i!=(xnmarks-1) || (xstartpos+7*xmaxdigits)<(bmpwidth-xmargin) ) ) ){
		        yendpos+=dashsize/2;
		        drawNumber(number,xstartpos-2-6*(digitCount(number)-1)/2,yendpos+2);
		}
		drawLine(xstartpos,ystartpos,xendpos,yendpos,black);
		//desenhar submarca�es verticais
		if(xstartpos==(bmpwidth-xmargin) || xstartpos==xmargin) continue;
		ystartpos=ystartpos-1;
		yendpos=ystartpos-subdashsize;
		while(yendpos>(ymargin-subdashsize)){
			yendpos=xymax(yendpos,(ymargin+1));
			drawLine(xstartpos,ystartpos,xendpos,yendpos,grey);
			ystartpos-=2*subdashsize;
			yendpos=ystartpos-subdashsize;
		}
	}
	for(i=0;i<ynmarks;i++) { // YY - horizontais
		number=ymarkstart+i*ystep;
		xstartpos=xmargin-dashsize;
		ystartpos=bmpheight-ymargin-double2int(((double)(number-ymin))*yunitsize);
		xendpos=xmargin;
		yendpos=ystartpos;
		drawLine(xstartpos,ystartpos,xendpos,yendpos,black);
		drawNumber(number,xmargin-dashsize-2-6*digitCount(number)+1,yendpos-2);
		//desenhar submarca�es horizontais
		if(ystartpos==(bmpheight-ymargin) || ystartpos==ymargin) continue;
		xstartpos=xendpos+2;
		xendpos=xstartpos+subdashsize;
		while(xendpos<(bmpwidth-xmargin+subdashsize)){
			xendpos=xymin(xendpos,(bmpwidth-xmargin-1));
			drawLine(xstartpos,ystartpos,xendpos,yendpos,grey);
			xstartpos+=2*subdashsize;
			xendpos=xstartpos+subdashsize;
		}
	}
	// escrever legendas dos eixos
	drawText("X",bmpwidth-xmargin+(xmaxdigits/2)*(5+1)+(2+5),bmpheight-ymargin+dashsize+2); // XX
	drawText("#",xmargin-dashsize-(2+5),ymargin-dashsize-6); // YY
	// desenhar legenda de cor
	xpoint=bmpwidth-xmargin+(xmaxdigits/2)*(5+1)+(2+5);
	for(ypoint=(bmpheight-ymargin);ypoint>=ymargin;ypoint--){
		colorval=double2int(255.0*(1.0-((double)(ypoint-ymargin))/((double)(bmpheight-2*ymargin))));
		if(colorval>127) color=getColorFromPalette(255,0,2*(255-colorval));
		else color=getColorFromPalette(2*colorval,0,255);
		for(i=0;i<5;i++) drawPoint(xpoint+i,ypoint,color);
	}
	drawLine(xpoint-1,(bmpheight-ymargin),xpoint+6,(bmpheight-ymargin),black);
	drawLine(xpoint-1,ymargin,xpoint+6,ymargin,black);
	drawLine(xpoint-1,(bmpheight-ymargin),xpoint-1,ymargin,black);
	drawLine(xpoint+5,(bmpheight-ymargin),xpoint+5,ymargin,black);
	// desenhar barras
	for(i=0;i<n;i++){
		xpoint=xmargin+double2int(((double)i)*xunitsize); // ALTERADO
		ypoint=bmpheight-ymargin-double2int((ylist[i]-(double)ymin)*yunitsize);
		barheight=double2int((ylist[i]-(double)ymin)*yunitsize);
		colorval=double2int(255.0*(ylist[i]/dymax));
		if(colorval>127) color=getColorFromPalette(255,0,2*(255-colorval));
		else color=getColorFromPalette(2*colorval,0,255);
		barwidth=double2int(((double)(i+1))*xunitsize)-double2int(((double)(i))*xunitsize);
		drawRectangle(xpoint,ypoint,barwidth,barheight,color);
		drawLine(xpoint,ypoint+barheight,xpoint,ypoint,black);
		drawLine(xpoint,ypoint,xpoint+barwidth,ypoint,black);
		drawLine(xpoint+barwidth,ypoint,xpoint+barwidth,ypoint+barheight,black);
	}
}


// desenha linhas diagonais de teste
void testLines(){
	drawLine(bmpwidth/2-16,bmpheight,bmpwidth/2+16,0,green);// D/L->U/R
	drawLine(bmpwidth/2+16,bmpheight,bmpwidth/2-16,0,red);// D/R->U/L
	drawLine(bmpwidth/2-32,0,bmpwidth/2+32,bmpheight,cyan);// U/L->D/R
	drawLine(bmpwidth/2+32,0,bmpwidth/2-32,bmpheight,yellow);// U/R->D/L
	drawLine(0,bmpheight/2+16,bmpwidth,bmpheight/2-16,green);// D/L->U/R
	drawLine(bmpwidth,bmpheight/2+16,0,bmpheight/2-16,red);// D/R->U/L
	drawLine(0,bmpheight/2-32,bmpwidth,bmpheight/2+32,cyan);// U/L->D/R
	drawLine(bmpwidth,bmpheight/2-32,0,bmpheight/2+32,yellow);// U/R->D/L
}

// desenha um gr�ico de teste
void testPlot(int start, int n){
	int i;
	double *xlist, *ylist;
	double d;
	xlist=(double *)malloc(n*sizeof(double));
	ylist=(double *)malloc(n*sizeof(double));
	for(i=0;i<n;i++){
		d=(double)(start+i);
		xlist[i]=d;
		ylist[i]=d*d;
		//ylist[i]=d*cos(d);
		//ylist[i]=d+d*d+d*d*d;
	}
	drawPlot(xlist,ylist,n,1,"","",0);
}

// desenha um histograma de teste
void testHistogram(int n){
	int i;
	double *xlist, *ylist;
	double d;
	xlist=(double *)malloc(n*sizeof(double));
	ylist=(double *)malloc(n*sizeof(double));
	for(i=0;i<n;i++){
		d=(double)(i+1);
		xlist[i]=10*d;
		ylist[i]=2*d;
	}
	drawHistogram(xlist,ylist,n,0);
}

// FALTA: corrigir repeti�o dos primeiros valores
void buildCGRMap(int n){
	int i;
	int squaresize;
	int xpos, ypos;
	int current;
	char *charsvector, *label;
	int *vector, *sizevector;
	treenode *treeroot,*node,**sidelinksvector;
	int count, mincount, maxcount;
	double percentage;
	int colorval,coloradd;
	unsigned char color;
	treeroot=getSuffixTreeRoot();
	//total=(double)((treeroot->count)-n+1);
	sidelinksvector=getSideLinksVector();
	node=sidelinksvector[n];
	maxcount=0;
	mincount=(node->count);
	while(node!=NULL){
		if((node->count)>maxcount) maxcount=(node->count);
		if((node->count)<mincount) mincount=(node->count);
		node=(node->sidelink);
	}
	//printf("mincount=%d\nmaxcount=%d\n",mincount,maxcount);
	charsvector=(char *)malloc((n+1)*sizeof(char));
	vector=(int *)calloc((n+1),sizeof(int));
	sizevector=(int *)calloc((n+1),sizeof(int));
	squaresize=15;
	sizevector[n]=squaresize+1;
	for(i=(n-1);i>=0;i--) sizevector[i]=2*sizevector[i+1]+double2int(pow(2.0,(n-1-i)));
	//sizevector[i]=double2int(pow(2.0,(n-i)))*(squaresize+1)+(n-i)*(n-i+1);
	for(i=0;i<n;i++) charsvector[i]='A';
	charsvector[n]='\0';
	xpos=sizevector[0]-double2int(pow(2.0,n));
	initializeGraphics(xpos+3*8+6*xymax(3,digitCount(maxcount))+2,xpos+2*8);
	xpos=8;
	ypos=8;
	i=n;
	while(1){
		current=vector[i];
		if(current==1) {charsvector[(n-i)]='C'; xpos+=sizevector[i];}
		else if(current==2) {charsvector[(n-i)]='T'; ypos+=sizevector[i];}
		else if(current==3) {charsvector[(n-i)]='G'; xpos-=sizevector[i];}
		else if(current==4) {
			charsvector[(n-i)]='A';
			ypos-=sizevector[i];
			vector[i]=0;
			i--;
			if(i==0) break;
			vector[i]++;
			continue;
		}
		i=n;
		count=getSubStringCount(treeroot,charsvector);
		percentage=((double)count)/((double)maxcount);
		colorval=double2int(255.0*percentage);
		coloradd=255-xymax(colorval,255-colorval);
		color=getColorFromPalette(colorval+coloradd,(255-colorval)+coloradd,0);
		drawRectangle(xpos,ypos,(sizevector[i]-1),(sizevector[i]-1),color);
		//printf("%d:[%s]=%d\n",i,charsvector,count);
		vector[i]++;
	}
	xpos=sizevector[0]-double2int(pow(2.0,n));
	drawText("A",1,1);
	drawText("C",8+xpos+2,1);
	drawText("T",8+xpos+2,bmpheight-7);
	drawText("G",1,bmpheight-7);
	//drawLabel(0,0,0,"L=3");

	// desenhar legenda de cor
	xpos+=3*8;
	for(ypos=(bmpheight/4);ypos<=(3*bmpheight/4);ypos++){
		colorval=double2int(255.0*(1.0-((double)(ypos-(bmpheight/4)))/((double)(bmpheight/2))));
		if(colorval>127) color=getColorFromPalette(255,2*(255-colorval),0);
		else color=getColorFromPalette(2*colorval,255,0);
		for(i=0;i<6*3;i++) drawPoint(xpos+i,ypos,color);
	}
	drawLine(xpos-1,(3*bmpheight/4),xpos+6*3,(3*bmpheight/4),black); // h
	drawLine(xpos-1,(bmpheight/4),xpos+6*3,(bmpheight/4),black); // h
	drawLine(xpos-1,(3*bmpheight/4),xpos-1,(bmpheight/4),black); // v
	drawLine(xpos+6*3-1,(3*bmpheight/4),xpos+6*3-1,(bmpheight/4),black); // v
	drawNumberAtCenter(maxcount,xpos+(6*3)/2,(bmpheight/4)-8);
	drawNumberAtCenter(0,xpos+(6*3)/2,3*(bmpheight/4)+2);
	label=(char *)malloc((3+digitCount(n))*sizeof(char));
	sprintf(label,"L=%d",n);
	drawTextAtCenter(label,xpos+(6*3)/2,(bmpheight/4)-3*8);
	free(label);
	
	saveBitmap("cgrmap.bmp");
	free(charsvector);
	free(vector);
	free(sizevector);
}


// ****
// MAIN
// ****

// FALTA: gr�icos com mltiplas fun�es
// FALTA: gr�ico de representa�o das frequ�cias relativas das letras
// FALTA: eixos com nmeros decimais
// FALTA: c�culo autom�ico da largura e altura da imagem
/*
int main(int argc, char *argv[]){
	char *file;
	int i, n, *xlist, *ylist;

	file=(char *)malloc(255*sizeof(char));
	strcpy(file,"plot.bmp");
	
	initializeGraphics(320,240);
	//initializeGraphics(400,320);
	//initializeGraphics(32,32);

	//printAlphabet();
	//drawText("N=0 XI=ACTGU L=0 P=0",2,2);
	
	if(argc>3){
		n=atoi(argv[1]);
		printf("n=%d\n",n);
		xlist=(int *)malloc(n*sizeof(int));
		ylist=(int *)malloc(n*sizeof(int));
		for(i=0;(i<n && (2+2*i+1)<=argc); i++){
			xlist[i]=atoi(argv[2+2*i]);
			ylist[i]=atoi(argv[2+2*i+1]);
			printf("[%d]=(%d,%d)\n",i,xlist[i],ylist[i]);
		}
		n=i;
		printf("n=%d\n",n);
		drawPlot(intlist2doublelist(xlist,n),intlist2doublelist(ylist,n),n,1);
	}
	else{
		testHistogram(4);
		//testPlot(-32,64);
		//testPlot(-6,13);
		//testLines();
		//testBitmap(0);
	}
	saveBitmap(file);
	//showFileHexData(file);
	//freeBitmap();
	//system("pause");
	return 1;
}
*/

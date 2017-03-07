#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <stdint.h>
#include "bitmap.h"


// *****************
// VARIÁVEIS GLOBAIS
// *****************

static Bitmap *bitmap = NULL; // Bitmap
static uint8_t *image = NULL; // superfície de desenho
static int width = 320; // largura da imagem
static int height = 240; // altura da imagem


// ****************
// BITMAP FUNCTIONS
// ****************

// cria uma cor no formato red/green/blue
RGBColor *newRGBColor(uint8_t r, uint8_t g, uint8_t b){
	RGBColor *color=(RGBColor *)malloc(sizeof(RGBColor));
	color->red=r;
	color->green=g;
	color->blue=b;
	color->unused=0;
	return color;
}

// devolve a posição na palette da cor mais próxima da cor dada
uint8_t getColorFromPalette(uint8_t r, uint8_t g, uint8_t b){
	RGBColor *color=(RGBColor *)(bitmap->palette->colors);
	int n=bitmap->information->numberofcolors;
	int dif,mindif;
	uint8_t i,bestpos;
	bestpos=0;
	mindif=3*255;
	for(i=0;i<n;i++){
		dif=abs((color->red)-r)+abs((color->green)-g)+abs((color->blue)-b);
		if(dif==0) return i;
		if(dif<mindif){
			mindif=dif;
			bestpos=i;
		}
		color++;
	}
	return bestpos;
}

// cria uma nova BitmapPalette (com 1, 8, 27, 64, 125 ou 216 cores)
BitmapPalette *newBitmapPalette(int *ncolors){
	BitmapPalette *pal;
	uint8_t *values;
	int n,i,j,k,step,pos;
	n=2; // 8 cores
	if((*ncolors)>=27) n=3;
	if((*ncolors)>=64) n=4;
	if((*ncolors)>=125) n=5;
	if((*ncolors)>=216) n=6;
	(*ncolors)=n*n*n;
	step=256/(n-1);
	values=(uint8_t *)malloc(n*sizeof(uint8_t));
	values[0]=255;
	for(i=1;i<n;i++) values[i]=(256-i*step);
	values[(n-1)]=0;
	pal=(BitmapPalette *)malloc(sizeof(BitmapPalette));
	pal->colors=(RGBColor *)calloc((*ncolors),sizeof(RGBColor));
	pos=0;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			for(k=0;k<n;k++){
				pal->colors[pos].red=values[i];
				pal->colors[pos].green=values[j];
				pal->colors[pos].blue=values[k];
				pal->colors[pos].unused=0;
				pos++;
			}
		}
	}
	return pal;
}

// cria uma nova BitmapPalette (optimizada)
BitmapPalette *newBitmapPaletteOptimized(int *ncolors){
	BitmapPalette *pal;
	uint8_t *values;
	int n,i,j,k,step,pos;
	(*ncolors)=148; // numero de cores
	pal=(BitmapPalette *)malloc(sizeof(BitmapPalette));
	pal->colors=(RGBColor *)calloc((*ncolors),sizeof(RGBColor));
	values=(uint8_t *)malloc(20*sizeof(uint8_t));
	n=2; // 2*2*2 = 8 cores: red/green/blue
	step=255/(n-1);
	for(i=0;i<n;i++) values[i]=(255-i*step);
	pos=0;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			for(k=0;k<n;k++){
				pal->colors[pos].red=values[i];
				pal->colors[pos].green=values[j];
				pal->colors[pos].blue=values[k];
				pal->colors[pos].unused=0;
				pos++;
			}
		}
	}
	n=20; // 1*20 = 20 cores: grey
	step=255/(n+1);
	for(i=0;i<n;i++) values[i]=(255-(i+1)*step);
	for(i=0;i<n;i++){
		pal->colors[pos].red=values[i];
		pal->colors[pos].green=values[i];
		pal->colors[pos].blue=values[i];
		pal->colors[pos].unused=0;
		pos++;
	}
	n=20; // 2*20 = 40 cores: red+green
	step=255/(n-1);
	for(i=0;i<n;i++) values[i]=(255-i*step);
	values[(n-1)]=0;
	for(i=0;i<n;i++){
		pal->colors[pos].red=255;
		pal->colors[pos].green=values[i];
		pal->colors[pos].blue=0;
		pal->colors[pos].unused=0;
		pos++;
		pal->colors[pos].red=values[i];
		pal->colors[pos].green=255;
		pal->colors[pos].blue=0;
		pal->colors[pos].unused=0;
		pos++;
	}
	n=20; // 2*20 = 40 cores: green+blue
	step=255/(n-1);
	for(i=0;i<n;i++) values[i]=(255-i*step);
	values[(n-1)]=0;
	for(i=0;i<n;i++){
		pal->colors[pos].red=0;
		pal->colors[pos].green=255;
		pal->colors[pos].blue=values[i];
		pal->colors[pos].unused=0;
		pos++;
		pal->colors[pos].red=0;
		pal->colors[pos].green=values[i];
		pal->colors[pos].blue=255;
		pal->colors[pos].unused=0;
		pos++;
	}
	n=20; // 2*20 = 40 cores: red+blue
	step=255/(n-1);
	for(i=0;i<n;i++) values[i]=(255-i*step);
	values[(n-1)]=0;
	for(i=0;i<n;i++){
		pal->colors[pos].red=255;
		pal->colors[pos].green=0;
		pal->colors[pos].blue=values[i];
		pal->colors[pos].unused=0;
		pos++;
		pal->colors[pos].red=values[i];
		pal->colors[pos].green=0;
		pal->colors[pos].blue=255;
		pal->colors[pos].unused=0;
		pos++;
	}
	free(values);
	return pal;
}

// cria uma nova BitmapData (e guarda o tamanho na variável npixels)
BitmapData *newBitmapData(int width, int height, long int *npixels){
	BitmapData *dat;
	dat=(BitmapData *)malloc(sizeof(BitmapData));
	(*npixels)=((long)width)*((long)height);
	while(((*npixels)%4)!=0) (*npixels)++; // por definição tem de ser múltiplo de 4
	dat->pixels=(uint8_t *)calloc((*npixels),sizeof(uint8_t));
	return dat;
}

// cria um novo Bitmap
Bitmap *newBitmap(int width, int height){
	Bitmap *bmp;
	BitmapHeader *hdr;
	BitmapInformation *inf;
	BitmapPalette *pal;
	BitmapData *dat;
	long int npixels=0;
	//int ncolors=216; // número de cores a usar (k^3): 8, 27, 64, 125, 216
	int ncolors=255;
	int sizeofheader=14;
	int sizeofinformation=40;
	
	hdr=(BitmapHeader *)malloc(sizeof(BitmapHeader));
	//hdr=(BitmapHeader *)malloc(sizeofheader);
	hdr->filetype=19778u; // 0x424D = 19778u = 'BM'
	hdr->filesize=(unsigned long)0;
	hdr->reserved1=0u;
	hdr->reserved2=0u;
	hdr->dataoffset=(unsigned long)0;
	
	inf=(BitmapInformation *)malloc(sizeof(BitmapInformation));
	inf->headersize=40l;
	inf->width=(long)width;
	inf->height=(long)height;
	inf->numberofplanes=1u;
	inf->bitsperpixel=8u;
	inf->compressionmethod=0l; // BI_RGB
	inf->datasize=0l;
	inf->horizontalpixelspermeter=1024l;
	inf->verticalpixelspermeter=1024l;
	//inf->numberofcolors=(long)ncolors;
	//inf->numberofimportantcolors=(long)ncolors;
	
	//pal=newBitmapPalette(&ncolors);
	
	pal=newBitmapPaletteOptimized(&ncolors);
	inf->numberofcolors=(long)ncolors;
	inf->numberofimportantcolors=(long)ncolors;

	dat=newBitmapData(width,height,&npixels);
	
	bmp=(Bitmap *)malloc(sizeof(Bitmap));
	inf->datasize=(unsigned long)npixels;
	hdr->dataoffset=(unsigned long)(sizeofheader)+(unsigned long)(sizeofinformation)+(unsigned long)(ncolors*4);
	hdr->filesize=(unsigned long)((hdr->dataoffset)+(inf->datasize));
	
	bmp->header=hdr;
	bmp->information=inf;
	bmp->palette=pal;
	bmp->data=dat;
	return bmp;
}

// liberta a memória alocada pelo Bitmap
void freeBitmap(){
	Bitmap *bmp=bitmap;
	free(bmp->data->pixels); // tem problema ao desalocar a memória
	free(bmp->data);
	free(bmp->palette->colors);
	free(bmp->palette);
	free(bmp->information);
	free(bmp->header); // tem problema ao desalocar a memória
	free(bmp);
}

// salva o Bitmap para um ficheiro
int saveBitmap(char *filename){
	Bitmap *bmp=bitmap;
	FILE *file=NULL;
	int byteswritten=0;
	long int sizeofpalette=0;
	long int sizeofdata=0;
	if((file=fopen(filename,"wb"))==NULL) return 0;
	compressBitmapData(); // comprimir o Bitmap
	sizeofpalette=(long)(bmp->information->numberofcolors);
	sizeofdata=(long)(bmp->information->datasize);
	//byteswritten+=(int)fwrite(bmp->header,sizeof(*(bmp->header)),1,file);
	byteswritten+=(int)fwrite(&(bmp->header->filetype),sizeof(uint16_t),1,file);
	byteswritten+=(int)fwrite(&(bmp->header->filesize),sizeof(uint32_t),1,file);
	byteswritten+=(int)fwrite(&(bmp->header->reserved1),sizeof(uint16_t),1,file);
	byteswritten+=(int)fwrite(&(bmp->header->reserved2),sizeof(uint16_t),1,file);
	byteswritten+=(int)fwrite(&(bmp->header->dataoffset),sizeof(uint32_t),1,file);
	byteswritten+=(int)fwrite(bmp->information,sizeof(*(bmp->information)),1,file);
	byteswritten+=(int)fwrite(bmp->palette->colors,sizeof(RGBColor),sizeofpalette,file);
	byteswritten+=(int)fwrite(bmp->data->pixels,sizeof(uint8_t),sizeofdata,file);
	if(fclose(file)==EOF) return 0;
	return (byteswritten==(bmp->header->filesize));
	freeBitmap();
}

// inicializa variáveis do bitmap
void initializeBitmap(int w, int h){
	bitmap=newBitmap(w,h);
	image=bitmap->data->pixels;
	width=w;
	height=h;
}

// devolve a altura da imagem em pixels
int getBitmapHeight(){
	return (int)(bitmap->information->height);
}

// devolve a largura da imagem em pixels
int getBitmapWidth(){
	return (int)(bitmap->information->width);
}

// mostra na imagem todas as cores disponíveis na palete
void testBitmap(int method){
	Bitmap *bmp;
	long int ncolors,npixels;
	long int i,j,step;
	uint8_t *pixel,colornumber;
	bmp=bitmap;
	ncolors=bmp->information->numberofcolors;
	npixels=bmp->information->datasize;
	//printf("#pixels=%ld\n",npixels);
	//printf("#colors=%ld\n",ncolors);
	if(method==0){
		step=npixels/ncolors;
		colornumber=0;
		pixel=(uint8_t *)(bmp->data->pixels);
		for(i=0,j=0;i<npixels;i++,j++){
			if(j==step){colornumber++;j=0;}
			*pixel=colornumber;
			pixel++;
		}
		return;
	}
	colornumber=0;
	step=width/ncolors;
	pixel=(uint8_t *)(bmp->data->pixels);
	for(i=0,j=0;i<npixels;i++,j++){
		*pixel=colornumber;
		pixel++;
		if(((i+1)%width)==0){colornumber=0;j=0;}
		if(((j+1)%step)==0){colornumber++;}
	}
}

// comprime a BitmapData com o algoritmo Run Length Encoding
void compressBitmapData(){
	Bitmap *bmp;
	uint8_t *originaldatastart,*compresseddatastart;
	uint8_t *originaldata,*compresseddata,*startpos,*endpos;
	uint8_t specialcode,endofbitmapcode;
	uint8_t count,byte,nextbyte,equalbyte,stop;
	long int originalsize,compressedsize,i;
	char mode;
	bmp=bitmap;
	specialcode=0x00;
	endofbitmapcode=0x01;
	originaldatastart=bmp->data->pixels;
	originaldata=originaldatastart;
	originalsize=bmp->information->datasize;
	compresseddatastart=(uint8_t *)malloc(originalsize*sizeof(uint8_t));
	compresseddata=compresseddatastart;
	compressedsize=0;
	stop=0;
	count=0;
	mode='0';
	for(i=1;i<=originalsize;i++){
		byte=(uint8_t)(*originaldata);
		count++;
		if(i!=originalsize){ // se não é o último byte podemos obter o nextbyte
			nextbyte=(uint8_t)(*(originaldata+1));
			if(nextbyte==byte) equalbyte=1;
			else equalbyte=0;
		} // lidar com o último byte ou se se chegou ao limite do contador ou ao fim da linha
		if(i==originalsize || count==255 || (i%width)==0){
			stop=1;
			if(mode=='0') mode='R';
			if(mode=='R') equalbyte=0;
			if(mode=='D'){
				equalbyte=1;
				originaldata++;
				count++;
			}
		}
		if(mode=='0'){ // em nenhum modo
			if(equalbyte) mode='R'; // contar bytes repetidos
			if(!equalbyte){
				mode='D'; // contar bytes distintos
				startpos=originaldata;
			}
			originaldata++;
			continue;
		}
		if(mode=='R'){
			if(!equalbyte){ // encontrou byte diferente, terminar modo
				(*compresseddata)=count;
				compresseddata++;
				(*compresseddata)=byte;
				compresseddata++;
				compressedsize+=2;
				mode='0';
				count=0;
				stop=0;
			}
		}
		if(mode=='D'){
			if(equalbyte && count<=3){ // se só havia 1 ou 2 bytes distintos gravar em modo R
				endpos=originaldata-1;
				count--;
				while(startpos<=endpos){
					(*compresseddata)=01;
					compresseddata++;
					(*compresseddata)=(*startpos);
					compresseddata++;
					compressedsize+=2;
					startpos++;
				}
				mode='R';
				count=1;
				if(stop){
					mode='0';
					count=0;
					stop=0;
				}
			}
			if(equalbyte && count>3){ // encontrou byte igual, terminar modo D
				endpos=originaldata-1;
				count--;
				(*compresseddata)=specialcode;
				compresseddata++;
				(*compresseddata)=count;
				compresseddata++;
				compressedsize+=2;
				while(startpos<=endpos){ // copiar bytes distintos
					(*compresseddata)=(*startpos);
					compresseddata++;
					compressedsize++;
					startpos++;
				}
				while((count%2)!=0){ // preencher com 00 se for ímpar
					(*compresseddata)=specialcode;
					compresseddata++;
					compressedsize++;
					count++;
				}
				mode='R'; // mudar para o modo de contagem de repetições
				count=1;
				if(stop){
					mode='0';
					count=0;
					stop=0;
				}
			}
		}
		originaldata++;
		if(compressedsize>=originalsize) return;
		if((i%width)==0){ // fim da linha
			(*compresseddata)=specialcode;
			compresseddata++;
			(*compresseddata)=specialcode;
			compresseddata++;
			compressedsize+=2;
		}
	}
	(*compresseddata)=specialcode;
	compresseddata++;
	(*compresseddata)=endofbitmapcode;
	compresseddata++;
	compressedsize+=2;
	while((compressedsize%4)!=0){
		(*compresseddata)=specialcode;
		compresseddata++;
		compressedsize++;
	}
	//if(compressedsize>originalsize){} // erro
	free(bmp->data->pixels);
	bmp->data->pixels=compresseddatastart;
	bmp->information->compressionmethod=1l; // BI_RLE8
	bmp->information->datasize=compressedsize;
	bmp->header->filesize=(bmp->header->dataoffset)+(bmp->information->datasize);
}

// mostra o tamanho em bytes dos diferentes tipos de dados
void printTypeSizes(){
	printf("sizeof(uint8_t) = %d\n",sizeof(uint8_t));
	printf("sizeof(uint16_t) = %d\n",sizeof(uint16_t));
	printf("sizeof(uint32_t) = %d\n",sizeof(uint32_t));
	printf("sizeof(char) = %d\n",sizeof(char));
	printf("sizeof(short) = %d\n",sizeof(short));
	printf("sizeof(int) = %d\n",sizeof(int));
	printf("sizeof(long) = %d\n",sizeof(long));
	printf("sizeof(double) = %d\n",sizeof(double));
}

// mostra o conteúdo binário do ficheiro
int showFileHexData(char *filename){
	FILE *file=NULL;
	FILE *output=NULL;
	char *outputname=(char *)malloc((strlen(filename)+5)*sizeof(char));
	int bytesread=0;
	unsigned char c;
	outputname=strcpy(outputname,filename);
	outputname=strcat(outputname,".txt");
	output=fopen(outputname,"w");
	if((file=fopen(filename,"rb"))==NULL) return 0;
	fprintf(output,"[%s]\n",filename);
	while(fread(&c,1,1,file)!=0){
		fprintf(output,"[%.4d][%.4X] %.2X %.3u %.1c\n",bytesread,bytesread,c,c,c);
		bytesread++;
	}
	fprintf(output,"[%d]\n",bytesread);
	fclose(output);
	if(fclose(file)==EOF) return 0;
	return bytesread;
}


// *********************
// PRIMITIVAS DE DESENHO
// *********************

// devolve a posição na BitmapData das coordenadas (x,y) na imagem
int dataPosition(int x, int y){
	if(x<0 || y<0 || x>=width || y>=height) return -1;
	return (x + (height - y - 1)*width);
}

// define a cor do pixel na posição (x,y)
// Nota: a posição de Y cresce de cima para baixo na imagem
void drawPoint(int x, int y, uint8_t colorpos){
	if(x<0 || y<0 || x>=width || y>=height) return;
	*((uint8_t *)(image + x + (height - y - 1)*width))=colorpos;
}

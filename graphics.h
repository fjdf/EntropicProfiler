//#include "bitmap.h"
#define uint8_t unsigned char

// ********
// GRAPHICS
// ********

// inicializa as vari�veis da imagem, do alfabeto e das cores
void initializeGraphics(int w, int h);

// desenha uma lista de pontos num gr�fico
void drawPlot(double *xlist, double *ylist, int n, int joinpoints, char *xlabel, char *ylabel, int starat);

// desenha uma legenda centrada no topo da imagem
void drawLabel(int l, int p, int i, char *seqtext);

// desenha o texto na posi��o indicada da imagem
void drawText(char *text, int x, int y);

// desenha o n�mero na posi��o indicada da imagem
void drawNumber(int n, int x, int y);

// desenha o n�mero alinhado � esquerda (termina na posi��o dada)
void drawNumberAtLeft(int n, int x, int y);

// desenha o n�mero alinhado ao centro (da posi��o dada)
void drawNumberAtCenter(int n, int x, int y);

// liberta a mem�ria alocada pelas vari�veis do alfabeto
void freeAlphabet();

// devolve o n�mero de algarismos de um n�mero
int digitCount(int n);

// desenha um gr�fico de barras
void drawHistogram(double *xlist, double *ylist, int n, int startx);

void buildCGRMap(int n);

// *******
// DRAWING
// *******
/*
// desenha uma linha (geral)
int drawLine(int x1, int y1, int x2, int y2, uint8_t color);

// inicializa as vari�veis das cores
void initializeColors();
*/

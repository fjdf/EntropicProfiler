//#include "bitmap.h"
#define uint8_t unsigned char

// ********
// GRAPHICS
// ********

// inicializa as variáveis da imagem, do alfabeto e das cores
void initializeGraphics(int w, int h);

// desenha uma lista de pontos num gráfico
void drawPlot(double *xlist, double *ylist, int n, int joinpoints, char *xlabel, char *ylabel, int starat);

// desenha uma legenda centrada no topo da imagem
void drawLabel(int l, int p, int i, char *seqtext);

// desenha o texto na posição indicada da imagem
void drawText(char *text, int x, int y);

// desenha o número na posição indicada da imagem
void drawNumber(int n, int x, int y);

// desenha o número alinhado à esquerda (termina na posição dada)
void drawNumberAtLeft(int n, int x, int y);

// desenha o número alinhado ao centro (da posição dada)
void drawNumberAtCenter(int n, int x, int y);

// liberta a memória alocada pelas variáveis do alfabeto
void freeAlphabet();

// devolve o número de algarismos de um número
int digitCount(int n);

// desenha um gráfico de barras
void drawHistogram(double *xlist, double *ylist, int n, int startx);

void buildCGRMap(int n);

// *******
// DRAWING
// *******
/*
// desenha uma linha (geral)
int drawLine(int x1, int y1, int x2, int y2, uint8_t color);

// inicializa as variáveis das cores
void initializeColors();
*/

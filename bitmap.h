//#include <stdint.h>
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long

// ****************
// BITMAP STRUCTURE
// ****************

//  _______________________________________
// |bits|bytes|  type  |     name     |info|
// |----|-----|--------|--------------|----|
// | 8  |  1  |uint8_t |unsigned char |#256|
// |----|-----|--------|--------------|----|
// | 16 |  2  |uint16_t|unsigned short|    |
// |----|-----|--------|--------------|----|
// | 32 |  4  |uint32_t|unsigned long |int |
// '----'-----'--------'--------------'----'

typedef struct _RGBColor {
	uint8_t blue; //
	uint8_t green;
	uint8_t red; //
	uint8_t unused;
} RGBColor;

typedef struct _BitmapHeader {
	uint16_t filetype; // 'BM'
	uint32_t filesize; // 14 + 40 + 4*numberofcolors + width*height
	uint16_t reserved1; // '0'
	uint16_t reserved2; // '0'
	uint32_t dataoffset;
} BitmapHeader;

typedef struct _BitmapInformation {
	uint32_t headersize; // '40'
	uint32_t width;
	uint32_t height;
	uint16_t numberofplanes; // '1'
	uint16_t bitsperpixel; // '8'
	uint32_t compressionmethod; // '0'
	uint32_t datasize;
	uint32_t horizontalpixelspermeter;
	uint32_t verticalpixelspermeter;
	uint32_t numberofcolors;
	uint32_t numberofimportantcolors;
} BitmapInformation;
	
typedef struct _BitmapPalette {
	RGBColor *colors;
} BitmapPalette;
	
typedef struct _BitmapData {
	uint8_t *pixels;
} BitmapData;

typedef struct _Bitmap {
	BitmapHeader *header;
	BitmapInformation *information;
	BitmapPalette *palette;
	BitmapData *data;
} Bitmap;


// ******************
// FUNÇÕES RELEVANTES
// ******************

// mostra na imagem todas as cores disponíveis na palete
void testBitmap(int method);

// comprime a BitmapData com o algoritmo Run Length Encoding
void compressBitmapData();

// mostra o conteúdo binário do ficheiro
int showFileHexData(char *filename);

// inicializa variáveis do bitmap
void initializeBitmap(int w, int h);

// salva o Bitmap para um ficheiro
int saveBitmap(char *filename);

// liberta a memória alocada pelo Bitmap
void freeBitmap();

// devolve a altura da imagem em pixels
int getBitmapHeight();

// devolve a largura da imagem em pixels
int getBitmapWidth();

// devolve a posição na palette da cor mais próxima da cor RGB dada
uint8_t getColorFromPalette(uint8_t r, uint8_t g, uint8_t b);

// devolve a posição na BitmapData das coordenadas (x,y) na imagem
int dataPosition(int x, int y);

// define a cor do pixel na posição (x,y)
void drawPoint(int x, int y, uint8_t colorpos);

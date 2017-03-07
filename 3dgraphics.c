#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"
#include "graphics.h"
#include "3dgraphics.h"

typedef struct _point2d {
	double x;
	double y;
} point2d;

typedef struct _point3d {
	double x;
	double y;
	double z;
} point3d;

static int height;
static int width;

static int hmargin;
static int vmargin;

static int originx;
static int originy;
static point2d *origin2d=NULL;
static point3d *origin3d=NULL;
static point2d *vectorx;
static point2d *vectory;
static point2d *vectorz;
static point2d *unitvectorx;
static point2d *unitvectory;
static point2d *unitvectorz;

static int xendpointx;
static int xendpointy;
static int yendpointx;
static int yendpointy;
static int zendpointx;
static int zendpointy;
static point2d *xendpoint2d;
static point2d *yendpoint2d;
static point2d *zendpoint2d;

static double zlow;
static double zhigh;

static int nx;
static int ny;
static int nz;

static double xsize;
static double ysize;
static double zsize;

static double xunit;
static double yunit;
static double zunit;


static unsigned char black,white,red,green,blue,yellow,purple,cyan,grey,lightgrey;

void printPoint2D(point2d *p){
	printf("(%4.2f,%4.2f)",p->x,p->y);
}

void printPoint3D(point3d *p){
	printf("(%4.2f,%4.2f,%4.2f)",p->x,p->y,p->z);
}

void Plot3DInfo(){
	printf("height=%d\nwidth=%d\n",height,width);
	printf("Hmargin=%d\nVmargin=%d\n",hmargin,vmargin);
	printf("origin2D=");printPoint2D(origin2d);printf("\n");
	printf("origin3D=");printPoint3D(origin3d);printf("\n");
	printf("vectorX=");printPoint2D(vectorx);printf("\n");
	printf("vectorY=");printPoint2D(vectory);printf("\n");
	printf("vectorZ=");printPoint2D(vectorz);printf("\n");
	printf("unitvectorX=");printPoint2D(unitvectorx);printf("\n");
	printf("unitvectorY=");printPoint2D(unitvectory);printf("\n");
	printf("unitvectorZ=");printPoint2D(unitvectorz);printf("\n");
	printf("Xendpoint2D=");printPoint2D(xendpoint2d);printf("\n");
	printf("Yendpoint2D=");printPoint2D(yendpoint2d);printf("\n");
	printf("Zendpoint2D=");printPoint2D(zendpoint2d);printf("\n");
	printf("nX=%d\nnX=%d\nnZ=%d\n",nx,ny,nz);
	printf("Xsize=%f\nYsize=%f\nZsize=%f\n",xsize,ysize,zsize);
	printf("Xunit=%f\nYunit=%f\nZunit=%f\n",xunit,yunit,zunit);
}


void setupColors(){
	black=getColorFromPalette(0,0,0);
	white=getColorFromPalette(255,255,255);
	red=getColorFromPalette(255,0,0);
	green=getColorFromPalette(0,255,0);
	blue=getColorFromPalette(0,0,255);
	yellow=getColorFromPalette(255,255,0);
	purple=getColorFromPalette(255,0,255);
	cyan=getColorFromPalette(0,255,255);
	grey=getColorFromPalette(128,128,128);
	lightgrey=getColorFromPalette(240,240,240);
}


int dtoi(double d){
	return (int)floor(d);
}

int moddtoi(double d){
	if(d<0) return dtoi(-d);
	return dtoi(d);
}

point2d * newPoint2D(double x, double y){
	point2d *newpoint;
	newpoint=(point2d *)malloc(sizeof(point2d));
	newpoint->x=x;
	newpoint->y=y;
	return newpoint;
}

point3d * newPoint3D(double x, double y, double z){
	point3d *newpoint;
	newpoint=(point3d *)malloc(sizeof(point3d));
	newpoint->x=x;
	newpoint->y=y;
	newpoint->z=z;
	return newpoint;
}

point2d * vector2D(point2d *p1, point2d *p2){
	return newPoint2D((p2->x)-(p1->x),(p2->y)-(p1->y));
}

point3d * vector3D(point3d *p1, point3d *p2){
	return newPoint3D((p2->x)-(p1->x),(p2->y)-(p1->y),(p2->z)-(p1->z));
}

point2d *sumVector2D(point2d *v1, point2d *v2){
	return newPoint2D((v1->x)+(v2->x),(v1->y)+(v2->y));
}

point3d *sumVector3D(point3d *v1, point3d *v2){
	return newPoint3D((v1->x)+(v2->x),(v1->y)+(v2->y),(v1->z)+(v2->z));
}

double vectorSize2D(point2d *p){
	return sqrt((p->x)*(p->x)+(p->y)*(p->y));
}

double convert3Dto2DX(double x, double y, double z){
	return (originx + x*(unitvectorx->x) + y*(unitvectory->x) + z*(unitvectorz->x));
}

double convert3Dto2DY(double x, double y, double z){
	return (originy + x*(unitvectorx->y) + y*(unitvectory->y) + z*(unitvectorz->y));
}

double convert3Dto2DXp(point3d *p){
	return (originx + (p->x)*(unitvectorx->x) + (p->y)*(unitvectory->x) + (p->z)*(unitvectorz->x));
}

double convert3Dto2DYp(point3d *p){
	return (originy + (p->x)*(unitvectorx->y) + (p->y)*(unitvectory->y) + (p->z)*(unitvectorz->y));
}

point2d *convert3Dto2Dp(point3d *p){
	return newPoint2D(convert3Dto2DXp(p),convert3Dto2DYp(p));
}

void drawPoint2D(double x, double y, unsigned char color){
	int xx,yy;
	xx=dtoi(x);
	yy=dtoi(y);
	drawPoint(xx,yy,color);
	drawPoint(xx+1,yy,color);
	drawPoint(xx,yy+1,color);
	drawPoint(xx+1,yy+1,color);
}

void drawPoint2Dp(point2d *p, unsigned char color){
	drawPoint2D(p->x,p->y,color);
}

void drawPoint3D(double x, double y, double z, unsigned char color){
	drawPoint2D(convert3Dto2DX(x,y,z),convert3Dto2DY(x,y,z),color);
}

void drawPoint3Dp(point3d *p, unsigned char color){
	drawPoint3D(p->x,p->y,p->z,color);
}

// FALTA: fazer vers� optimizada no bitmap.c
void drawLine2D(double x1, double y1, double x2, double y2, unsigned char color){
	int i;
	double xstart,ystart;
	double xdif,ydif,slope;
	double xmoddif,ymoddif;
	xdif=(x2-x1);
	ydif=(y2-y1);
	if(xdif<0) xmoddif=-xdif;
	else xmoddif=xdif;
	if(ydif<0) ymoddif=-ydif;
	else ymoddif=ydif;
	if( (xmoddif>=ymoddif) && (xdif!=0) ){
		slope=ydif/xdif;
		if(xdif<0) {xstart=x2;ystart=y2;}
		else {xstart=x1;ystart=y1;}
		for(i=0;i<=xmoddif;i++)
			drawPoint(dtoi(xstart+i),dtoi(ystart+slope*i),color);
	}
	else if( (ydif!=0) ){
		slope=xdif/ydif;
		if(ydif<0) {xstart=x2;ystart=y2;}
		else {xstart=x1;ystart=y1;}
		for(i=0;i<=ymoddif;i++)
			drawPoint(dtoi(xstart+slope*i),dtoi(ystart+i),color);
	}
}

void drawLine2Dp(point2d *p1, point2d *p2, unsigned char color){
	drawLine2D(p1->x,p1->y,p2->x,p2->y,color);
	/*
	int i,itempx,itempy;
	double tempx,tempy;
	int xpos, ypos;
	double m;
	xpos=dtoi(p1->x);
	ypos=dtoi(p1->y);
	tempx=(p2->x)-(p1->x);
	tempy=(p2->y)-(p1->y);
	itempx=dtoi(tempx);
	itempy=dtoi(tempy);
	if(itempx==0){ // vertical
		if(itempy>0) for(i=0;i<=itempy;i++) drawPoint(xpos,ypos++,color);
		if(itempy<0) for(i=0;i<=(-itempy);i++) drawPoint(xpos,ypos--,color);
		return;
	}
	if(itempy==0){ // horizontal
		if(itempx>0) for(i=0;i<=itempx;i++) drawPoint(xpos++,ypos,color);
		if(itempx<0) for(i=0;i<=(-itempx);i++) drawPoint(xpos--,ypos,color);
		return;
	}
	m=((double)tempy)/((double)tempx);
	if(itempx>0) for(i=0;i<=itempx;i++)
			drawPoint((xpos+i),(ypos+dtoi(m*i)),color);
	else if(itempx<0) for(i=0;i<=(-itempx);i++)
			drawPoint((xpos-i),(ypos-dtoi(m*i)),color);
	m=((double)tempx)/((double)tempy);
	if(itempy>0) for(i=0;i<=itempy;i++)
			drawPoint((xpos+dtoi(m*i)),(ypos+i),color);
	else if(itempy<0) for(i=0;i<=(-itempy);i++)
			drawPoint((xpos-dtoi(m*i)),(ypos-i),color);
	*/
}

void drawLine3D(double x1, double y1, double z1, double x2, double y2, double z2, unsigned char color){
	drawLine2D(convert3Dto2DX(x1,y1,z1),convert3Dto2DY(x1,y1,z1),convert3Dto2DX(x2,y2,z2),convert3Dto2DY(x2,y2,z2),color);
}

void drawLine3Dp(point3d *p1, point3d *p2, unsigned char color){
	drawLine3D(p1->x,p1->y,p1->z,p2->x,p2->y,p2->z,color);
}


void fillTriangle2D(double x1, double y1, double x2, double y2, double x3, double y3, unsigned char color){
	double xup, xdown, xmiddle;
	double yup, ydown, ymiddle;
	double slopedownup, slopedownmiddle, slopemiddleup;
	double slopeleft, sloperight;
	double xstart, ystart, xend;
	double xstartprev, xendprev;
	double xleft, xright;
	//double aux;
	int verticalsize, horizontalsize;
	int i,j;

	xdown=x1; ydown=y1;
	xmiddle=x2; ymiddle=y2;
	xup=x3; yup=y3;

	/*
	yup=y1; xup=x1;
	if(y2>=yup) {yup=y2;xup=x2;}
	if(y3>=yup) {yup=y3;xup=x3;}
	ydown=y1; xdown=x1;
	if(y2<=ydown) {ydown=y2;xdown=x2;}
	if(y3<=ydown) {ydown=y3;xdown=x3;}	
	if((x1!=xup || y1!=yup) && (x1!=xdown || y1!=ydown)) {xmiddle=x1;ymiddle=y1;}
	if((x2!=xup || y2!=yup) && (x2!=xdown || y2!=ydown)) {xmiddle=x2;ymiddle=y2;}
	if((x3!=xup || y3!=yup) && (x3!=xdown || y3!=ydown)) {xmiddle=x3;ymiddle=y3;}
	*/

	xleft=x1;
	if(x2<xleft) xleft=x2;
	if(x3<xleft) xleft=x3;
	xright=x1;
	if(x2>xright) xright=x2;
	if(x3>xright) xright=x3;

	/*
	if(ydown==ymiddle && xmiddle<xdown) {aux=xdown;xdown=xmiddle;xmiddle=aux;}
	if(yup==ymiddle && xmiddle>xup) {aux=xup;xup=xmiddle;xmiddle=aux;}
	*/

	slopedownup=0.0;
	slopedownmiddle=0.0;
	slopemiddleup=0.0;
	if(yup!=ydown) slopedownup=(xup-xdown)/(yup-ydown);
	if(ymiddle!=ydown) slopedownmiddle=(xmiddle-xdown)/(ymiddle-ydown);
	if(xup!=xmiddle) slopemiddleup=(xup-xmiddle)/(yup-ymiddle);

	xstartprev=xdown;
	xendprev=xdown;
	if(ydown==ymiddle) xendprev=xmiddle;
	xstart=xstartprev;
	xend=xendprev;
	if(slopedownmiddle>slopedownup) {sloperight=slopedownmiddle;slopeleft=slopedownup;}
	else {sloperight=slopedownup;slopeleft=slopedownmiddle;}
	//if(xmiddle<xup || (xmiddle==xup && xmiddle<=xdown)) {slopeleft=slopedownmiddle;sloperight=slopedownup;}
	//else {slopeleft=slopedownup;sloperight=slopedownmiddle;}
	verticalsize=dtoi(ymiddle-ydown+1);
	for(i=1;i<verticalsize;i++){
		ystart=ydown+i;
		xstart=xstartprev+slopeleft*i;
		xend=xendprev+sloperight*i;
		//if(xstart<xleft) xstart=xleft;
		//if(xend>xright) xend=xright;
		//if(xstart<xleft || xend>xright) break;
		horizontalsize=dtoi(xend-xstart+1);
		for(j=0;j<horizontalsize;j++)
			drawPoint(dtoi(xstart+j),dtoi(ystart),color);
	}
	xstartprev=xstart;
	xendprev=xend;
	//if(moddtoi(xmiddle-xleft)<moddtoi(xmiddle-xright)) slopeleft=slopemiddleup;
	//else sloperight=slopemiddleup;
	if(slopemiddleup>slopedownup) {slopeleft=slopemiddleup;sloperight=slopedownup;}
	else {slopeleft=slopedownup;sloperight=slopemiddleup;}
	verticalsize=dtoi(yup-ymiddle+1);
	for(i=1;i<verticalsize;i++){
		ystart=ymiddle+i;
		xstart=xstartprev+slopeleft*i;
		xend=xendprev+sloperight*i;
		//if(xstart<xleft) xstart=xleft;
		//if(xend>xright) xend=xright;
		//if(xstart<xleft || xend>xright) break;
		horizontalsize=dtoi(xend-xstart+1);
		for(j=0;j<horizontalsize;j++)
			drawPoint(dtoi(xstart+j),dtoi(ystart),color);
	}

}


void fillTile2D(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, unsigned char color){
	double xup, xdown, xmiddle1, xmiddle2;
	double yup, ydown, ymiddle1, ymiddle2;
	double aux;

	yup=y1; xup=x1;
	if(y2>=yup) {yup=y2;xup=x2;}
	if(y3>=yup) {yup=y3;xup=x3;}
	if(y4>=yup) {yup=y4;xup=x4;}
	ydown=y1; xdown=x1;
	if(y2<=ydown) {ydown=y2;xdown=x2;}
	if(y3<=ydown) {ydown=y3;xdown=x3;}
	if(y4<=ydown) {ydown=y4;xdown=x4;}
	ymiddle1=height; xmiddle1=height;
	if(y1<=ymiddle1 && (x1!=xup || y1!=yup) && (x1!=xdown || y1!=ydown)) {xmiddle1=x1;ymiddle1=y1;}
	if(y2<=ymiddle1 && (x2!=xup || y2!=yup) && (x2!=xdown || y2!=ydown)) {xmiddle1=x2;ymiddle1=y2;}
	if(y3<=ymiddle1 && (x3!=xup || y3!=yup) && (x3!=xdown || y3!=ydown)) {xmiddle1=x3;ymiddle1=y3;}
	if(y4<=ymiddle1 && (x4!=xup || y4!=yup) && (x4!=xdown || y4!=ydown)) {xmiddle1=x4;ymiddle1=y4;}
	ymiddle2=0; xmiddle2=0;
	if(y1>=ymiddle2 && (x1!=xup || y1!=yup) && (x1!=xdown || y1!=ydown)) {xmiddle2=x1;ymiddle2=y1;}
	if(y2>=ymiddle2 && (x2!=xup || y2!=yup) && (x2!=xdown || y2!=ydown)) {xmiddle2=x2;ymiddle2=y2;}
	if(y3>=ymiddle2 && (x3!=xup || y3!=yup) && (x3!=xdown || y3!=ydown)) {xmiddle2=x3;ymiddle2=y3;}
	if(y4>=ymiddle2 && (x4!=xup || y4!=yup) && (x4!=xdown || y4!=ydown)) {xmiddle2=x4;ymiddle2=y4;}

	ydown=floor(ydown);
	ymiddle1=floor(ymiddle1);
	ymiddle2=floor(ymiddle2);
	yup=floor(yup);
	/*
	xdown=floor(xdown);
	xmiddle1=floor(xmiddle1);
	xmiddle2=floor(xmiddle2);
	xup=floor(xup);
	*/

	if(ydown==ymiddle1 && xmiddle1<xdown) {aux=xdown;xdown=xmiddle1;xmiddle1=aux;}
	if(yup==ymiddle2 && xmiddle2>xup) {aux=xup;xup=xmiddle2;xmiddle2=aux;}


	if( ((xmiddle1>xdown && xmiddle1>xup) && (xmiddle2>xdown && xmiddle2>xup))
		|| ((xmiddle1<xdown && xmiddle1<xup) && (xmiddle2<xdown && xmiddle2<xup)) ){
		fillTriangle2D(xdown,ydown,xmiddle1,ymiddle1,xmiddle2,ymiddle2,color);
		fillTriangle2D(xdown,ydown,xmiddle2,ymiddle2,xup,yup,color);
		//drawLine2D(xdown,ydown,xmiddle2,ymiddle2,white);
	} else {
		fillTriangle2D(xdown,ydown,xmiddle1,ymiddle1,xup,yup,color);
		fillTriangle2D(xdown,ydown,xmiddle2,ymiddle2,xup,yup,color);
		//drawLine2D(xdown,ydown,xup,yup,white);
	}

	/*
	double xup, xdown, xleft, xright;
	double yup, ydown, yleft, yright;
	double xmiddle1, xmiddle2, ymiddle1, ymiddle2;
	double slopeleft, sloperight;
	double xstart, ystart, xend;
	double xstartprev, xendprev;
	double aux;
	int verticalsize, horizontalsize;
	int i,j;

	yup=y1; xup=x1;
	if(y2>=yup) {yup=y2;xup=x2;}
	if(y3>=yup) {yup=y3;xup=x3;}
	if(y4>=yup) {yup=y4;xup=x4;}
	ydown=y1; xdown=x1;
	if(y2<=ydown) {ydown=y2;xdown=x2;}
	if(y3<=ydown) {ydown=y3;xdown=x3;}
	if(y4<=ydown) {ydown=y4;xdown=x4;}
	
	xright=0; yright=0;
	if(x1>=xright && (x1!=xup || y1!=yup) && (x1!=xdown || y1!=ydown)) {xright=x1;yright=y1;}
	if(x2>=xright && (x2!=xup || y2!=yup) && (x2!=xdown || y2!=ydown)) {xright=x2;yright=y2;}
	if(x3>=xright && (x3!=xup || y3!=yup) && (x3!=xdown || y3!=ydown)) {xright=x3;yright=y3;}
	if(x4>=xright && (x4!=xup || y4!=yup) && (x4!=xdown || y4!=ydown)) {xright=x4;yright=y4;}
	xleft=width; yleft=width;
	if(x1<=xleft && (x1!=xup || y1!=yup) && (x1!=xdown || y1!=ydown) && (x1!=xright || y1!=yright)) {xleft=x1;yleft=y1;}
	if(x2<=xleft && (x2!=xup || y2!=yup) && (x2!=xdown || y2!=ydown) && (x2!=xright || y2!=yright)) {xleft=x2;yleft=y2;}
	if(x3<=xleft && (x3!=xup || y3!=yup) && (x3!=xdown || y3!=ydown) && (x3!=xright || y3!=yright)) {xleft=x3;yleft=y3;}
	if(x4<=xleft && (x4!=xup || y4!=yup) && (x4!=xdown || y4!=ydown) && (x4!=xright || y4!=yright)) {xleft=x4;yleft=y4;}

	if(yright==yup && xup>xright) {aux=xup;xup=xright;xright=aux;}
	if(yright==ydown && xdown>xright) {aux=xdown;xdown=xright;xright=aux;}
	if(yleft==yup && xup<xleft) {aux=xup;xup=xleft;xleft=aux;}
	if(yleft==ydown && xdown<xleft) {aux=xdown;xdown=xleft;xleft=aux;}

	if(yright<yleft) {xmiddle1=xright;ymiddle1=yright;xmiddle2=xleft;ymiddle2=yleft;}
	else {xmiddle1=xleft;ymiddle1=yleft;xmiddle2=xright;ymiddle2=yright;}

	slopeleft=0;
	sloperight=0;
	xstartprev=xdown;
	xendprev=xdown;
	if(yleft==ydown && xleft<xdown) xstartprev=xleft;
	if(yright==ydown && xright>xdown) xendprev=xright;
	verticalsize=dtoi(ymiddle1-ydown+1);
	if(yleft!=ydown) slopeleft=(xleft-xdown)/(yleft-ydown);
	if(yright!=ydown) sloperight=(xright-xdown)/(yright-ydown);
	for(i=1;i<=verticalsize;i++){
		ystart=ydown+i;
		xstart=xstartprev+slopeleft*i;
		xend=xendprev+sloperight*i;
		horizontalsize=dtoi(xend-xstart);
		for(j=1;j<=horizontalsize;j++)
			drawPoint(dtoi(xstart+j),dtoi(ystart),color);
	}
	xstartprev=xstart;
	xendprev=xend;
	verticalsize=dtoi(ymiddle2-ymiddle1+1);
	if(yup!=yleft && ymiddle1==yleft) slopeleft=(xup-xleft)/(yup-yleft);
	if(yup!=yright && ymiddle1==yright) sloperight=(xup-xright)/(yup-yright);
	for(i=0;i<verticalsize;i++){
		ystart=ymiddle1+i;
		xstart=xstartprev+slopeleft*i;
		xend=xendprev+sloperight*i;
		horizontalsize=dtoi(xend-xstart);
		for(j=1;j<=horizontalsize;j++)
			drawPoint(dtoi(xstart+j),dtoi(ystart),color);
	}
	xstartprev=xstart;
	xendprev=xend;
	verticalsize=dtoi(yup-ymiddle2+1);
	if(yup!=yleft && ymiddle2==yleft) slopeleft=(xup-xleft)/(yup-yleft);
	if(yup!=yright && ymiddle2==yright) sloperight=(xup-xright)/(yup-yright);
	for(i=0;i<verticalsize;i++){
		ystart=ymiddle2+i;
		xstart=xstartprev+slopeleft*i;
		xend=xendprev+sloperight*i;
		horizontalsize=dtoi(xend-xstart);
		for(j=1;j<=horizontalsize;j++)
			drawPoint(dtoi(xstart+j),dtoi(ystart),color);
	}
	*/
}

void fillTile2Dp(point2d *p1, point2d *p2, point2d *p3, point2d *p4, unsigned char color){
	fillTile2D(p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,p4->x,p4->y,color);
}

void fillTile3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4, unsigned char color){
	fillTile2D(convert3Dto2DX(x1,y1,z1),convert3Dto2DY(x1,y1,z1),convert3Dto2DX(x2,y2,z2),convert3Dto2DY(x2,y2,z2),convert3Dto2DX(x3,y3,z3),convert3Dto2DY(x3,y3,z3),convert3Dto2DX(x4,y4,z4),convert3Dto2DY(x4,y4,z4),color);
}

void fillTile3Dp(point3d *p1, point3d *p2, point3d *p3, point3d *p4, unsigned char color){
	fillTile3D(p1->x,p1->y,p1->z,p2->x,p2->y,p2->z,p3->x,p3->y,p3->z,p4->x,p4->y,p4->z,color);
}

void fillColorTile3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4){
	double zval,zpercentage;
	unsigned char colorval,gradientcolor;
	//zval=(z1+z2+z3+z4)/4.0;
	zval=z1;
	if(z2>zval) zval=z2;
	if(z3>zval) zval=z3;
	if(z4>zval) zval=z4;
	//zpercentage=(zhigh-yval)/(zhigh-zlow);
	//redval=(unsigned char)dtoi(zpercentage*255.0);
	//toadd=((redval>127)?(255-redval):(redval));
	//gradientcolor=getColorFromPalette(255-redval+toadd,redval+toadd,0); // trocado porque +Z=-Y
	//gradientcolor=((redval>127)?(getColorFromPalette(255,2*(255-redval),0)):(getColorFromPalette(2*redval,255,0)));
	if(zval>0){
		zpercentage=(zval-0)/(zhigh-0); // red percentage
		colorval=(unsigned char)dtoi(zpercentage*255.0);
		gradientcolor=((colorval>127)?(getColorFromPalette(255,2*(255-colorval),0)):(getColorFromPalette(2*colorval,255,0)));
	}
	else{
		if(zlow==0) zpercentage=1.0;
		else zpercentage=(zval-0)/(zlow-0); // blue percentage
		colorval=(unsigned char)dtoi(zpercentage*255.0);
		gradientcolor=((colorval>127)?(getColorFromPalette(0,2*(255-colorval),255)):(getColorFromPalette(0,255,2*colorval)));
	}
	fillTile2D(convert3Dto2DX(x1,y1,z1),convert3Dto2DY(x1,y1,z1),convert3Dto2DX(x2,y2,z2),convert3Dto2DY(x2,y2,z2),convert3Dto2DX(x3,y3,z3),convert3Dto2DY(x3,y3,z3),convert3Dto2DX(x4,y4,z4),convert3Dto2DY(x4,y4,z4),gradientcolor);
}

void fillColorTile3Dp(point3d *p1, point3d *p2, point3d *p3, point3d *p4){
	fillColorTile3D(p1->x,p1->y,p1->z,p2->x,p2->y,p2->z,p3->x,p3->y,p3->z,p4->x,p4->y,p4->z);
}


void setDimensions(int w, int h){
	width=w;
	height=h;
}

void setOrigin(int x, int y){
	originx=x;
	originy=y;
	origin2d=newPoint2D((double)x,(double)y);
	origin3d=newPoint3D(0.0,0.0,0.0);
}

void setEndPoints(int xy, int yy){
	xendpointx=0+hmargin;
	xendpointy=xy;
	yendpointx=width-hmargin;
	yendpointy=yy;
	if((xendpointy+(yendpointy-originy))>(height-10)) yendpointy=(height-10)-(xendpointy-originy);
	zendpointx=originx;
	zendpointy=0+vmargin;
	xendpoint2d=newPoint2D((double)(xendpointx),(double)(xendpointy));
	yendpoint2d=newPoint2D((double)(yendpointx),(double)(yendpointy));
	zendpoint2d=newPoint2D((double)(zendpointx),(double)(zendpointy));
	vectorx=vector2D(origin2d,xendpoint2d);
	vectory=vector2D(origin2d,yendpoint2d);
	vectorz=vector2D(origin2d,zendpoint2d);
}

void setNumberOfPoints(int x, int y, int z){
	nx=x;
	ny=y;
	nz=z;
}

void calcAxesSizes(){
	xsize=dtoi(vectorSize2D(vectorx));
	ysize=dtoi(vectorSize2D(vectory));
	zsize=dtoi(vectorSize2D(vectorz));
}

void calcMargins(){
	hmargin=width/20;
	vmargin=height/20;
}

void calcUnits(){
	double xs,ys,zs;
	xunit=(xsize)/((double)(nx-1));
	yunit=(ysize)/((double)(ny-1));
	zunit=(zsize)/((double)(nz-1)); // linhas limitadoras contam
	xs=vectorSize2D(vectorx);
	ys=vectorSize2D(vectory);
	zs=vectorSize2D(vectorz);
	unitvectorx=newPoint2D(xunit*((vectorx->x)/xs),xunit*((vectorx->y)/xs));
	unitvectory=newPoint2D(yunit*((vectory->x)/ys),yunit*((vectory->y)/ys));
	unitvectorz=newPoint2D(zunit*((vectorz->x)/zs),zunit*((vectorz->y)/zs));
}

/*
double convert3dto2dx(double x, double y, double z){
	return ( originx + (-x)*xunit + (y)*yunit );
}

double convert3dto2dy(double x, double y, double z){
	return ( originy + (-z)*zunit + (x)*xunit*xm + (y)*yunit*ym );
}
*/

void drawAxes(){
	point2d *vleftup,*vrightup,*vclosedown,*vcloseup;
	vleftup=sumVector2D(xendpoint2d,vectorz);
	vrightup=sumVector2D(yendpoint2d,vectorz);
	vclosedown=sumVector2D(xendpoint2d,vectory);
	vcloseup=sumVector2D(vclosedown,vectorz);
	fillTile2Dp(vleftup,xendpoint2d,origin2d,zendpoint2d,lightgrey); // back
	fillTile2Dp(xendpoint2d,vclosedown,yendpoint2d,origin2d,lightgrey); // bottom
	fillTile2Dp(zendpoint2d,origin2d,yendpoint2d,vrightup,lightgrey); // right
	drawLine2Dp(origin2d,xendpoint2d,black);
	drawLine2Dp(origin2d,yendpoint2d,black);
	drawLine2Dp(origin2d,zendpoint2d,black);
	drawLine2Dp(xendpoint2d,vleftup,black);
	drawLine2Dp(yendpoint2d,vrightup,black);
	drawLine2Dp(xendpoint2d,vclosedown,black);
	drawLine2Dp(yendpoint2d,vclosedown,black);
	drawLine2Dp(vleftup,zendpoint2d,black);
	drawLine2Dp(vrightup,zendpoint2d,black);
	free(vleftup);
	free(vrightup);
	free(vclosedown);
	free(vcloseup);
}

void drawGrid(){
	int i;
	int nnx,nny,nnz;
	double xstep, ystep, zstep;
	nnx=nx;
	nny=ny;
	nnz=nz;
	if(nnx>10) nnx=10;
	if(nny>10) nny=10;
	if(nnz>10) nnz=10;
	xstep=((double)nx)/((double)nnx);
	ystep=((double)ny)/((double)nny);
	zstep=((double)nz)/((double)nnz);
	for(i=1;i<nnx;i++){
		drawLine3D(i*xstep,0,zlow,i*xstep,0,zhigh,grey);
		drawLine3D(i*xstep,0,zlow,i*xstep,ny,zlow,grey);
	}
	for(i=1;i<nny;i++){
		drawLine3D(0,i*ystep,zlow,0,i*ystep,zhigh,grey);
		drawLine3D(0,i*ystep,zlow,nx,i*ystep,zlow,grey);
	}
	for(i=1;i<(nnz-1);i++){
		drawLine3D(0,0,zlow+i*zstep,nx,0,zlow+i*zstep,grey);
		drawLine3D(0,0,zlow+i*zstep,0,ny,zlow+i*zstep,grey);
	}
}

// FALTA: linhas a tracejado
void drawLabelledGrid(int xstart){
	int i, number, xpos, ypos;
	int nnx,nny,nnz;
	int xmaxmarks, zmaxmarks;
	int lastx, firstz, xgap;
	int xstep, ystep, zstep;
	nnx=nx;
	nny=ny;
	nnz=nz;
	xstep=1;
	ystep=1;
	zstep=1;
	i=digitCount(xstart+nx);
	xmaxmarks=(originx-xendpointx)/(i*(5+2));
	while(1){
		if(nnx<=xmaxmarks) break;
		nnx=nnx/5;
		xstep=xstep*5;
		if(nnx<=xmaxmarks) break;
		nnx=nnx/2;
		xstep=xstep*2;
	}
	lastx=(xstart+nx-1);
	lastx=xstep*(lastx/xstep);
	xgap=(xstart+nx-1)-lastx;
	for(i=0;i<nnx;i++){ // numera�o do maior para o menor
		number=xgap+i*xstep;
		if(number!=0 && number!=(nx-1)){
			drawLine3D(number,0,zlow,number,0,zhigh,grey);
			drawLine3D(number,0,zlow,number,ny-1,zlow,grey);
		}
		xpos=dtoi(convert3Dto2DX((double)number,ny-1,zlow)+2);
		ypos=dtoi(convert3Dto2DY((double)number,ny-1,zlow)+(3+2));
		//number=nnx-(i+1)*xstep;
		number=lastx-i*xstep;
		drawNumberAtCenter(number,xpos,ypos);
	}
	for(i=0;i<nny;i++){ // numera�o do maior para o menor
		number=i*ystep;
		xpos=dtoi(convert3Dto2DX(nx-1,(double)number,zlow)-(3+2));
		ypos=dtoi(convert3Dto2DY(nx-1,(double)number,zlow)-3);
		number=(i+1)*ystep;
		drawNumberAtLeft(number,xpos,ypos);
		if(i!=0 && i!=(ny-1)){
			drawLine3D(0,i*ystep,zlow,0,i*ystep,zhigh,grey);
			drawLine3D(0,i*ystep,zlow,nx-1,i*ystep,zlow,grey);
		}
	}
	zmaxmarks=dtoi(zsize)/(6+2);
	while(1){
		if(nnz<=zmaxmarks) break;
		nnz=nnz/5;
		zstep=zstep*5;
		if(nnz<=zmaxmarks) break;
		nnz=nnz/2;
		zstep=zstep*2;
	}
	firstz=zstep*((dtoi(zlow)/zstep));
	for(i=0;i<=nnz;i++){
		number=firstz+i*zstep;
		xpos=xendpointx-2;
		ypos=dtoi(convert3Dto2DY(nx-1,0,(double)number))-3;
		if(number>dtoi(zlow) && number<=dtoi(zhigh)) drawNumberAtLeft(number,xpos,ypos);
		if(number>dtoi(zlow) && number<dtoi(zhigh)){
			drawLine3D(0,0,number,nx-1,0,number,grey);
			drawLine3D(0,0,number,0,ny-1,number,grey);
		}
	}
	drawText("EP",hmargin-(5+2),xendpointy-dtoi(zsize)-2*(6+2)); // ZZ
	//drawText("L",xendpointx+(yendpointx-originx)-4*(6+2),xendpointy+(yendpointy-(originy+dtoi(zlow*(unitvectorz->y))))-3); // YY
	//drawText("L",xendpointx-2*(5+2),xendpointy+2*(6+2)); // YY
	drawText("L",xendpointx-2*(5+2),xendpointy+2*(6+2)); // YY
	drawText("X",width-hmargin,yendpointy+2*(6+2)); // XX
}

void drawOverAxes(){
	point2d *vleftup,*vrightup,*vclosedown,*vcloseup;
	vleftup=sumVector2D(xendpoint2d,vectorz);
	vrightup=sumVector2D(yendpoint2d,vectorz);
	vclosedown=sumVector2D(xendpoint2d,vectory);
	vcloseup=sumVector2D(vclosedown,vectorz);
	drawLine2Dp(vleftup,vcloseup,grey);
	drawLine2Dp(vrightup,vcloseup,grey);
	drawLine2Dp(vclosedown,vcloseup,grey);
	free(vleftup);
	free(vrightup);
	free(vclosedown);
	free(vcloseup);
}

void testLines2D(){
	point2d *topleft,*topright,*bottomleft,*bottomright,*middleleft,*middleright,*middletop,*middlebottom;
	topleft=newPoint2D(0,0);
	topright=newPoint2D(width,0);
	bottomleft=newPoint2D(0,height);
	bottomright=newPoint2D(width,height);
	middleleft=newPoint2D(0,height/2);
	middleright=newPoint2D(width,height/2);
	middletop=newPoint2D(width/2,0);
	middlebottom=newPoint2D(width/2,height);
	drawLine2Dp(topright,bottomleft,green);
	drawLine2Dp(bottomright,topleft,red);
	//drawLine2Dp(bottomleft,topright,blue);
	//drawLine2Dp(topleft,bottomright,purple);
	drawLine2Dp(middleright,middleleft,black);
	drawLine2Dp(middlebottom,middletop,black);
}

void testGrid(){
	int i,j;
	point3d ***xpointslist,***ypointslist,***zpointslist;
	xpointslist=(point3d ***)malloc((nx+2)*sizeof(point3d **));
	ypointslist=(point3d ***)malloc((ny+2)*sizeof(point3d **));
	zpointslist=(point3d ***)malloc((nz+2)*sizeof(point3d **));
	for(i=0;i<=ny;i++){
		xpointslist[i]=(point3d **)malloc(nz*sizeof(point3d *));
		for(j=0;j<=nz;j++){
			xpointslist[i][j]=newPoint3D(0,i,j);
			if(i!=0 && j!=0) fillColorTile3Dp(xpointslist[i-1][j-1],xpointslist[i-1][j],xpointslist[i][j-1],xpointslist[i][j]);
			if(i!=0) drawLine3Dp(xpointslist[i-1][j],xpointslist[i][j],grey);
			if(j!=0) drawLine3Dp(xpointslist[i][j-1],xpointslist[i][j],grey);
			drawPoint3Dp(xpointslist[i][j],red);
		}
	}
	for(i=0;i<=nx;i++){
		ypointslist[i]=(point3d **)malloc(nz*sizeof(point3d *));
		for(j=0;j<=nz;j++){
			ypointslist[i][j]=newPoint3D(i,0,j);
			if(i!=0 && j!=0) fillColorTile3Dp(ypointslist[i-1][j-1],ypointslist[i-1][j],ypointslist[i][j-1],ypointslist[i][j]);
			if(i!=0) drawLine3Dp(ypointslist[i-1][j],ypointslist[i][j],grey);
			if(j!=0) drawLine3Dp(ypointslist[i][j-1],ypointslist[i][j],grey);
			drawPoint3Dp(ypointslist[i][j],green);
		}
	}
	for(i=0;i<=nx;i++){
		zpointslist[i]=(point3d **)malloc(ny*sizeof(point3d *));
		for(j=0;j<=ny;j++){
			zpointslist[i][j]=newPoint3D(i,j,0);
			if(i!=0 && j!=0) fillColorTile3Dp(zpointslist[i-1][j-1],zpointslist[i-1][j],zpointslist[i][j-1],zpointslist[i][j]);
			if(i!=0) drawLine3Dp(zpointslist[i-1][j],zpointslist[i][j],grey);
			if(j!=0) drawLine3Dp(zpointslist[i][j-1],zpointslist[i][j],grey);
			drawPoint3Dp(zpointslist[i][j],blue);
		}
	}
	drawOverAxes();
	/*
	for(i=0;i<nx;i++) free(xpointslist[i]);
	for(i=0;i<ny;i++) free(ypointslist[i]);
	for(i=0;i<nz;i++) free(zpointslist[i]);
	free(xpointslist);
	free(ypointslist);
	free(zpointslist);
	*/
}

void free3DVariables(){
	free(unitvectorx);
	free(unitvectory);
	free(unitvectorz);
	free(origin2d);
	free(origin3d);
}

void drawPlot3D(double **points, int xnp, int ynp, int xstart){
	int i,j;
	double *zline,*prevzline;
	double zmin, zmax;
	zmin=points[0][0];
	zmax=points[0][0];
	for(i=0;i<xnp;i++){
		zline=points[i];
		for(j=0;j<ynp;j++){
			if(zline[j]<zmin) zmin=zline[j];
			if(zline[j]>zmax) zmax=zline[j];
		}
	}
	if(zmax>=0) zmax=ceil(zmax);
	else zmax=-floor(-zmax);
	if(zmin>=0) zmin=floor(zmin);
	else zmin=-ceil(-zmin);
	setNumberOfPoints(xnp,ynp,dtoi(zmax-zmin)+1);
	zlow=zmin;
	zhigh=zmax;
	free3DVariables();
	calcUnits();
	setOrigin(originx,originy-dtoi(zmin*(unitvectorz->y))); // translac�o da origem
	//drawGrid();
	drawLabelledGrid(xstart);
	for(i=0;i<xnp;i++){
		zline=points[i];
		for(j=0;j<ynp;j++){
			if(i!=0 && j!=0) fillColorTile3D(i,j,zline[j],(i-1),j,prevzline[j],i,(j-1),zline[(j-1)],(i-1),(j-1),prevzline[(j-1)]);
			if(i!=0) drawLine3D(i,j,zline[j],(i-1),j,prevzline[j],black);
			if(j!=0) drawLine3D(i,j,zline[j],i,(j-1),zline[(j-1)],black);
		}
		prevzline=zline;
	}
	free3DVariables();
}

void testPlot3D(){
	int i,j;
	int xnp,ynp;
	double **points,*zpoints;
	double xsquared,ysquared;
	xnp=30;
	ynp=30;
	points=(double **)malloc(xnp*sizeof(double *));
	for(i=0;i<xnp;i++){
		points[i]=(double *)malloc(ynp*sizeof(double));
		zpoints=points[i];
		xsquared=(i-xnp/2)*(i-xnp/2);
		for(j=0;j<ynp;j++){
			ysquared=(j-ynp/2)*(j-ynp/2);
			zpoints[j]=xsquared+ysquared;
			//if((xsquared+ysquared)<25) zpoints[j]=sqrt(25.0-(xsquared+ysquared)); else zpoints[j]=0.0;
			//zpoints[j]=sqrt((i-xnp/2)*(i-xnp/2)+(j-ynp/2)*(j-ynp/2));
			//zpoints[j]=sin(i-xnp/2)+sin(j-ynp/2);
			//if((xsquared+ysquared)!=0) zpoints[j]=cos(sqrt(xsquared+ysquared)-0.04*3.1415)/(xsquared+ysquared); else zpoints[j]=0.0;
		}
	}
	drawPlot3D(points,xnp,ynp,0);
	drawOverAxes();
}

void initialize3DGraphics(int w, int h){
	setDimensions(w,h);
	calcMargins();
	setOrigin((8*width)/10,(5*height)/10);
	setEndPoints((6*height)/10,(9*height)/10);
	setNumberOfPoints(10,10,10);
	calcAxesSizes();
	calcUnits();
	initializeBitmap(width,height);
	setupColors();
	drawAxes();
}

void draw3DLabel(int maxi, int maxl){
	char *label;
	int n;
	n=digitCount(maxi);
	label=(char *)calloc((n+13),sizeof(char));
	sprintf(label,"MAX EP: X = %d",maxi);
	drawText(label,width/10,height/20);
	free(label);
	n=digitCount(maxl);
	label=(char *)calloc((n+13),sizeof(char));
	sprintf(label,"        L = %d",maxl);
	drawText(label,width/10,height/20+8);
	free(label);
}

// FALTA: drawDashed2DLine ; draw3DLabel ; ...
/*
int main(int argc, char *argv[]){
	initialize3DGraphics(640,480);
	//testLines2D();
	//testGrid();
	//Plot3DInfo();
	testPlot3D();
	//fillTile2D(400.0,400.0,200.0,300.0,100.0,100.0,300.0,200.0,red); // all diagonals
	//fillTile2D(400.0,300.0,200.0,300.0,100.0,100.0,300.0,100.0,blue); // flat top and bottom
	//fillTile2D(400.0,302.0,200.0,300.0,100.0,100.0,300.0,102.0,blue); // flat top and bottom
	//fillTile2D(400.0,400.0,100.0,300.0,100.0,100.0,400.0,200.0,red); // flat left and right
	//fillTile2D(200.0,300.0,100.0,400.0,100.0,200.0,200.0,100.0,blue); // flat left and right
	//fillTile2D(200.0,200.0,100.0,200.0,100.0,100.0,200.0,100.0,blue); // square
	//fillTile2D(200.0,100.0,100.0,400.0,300.0,300.0,400.0,200.0,red); // trapezio
	//fillTile2D(100.0,100.0,100.0,400.0,200.0,300.0,200.0,200.0,red); // trapezio
	//fillTile2D(200.0,105.0,100.0,395.0,300.0,400.0,400.0,100.0,red); // trapezio
	//fillTile2D(100.0,100.0,100.5,200.0,200.5,200.5,200.0,100.5,red); // all diagonals
	saveBitmap("3dplot.bmp");
	//freeBitmap();
	//system("pause");
}
*/

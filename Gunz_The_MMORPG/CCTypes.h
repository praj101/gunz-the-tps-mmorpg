#pragma once
#include "CCVector3.h"
/* 
	CCTypes.h
		This header file contains multiple macros, defines, and structs that will
		be used throughout the program. Requires source file CCTypes.cpp


	STRUCTURES
	--------------------------------
		- sPoint
			\_	Contains locations in space, capable of being scaled and
				uses a default resolution of 640x480

		- sRect
			\_	Contains rectangle info such as width, height, and positions in
				2D space. Used for most GUI elements. Can be translated,
				scaled, Enlarged, and can be used to test for points inside
				and intersection.

		- sSize
			\_	Contains width and height data representing size in 2D.

		- sColor
			\_	Contains collor information and conversion data. Allows for extraction
				of color and presentation of color via unsigned long int values.
*/
#define MINT_ARGB(a,r,g,b)	((((unsigned long int)a)&0xFF)<<24) | ((((unsigned long int)r)&0xFF)<<16)	| ((((unsigned long int)g)&0xFF)<<8) | (((unsigned long int)b)&0xFF)
#define MINT_RGB(r,g,b)		((((unsigned long int)r)&0xFF)<<16) | ((((unsigned long int)g)&0xFF)<<8)	| (((unsigned long int)b)&0xFF)
#define uchar unsigned char
#define ulong unsigned long
#define uint  unsigned int

typedef CCVector3			CCVector;

//Location
struct sPoint{
	int x;
	int y;
public:
	sPoint(){};
	sPoint(int x, int y){
		sPoint::x = x;
		sPoint::y = y;
	}

	void Scale(float x, float y);
	void ScaleRes();		// 640x480 - Resolution in accordance with reference to the current scaling
	void TranslateRes();	// 640x480 - The distance from the center to fit the current resolution to move
};

struct sRect{
	int x;		//Location
	int y;		//Location
	int w;		//Size
	int h;		//Size
public:
	sRect(){};
	sRect(int x, int y, int w, int h){
		Set(x,y,w,h);
	}
	bool InPoint(sPoint &p){
		if(	p.x>=x && p.x<=x+w &&
			p.y>=y && p.y<=y+h)
			return true;
		return false;
	}

	void Set(int x, int y, int w, int h){
		sRect::x=x,sRect::y=y;
		sRect::w=w,sRect::h=h;
	}

	void ScalePos(float x, float y);	//Starting point for the scaling
	void ScaleArea(float x, float y);	//Starting and scaling for the width
	void ScalePosRes();		// 640x480 - Resolution in accordance with reference to the current scaling
	void ScaleAreaRes();	// 640x480 - Resolution in accordance with reference to the current scaling
	void TranslateRes();	// 640x480 - The distance from the center to fit the current resolution to move

	void Enlarge(int w);	//W, Down, Left, Right and raise the chunk size
	void Offset(int x, int y);		//Position move

	bool Intersect(sRect* pIntersect, const sRect& r);	//Common area of two squares
};
//Dimensions
struct sSize{
	int w;
	int h;
public:
	sSize(){};
	sSize(int w, int h){
		sSize::w=w;
		sSize::h=h;
	}
};

struct sColor{
public:
	uchar r;	// Alpha
	uchar g;	// Green
	uchar b;	// Blue
	uchar a;	// alpha
public:
	sColor(){
		r=g=b=a=0;
	};
	sColor(uchar r, uchar g, uchar b, uchar a){
		sColor::r=r;
		sColor::g=g;
		sColor::b=b;
		sColor::a=a;
	};
	sColor(uchar r, uchar g, uchar b){
		sColor::r=r;
		sColor::g=g;
		sColor::b=b;
		sColor::a=0;
	};
	sColor(ulong int argb){
		a = uchar((argb & 0xFF000000)>>24);
		r = uchar((argb & 0x00FF0000)>>16);
		g = uchar((argb & 0x0000FF00)>>8);
		b = uchar((argb & 0x000000FF));
	};

	ulong int GetARGB(){
		return MINT_ARGB(a,r,g,b);
	}
	ulong int GetRGB(){
		return MINT_RGB(r,g,b);
	}
};
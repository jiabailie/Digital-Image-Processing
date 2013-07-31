#ifndef _CALCULATETEMPLATESMOMENTS_H_
#define _CALCULATETEMPLATESMOMENTS_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <vector>
#include <map>
using namespace std;

typedef _int64		   LL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

const int	FILEHEADERSIZE		= 14;
const int	INFOHEADERSIZE		= 40;
const int	PathLength			= 400;
const int	NumLength			= 10;
const int	MOMENTS_LENGTH		= 6;
const int	BMPTYPE				= 0;
const int	TXTTYPE				= 1;
const int	IMGPERCHAR			= 1024;
const int	TEMPLATE_COUNT		= 36;
const int	p_value				= 127;
const BYTE	c_BLACK				= 0;
const BYTE	c_WHITE				= 255;

enum FILE_ACCESS_STATUS
{
	SUCCESS						= 0,
	NOT_EXIST					= 1
};

struct RGBQUAD {
	BYTE    rgbBlue;			
	BYTE    rgbGreen;				
	BYTE    rgbRed;					
	BYTE    rgbReserved;			
};

struct FILEHEADER {
	WORD    bfType;					
	DWORD   bfSize;					
	WORD    bfReserved1;			
	WORD    bfReserved2;			
	DWORD   bfOffBits;				
};

struct INFOHEADER{
	DWORD      biSize;				
	DWORD      biWidth;				
	DWORD      biHeight;			
	WORD       biPlanes;			
	WORD       biBitCount;			
	DWORD      biCompression;		
	DWORD      biSizeImage;			
	DWORD      biXPelsPerMeter;		
	DWORD      biYPelsPerMeter;		
	DWORD      biClrUsed;			
	DWORD      biClrImportant;		
};

struct bmpImage {
	INFOHEADER infoHeader;	
	FILEHEADER fileHeader;	
	RGBQUAD	   rgbQuad;		
};

typedef RGBQUAD*	BITMAPQUAD;
typedef FILEHEADER* BITMAPFILEHEADER;	
typedef INFOHEADER* BITMAPINFOHEADER;	
typedef bmpImage*	BMPIMAGE;

typedef struct fetch {
	int w;			
	int h;			
	int bitCount;	
	int lenLine;	
	long bmpSize;	
	BYTE * data;	

	fetch() : data(0) {}
	~fetch()
	{
		if(NULL == data)
		{
			free(data);
		}
	}
} infoFetch, *BMPINFO;

typedef struct point {
	int h;
	int w;
	point() : h(0), w(0) {}
	void copyPoint(int _h, int _w) {
		h = _h;
		w = _w;
	}
	point(int _h, int _w) {
		copyPoint(_h, _w);
	}
	point(const point& _point) {
		copyPoint(_point.h, _point.w);
	}
	point& operator=(const point& _point)
	{
		copyPoint(_point.h, _point.w);
		return *this;
	}
} Point;

template <class T>
inline T INDEX(T X, T Y, T Width)
{
	return (X * Width + Y) * 3;
}

template <class T>
inline T iABS(T X)
{
	return X > 0 ? X : -X;
}

#endif
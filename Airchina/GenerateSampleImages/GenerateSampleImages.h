#ifndef _GENERATE_SAMPLE_IMAGE_H_
#define _GENERATE_SAMPLE_IMAGE_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <vector>
#include <time.h>

using namespace std;

#define LOG_FILE				"..\\log\\log.txt"
#define SAMPLE_INPUT_FILE		"..\\samples\\test(0).bmp"
#define SAMPLE_OUTPUT_FILE		"..\\samples\\result(0).bmp"
#define SAMPLE_CHAR_OUTPUT		"..\\data\\s"
#define SAMPLE_CHAR_DEBUG		"..\\data\\t.bmp"

typedef _int64			LL;
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

const int REMOVE_LONELY_PIXEL	= 1;	// the operation times of remove lonely pixels
const int IMAGE_GROW_TIMES		= 6;	// execute how many times of image growing
const int IMAGE_GROW_DIMES		= 4;	// the dimensions of image growing directions
const int MAX_RANGE				= 20;	// the max range to search for a same pixel
const int CHARACTER_NUM			= 4;	// the number of characters in one image
const int PIXELNUM				= 256;
const int FILEHEADERSIZE		= 14;
const int INFOHEADERSIZE		= 40;
const int BMPHEADERSIZE			= 54 ;
const int SAMPLE_AMOUNT			= 8;
const int ERROR					= -1;
const int PathLength			= 400;
const int NumLength				= 50;
const int BLACK					= 0;
const int WHITE					= 255;
const int MIN_WIDTH				= 20;	// the approximate width of each character
const int TEMPLATE_COUNT		= 36;
const int MOMENTS_LENGTH		= 6;
const bool BOOL_WHITE			= true;
const double PI					= 3.1415926;

const BYTE c_BLACK		= 0;
const BYTE c_WHITE		= 255;

const int p_value		= 127;

const int DT_CHANGE_WHITE = 0; // not change white
const int DO_CHANGE_WHITE = 1; // change white

const long DT_CHANGE_WHITE_TIMES = 0; // the times of procession do not change white
const long DO_CHANGE_WHITE_TIMES = 1; // the times of procession do change white

enum FILE_ACCESS_STATUS
{
	SUCCESS = 0,
	NOT_EXIST = 1
};

const int BMPTYPE = 0;
const int TXTTYPE = 1;

template<class T>
inline T iABS(T X) { return X > 0 ? X : -X; }

template<class T>
inline T imax(T X, T Y) { return X > Y ? X : Y; }

template<class T>
inline T imin(T X, T Y) { return X < Y ? X : Y; }

template<class T>
inline T INDEX(T X, T Y, T Width) { return (X * Width + Y) * 3; }

struct RGBQUAD
{
	BYTE    rgbBlue;		
	BYTE    rgbGreen;		
	BYTE    rgbRed;			
	BYTE    rgbReserved;	
};

struct FILEHEADER
{
	WORD    bfType;		
	DWORD   bfSize;		
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;	
};

struct INFOHEADER
{
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

struct bmpImage
{
	INFOHEADER infoHeader;	
	FILEHEADER fileHeader;	
	RGBQUAD	   rgbQuad;		
};

struct infoFetch
{
	int w;			
	int h;			
	int bitCount;	
	int lenLine;	
	long bmpSize;	
	BYTE * data;	

	infoFetch() : data(0) {}
	~infoFetch()
	{
		if(NULL == data)
		{
			free(data);
		}
	}
};

struct Point
{
	int h;
	int w;
	Point() : h(0), w(0) {}
	void copyPoint(int _h, int _w)
	{
		h = _h;
		w = _w;
	}

	Point(int _h, int _w)
	{
		copyPoint(_h, _w);
	}

	Point(const Point& _point)
	{
		copyPoint(_point.h, _point.w);
	}

	Point& operator=(const Point& _point)
	{
		copyPoint(_point.h, _point.w);
		return *this;
	}
};

typedef RGBQUAD*		BITMAPQUAD;
typedef FILEHEADER*		BITMAPFILEHEADER;
typedef INFOHEADER*		BITMAPINFOHEADER;
typedef bmpImage*		BMPIMAGE;
typedef infoFetch*		BMPINFO;


#endif
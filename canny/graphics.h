#pragma once
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define PROCESS_FILE_OPEN_PATH  "..\\bmp\\test.bmp"
#define PROCESS_FILE_SAVE_PATH  "..\\bmp\\"
#define PROCESS_REMOVEBLA_PATH  "..\\bmp\\removeblackpixels.bmp"
#define PROCESS_TEXT_SAVE_PATH  "..\\data\\"

const int REMOVE_TIMES		= 1;
const int MAX_RANGE		= 250;
const int PIXELNUM		= 256;
const int FILEHEADERSIZE	= 14;
const int INFOHEADERSIZE	= 40;
const int BMPHEADERSIZE		= 54 ;
const int ERROR			= -1;
const int GAUSSIAN		= 25;
const int Filter_Num		= 8;
const int SobelX		= 4;
const int SobelY		= 5;
const int PathLength		= 200;
const int NumLength		= 10;
const int BLACK			= 0;
const int WHITE			= 255;
const bool BOOL_WHITE		= true;
const double PI			= 3.1415926;
const double gaussian[GAUSSIAN] = 
				{2,  4,  5,  4,  2,
				 4,  9, 12,  9,  4,
				 5, 12, 15, 12,  5,
				 4,  9, 12,  9,  4,
				 2,  4,  5,  4,  2};

const int DIREC_ARRAY_LEN	= 4;
const int DIRECTIONS[4]		= {-45, 0, 45, 90};

const int MASK[11]		= {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

enum canny_direction
{
		CANNY_HORIZONTAL	= 0, // 90
		CANNY_VERTICAL		= 1, // 0
		CANNY_LEFT_DIAGONAL	= 2, // 45
		CANNY_RIGHT_DIAGONAL	= 3  // -45
};

// file type
const int BMPTYPE = 0;
const int TXTTYPE = 1;

template <class T>
inline T iABS(T X)
{
		return X > 0 ? X : -X;
}

template <class T>
inline T imax(T X, T Y)
{
		return X > Y ? X : Y;
}

template <class T>
inline T imin(T X, T Y)
{
		return X < Y ? X : Y;
}

// get the angle through the value of tan
inline double getAngle(int gradientX, int gradientY)
{
		return 180 * atan(gradientY * 1.0 / gradientX) / PI;
}

// round the pixel value
// if value > 255, then set value = 255
// if value < 0,   then set value = 0
inline int roundPixelValue(int value)
{
		return value > 255 ? 255 : (value < 0 ? 0 : value);
}

// calculate the index value of point(x,y)
// consider the w by h image a one dimension array.
template <class T>
inline T INDEX(T X, T Y, T Width)
{
		return (X * Width + Y) * 3;
}

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

typedef struct tagRGBQUAD {
	BYTE	rgbBlue;		// value of blue
	BYTE	rgbGreen;		// value of green
	BYTE	rgbRed;			// value of red
	BYTE	rgbReserved;		// reservation
} RGBQUAD, *BITMAPQUAD;

typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;			// type of bitmap, should be "BM"
	DWORD	bfSize;			// the size of this bitmap, counted by byte
	WORD	bfReserved1;		// reservation segment, should be 0.
	WORD	bfReserved2;		// reservation segment, should be 0.
	DWORD	bfOffBits;		// the start position of bitmap data, using the offset to file head, counted by byte.
} FILEHEADER, *BITMAPFILEHEADER;// the data struct takes up 14 bytes.

typedef struct tagBITMAPINFOHEADER{
	DWORD	biSize;			// how many bytes this struct takes.
	DWORD	biWidth;		// the width of this bitmap, counted by pixels.
	DWORD	biHeight;		// the height of this bitmap, counted also by pixels.
	WORD	biPlanes;		// this segment should be 1
	WORD	biBitCount;		// how many bits each pixels needs, should be one of {1, 4, 8, 24}
	DWORD	biCompression;		// bitmap compressed type, should be 0(not compressed), 1(BI_RLE8), 2(BI_RLE4)
	DWORD	biSizeImage;		// the size of this bitmap file, counted by bytes.
	DWORD	biXPelsPerMeter;	// horizontal resolution, pixels per meter
	DWORD	biYPelsPerMeter;	// vertical resolution, pixels per meter
	DWORD	biClrUsed;		// how many difference colors this image used.
	DWORD	biClrImportant;		// the amount of important colors during drawing this image
} INFOHEADER, *BITMAPINFOHEADER;// this data struct takes up 40 bytes.

typedef struct image {
	INFOHEADER	infoHeader;	// bitmap info header
	FILEHEADER	fileHeader;	// bitmap file header
	RGBQUAD		rgbQuad;
} bmpImage, *BMPIMAGE;

typedef struct fetch {
	int w;				// image width
	int h;				// image height
	int bitCount;			// bitCount value
	int lenLine;			// how many bytes per line
	long bmpSize;			// image data length
	BYTE * data;			// image data
} infoFetch, *BMPINFO;

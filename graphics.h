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

const int REMOVE_TIMES  		= 1;
const int MAX_RANGE				= 250;
const int PIXELNUM				= 256;
const int FILEHEADERSIZE		= 14;
const int INFOHEADERSIZE		= 40;
const int BMPHEADERSIZE			= 54 ;
const int ERROR					= -1;
const int GAUSSIAN				= 25;
const int Filter_Num			= 8;
const int SobelX				= 4;
const int SobelY				= 5;
const int PathLength			= 200;
const int NumLength				= 10;
const int BLACK					= 0;
const int WHITE					= 255;
const bool BOOL_WHITE			= true;
const double PI					= 3.1415926;
const double gaussian[GAUSSIAN] = 
                            {2,  4,  5,  4,  2,
	                         4,  9, 12,  9,  4,
					         5, 12, 15, 12,  5,
					         4,  9, 12,  9,  4,
					         2,  4,  5,  4,  2};

const int DIREC_ARRAY_LEN = 4;
const int DIRECTIONS[4] = {-45, 0, 45, 90};

const int MASK[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

enum canny_direction
{
	CANNY_HORIZONTAL     = 0, // 90
	CANNY_VERTICAL       = 1, // 0
	CANNY_LEFT_DIAGONAL  = 2, // 45
	CANNY_RIGHT_DIAGONAL = 3  // -45
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

//计算点(x,y)在宽为w,高为h的图像的数据数组中的索引值
template <class T>
inline T INDEX(T X, T Y, T Width)
{
	return (X * Width + Y) * 3;
}

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

typedef struct tagRGBQUAD {
	BYTE    rgbBlue;				//蓝色的亮度
	BYTE    rgbGreen;				//绿色的亮度
	BYTE    rgbRed;					//红色的亮度
	BYTE    rgbReserved;			//保留，必须为0
} RGBQUAD, *BITMAPQUAD;

typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;					//位图文件的类型，必须为"BM"
	DWORD   bfSize;					//位图文件的大小，以字节为单位
	WORD    bfReserved1;			//位图文件保留字，必须为0
	WORD    bfReserved2;			//位图文件保留字，必须为0
	DWORD   bfOffBits;				//位图数据的起始位置，以相对于位图文件头的偏移量表示，以字节为单位
} FILEHEADER, *BITMAPFILEHEADER;	//该结构占据14个字节

typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;				//本结构所占用的字节数
	DWORD      biWidth;				//位图的宽度，以像素为单位
	DWORD      biHeight;			//位图的高度，以像素为单位
	WORD       biPlanes;			//目标设备的平面数不清，必须为1
	WORD       biBitCount;			//每个像素所需的位数，必须是1(双色)，4(16色)，8(256色)或24(真彩色)之一
	DWORD      biCompression;		//位图压缩类型，必须是0(不压缩)，1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
	DWORD      biSizeImage;			//位图的大小，以字节为单位
	DWORD      biXPelsPerMeter;		//位图水平分辨率，每米像素数
	DWORD      biYPelsPerMeter;		//位图垂直分辨率，每米像素数
	DWORD      biClrUsed;			//位图实际使用的颜色表中的颜色数
	DWORD      biClrImportant;		//位图显示过程中重要的颜色数
} INFOHEADER, *BITMAPINFOHEADER;	//该结构占据40个字节

typedef struct image {
	INFOHEADER infoHeader;	//位图信息头
	FILEHEADER fileHeader;	//位图文件头
	RGBQUAD	   rgbQuad;		//位图调色板
} bmpImage, *BMPIMAGE;

typedef struct fetch {
	int w;			//图片宽度
	int h;			//图片高度
	int bitCount;	//bitCount值
	int lenLine;	//图片行占字节数
	long bmpSize;	//图片数据长度
	BYTE * data;	//图像数据
} infoFetch, *BMPINFO;

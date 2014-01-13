/**   
* Title:     Crack the AirChina.com verify code
* Author:    Ruiguo Yang (spotlightyrg@gmail.com)
* Date:	     July 13th, 2013
*/ 

// BaseCrackVerifyCode.h
#ifndef _BASECRACKVERIFY_H_
#define _BASECRACKVERIFY_H_

#define DEBUG_ERROR
#undef  DEBUG_ERROR

#define DEBUG_FIND_PROPER_DISTANCE
#undef	DEBUG_FIND_PROPER_DISTANCE

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <strstream>
#include <map>
using namespace std;

#ifdef CRACKVERIFYCODEDLL_EXPORTS
#define CRACKVERIFYCODEDLL_API __declspec(dllexport) 
#else
#define CRACKVERIFYCODEDLL_API __declspec(dllimport) 
#endif

#define _USE_BAT_FILE_
#undef  _USE_BAT_FILE_

#define PROCESS_FILE_OPEN_PATH  "..\\bmp\\test.bmp"
#define PROCESS_FILE_SAVE_PATH  "..\\bmp\\"
#define PROCESS_REMOVEBLA_PATH  "..\\bmp\\remove_pixels"
#define PROCESS_TEXT_SAVE_PATH  "..\\data\\"
#define BMP_TEMPLATE_SAVE_PATH  "E:\\Projects\\CrackVerifyCode\\GenerateVerifyCode\\data\\1.bmp"

//**************modify this for correctness test*********************//	
#ifdef	_USE_BAT_FILE_
#define TEMPLATE_SAVE_PATH      "..\\..\\data\\moments.txt" 
#define SAVE_PROPER_DISTANCE    "..\\..\\data\\distance.txt"
#elif
#define TEMPLATE_SAVE_PATH      "..\\data\\moments.txt"
#define SAVE_PROPER_DISTANCE    "..\\data\\distance.txt"
#endif
//*******************************************************************//

typedef _int64         LL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

const int FILE_OPEN_SUCCESS     = 0;
const int FILE_NOT_EXIST        = -1;
const int FILE_WRITE_ERROR      = -2;
const int REMOVE_LONELY_PIXEL   = 1;  // the operation times of remove lonely pixels
const int IMAGE_GROW_TIMES      = 6;  // execute how many times of image growing
const int IMAGE_GROW_DIMES      = 4;  // the dimensions of image growing directions
const int MAX_RANGE             = 20; // the max range to search for a same pixel
const int CHARACTER_NUM         = 4;  // the number of characters in one image
const int PIXELNUM              = 256;
const int FILEHEADERSIZE        = 14;
const int INFOHEADERSIZE        = 40;
const int BMPHEADERSIZE	        = 54;
const int Filter_Num            = 8;
const int SobelX                = 4;
const int SobelY                = 5;
const int PathLength            = 400;
const int LineLength            = 1000;
const int NumLength             = 10;
const int BLACK                 = 0;
const int WHITE	                = 255;
const int MIN_WIDTH             = 20;  // the approximate width of each character
const int TEMPLATE_COUNT        = 36;
const int MOMENTS_LENGTH        = 6;
const bool BOOL_WHITE           = true;
const double MIN_NUMBER         = 0.1e-16;
const double PROPER_DISTANCE    = 5000;
const double PI                 = 3.1415926;

const int p_value               = 127;

const BYTE c_BLACK              = 0;
const BYTE c_WHITE              = 255;

const int DT_CHANGE_WHITE       = 0;  // not change white
const int DO_CHANGE_WHITE       = 1;  // change white

const long DT_CHANGE_WHITE_TIMES = 0; // the times of procession do not change white
const long DO_CHANGE_WHITE_TIMES = 1; // the times of procession do change white

const int DIREC_ARRAY_LEN        = 4;

const int MASK[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

const int FILE_ACCESS_STATUS_SUCCESS    = 0;
const int FILE_ACCESS_STATUS_NOT_EXIST  = 1;

// file type
const int BMPTYPE                = 0;
const int TXTTYPE                = 1;

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

template <class T>
inline T INDEX(T X, T Y, T Width)
{
	return (X * Width + Y) * 3;
}

namespace CrackVerifyCode
{
	class CRACKVERIFYCODEDLL_API crackVerifyCode
	{
	public:
		void CrackVerifyCode(const char* openFile, BYTE ret[CHARACTER_NUM]);
	};

	struct RGBQUAD {
		BYTE rgbBlue;	
		BYTE rgbGreen;	
		BYTE rgbRed;		
		BYTE rgbReserved;
	};
	struct FILEHEADER {
		WORD  bfType;		
		DWORD bfSize;		
		WORD  bfReserved1;
		WORD  bfReserved2;
		DWORD bfOffBits;	
	};
	struct INFOHEADER {
		DWORD biSize;			
		DWORD biWidth;			
		DWORD biHeight;		
		WORD  biPlanes;		
		WORD  biBitCount;		
		DWORD biCompression;	
		DWORD biSizeImage;		
		DWORD biXPelsPerMeter;	
		DWORD biYPelsPerMeter;	
		DWORD biClrUsed;		
		DWORD biClrImportant;	
	};
	struct bmpImage {
		INFOHEADER infoHeader;	
		FILEHEADER fileHeader;	
		RGBQUAD	   rgbQuad;		
	};
	struct infoFetch {
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
	struct Point {
		int h;
		int w;
		Point() : h(0), w(0) {}
		void copyPoint(int _h, int _w) {
			h = _h;
			w = _w;
		}
		Point(int _h, int _w) {
			copyPoint(_h, _w);
		}
		Point(const Point& _point) {
			copyPoint(_point.h, _point.w);
		}
		Point& operator=(const Point& _point)
		{
			copyPoint(_point.h, _point.w);
			return *this;
		}
	};

	struct charTemplate
	{
		int width;
		int height;
		BYTE name;
		BYTE* data;
		double moments[MOMENTS_LENGTH];
		charTemplate() : data(0) {}
		void copyValue(int _w, int _h, BYTE _name, BYTE* _data, const double _moments[MOMENTS_LENGTH])
		{
			width = _w;
			height = _h;
			name = _name;
			data = _data;
			for(int i = 0; i < MOMENTS_LENGTH; moments[i] = _moments[i], i++);
		}
		charTemplate(int _w, int _h, BYTE _name, BYTE* _data, double _moments[MOMENTS_LENGTH])
		{
			copyValue(_w, _h, _name, _data, _moments);
		}
		charTemplate(const charTemplate& _chartemplate)
		{
			copyValue(_chartemplate.height, _chartemplate.width, _chartemplate.name, _chartemplate.data, _chartemplate.moments);
		}
		charTemplate& operator=(const charTemplate& _chartemplate)
		{
			copyValue(_chartemplate.height, _chartemplate.width, _chartemplate.name, _chartemplate.data, _chartemplate.moments);
			return *this;
		}
		~charTemplate()
		{
			if(NULL == data)
			{
				free(data);
			}
		}
	};

	typedef RGBQUAD*	BITMAPQUAD;
	typedef FILEHEADER* 	BITMAPFILEHEADER;
	typedef INFOHEADER*	BITMAPINFOHEADER;
	typedef bmpImage*	BMPIMAGE;
	typedef infoFetch*	BMPINFO;

	class graphics
	{
	public:
		int Open(const char* path, BMPIMAGE image, BMPINFO info);
		
		void Save(char* path, BMPIMAGE image, BMPINFO info);
		
		void RemovePixels(const char* openFile, BYTE ret[CHARACTER_NUM]);
	};

	class common
	{
	public:
		static void reverseArray(int, int, char*);
	
		static void changeInt2CharArray(int, char*&);
	
		static void findNonZeroBits(int, bool*&);
	
		static void replaceFilePathForTemplate(char, char[]);	
		
		static void spellSaveFilePath1(int, int, char[]); // fileType: 0 bmp, 1 txt
	
		static void spellSaveFilePath2(int, vector<int>, char[]);
	
		static void spellSaveFilePathTail(int, char[]);
	
		static void singlizeimage(int, int, BYTE*, BYTE*);
	
		static LL calPow(LL base, LL exponent);

		static void transferData(const char* data, char& name, double moments[MOMENTS_LENGTH]);
		
		static void binarization(long bmpSize, BYTE* data);

		static double calSquare(int length, double* darray);
	};

	class repixels
	{
	public:
		repixels() {}
	
		repixels(int dt_change, int do_change, int width, int height, long bmpSize, BYTE* data, BYTE ret[CHARACTER_NUM], const char* openFile);
	
		BYTE ret[CHARACTER_NUM];
	
		void averagepixels(long bmpSize, BYTE* data);
			
		// kind: 0 not change white, 1 change white
		void changepixels(int kind, int width, int height, long bmpSize, BYTE* data);
		
		void mergeimages(int width, int height, const BYTE* dt_data, const BYTE* do_data, BYTE* data);
		
		void findnearestpixel(int ih, int iw, int height, int width, BYTE pixel, int& dis, const BYTE* data);
	
		void recoverimage(int width, int height, const BYTE* fake, BYTE* data);
	
		// to one 3 by 3 block, if the center is one kind of pixel
		// and the other 8 are another kind of pixel, then replace the center pixel
		void removelonelypixel(int width, int height, BYTE* data);
	
		// stuff the edge with background color (black)
		void filledges(int width, int height, BYTE* data);
	
		// toward the black pixels, if there are two white pixels lies at its 
		// (west, east), (north, south), (north-west, south-east), (north-east, south-west)
		// change the black to white
		void imagegrowing(int width, int height, BYTE* data);
	
		// if there are black rectangle which size is m by n and {m >= 2 and n >= 2}
		// through the observation, the size of black rectangle should be one part of the characters
		void find_thick_rectangle(int width, int height, vector<Point>& left_top, vector<Point>& right_bottom, BYTE* data);
	
		void merge_rectangles(vector<Point>& left_top, vector<Point>& right_bottom);
	
		void draw_rectangeles(vector<Point>& left_top, vector<Point>& right_bottom, int width, int height, BYTE* data);
	};
	
	class distinguish
	{
	public:
		distinguish() {}
		distinguish(int width, int height, BYTE*& data, BYTE ret[CHARACTER_NUM], const char* openFile);

		vector<Point> left_top;
		vector<Point> right_bottom;
		vector<charTemplate> ctemplates;
	
		inline void segment(int width, int height, BYTE*& data);
		inline void subsegment(int width, int height, int h_start, int h_end, BYTE* data);
		inline int readtemplates(const char* path);
		inline void readtemplates_data(BYTE name, const char* path);
		inline void clearoneline(int s_col, int e_col, int s_row, int e_row, int width, BYTE* data);
		inline void remove_redunant_pixels(int width, int height, BYTE* data);
		inline void remove_black_pixels(int vertical_start, int vertical_end, int horizontal_start, int horizontal_end, int width, BYTE* data);
	
		inline LL calMpq(int p, int q, int width, int height, BYTE* matrix);
		inline double calCenterMoment(int p, int q, int width, int height, BYTE* matrix);
		inline void calRegularMoment(int width, int height, BYTE* matrix, double moments[MOMENTS_LENGTH]);
		inline double calMomentSimilar(double moments1[MOMENTS_LENGTH], double moments2[MOMENTS_LENGTH]);
		inline double calMomentSimilarByCos(double moments1[MOMENTS_LENGTH], double moments2[MOMENTS_LENGTH]);
	
		BYTE distinguishOneCharacter(int index, int width, BYTE* data, const char* openFile);

		BYTE distinguishOneCharacterByCos(int index, int width, BYTE* data, const char* openFile);
	};
}

#endif
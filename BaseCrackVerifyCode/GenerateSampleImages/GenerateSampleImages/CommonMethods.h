#ifndef	_COMMON_METHODS_H_
#define _COMMON_METHODS_H_

#include "GenerateSampleImages.h"

LL calPow(LL base, LL exponent);

void WriteLog(char* message);

void reverseArray(int begin, int end, char* ary);

void changeInt2CharArray(int number, char*& aryNum);

void SpellSampleFilePath(int number, char samplePath[]);

// len(data) = width * height
// len(fake) = width * height * 3
void singlizeimage(int width, int height, BYTE* data, BYTE* fake);

void averagepixels(long bmpSize, BYTE* data);

void binarization(long bmpSize, BYTE* data);

void SpellSaveFilePathByNumbers(int fileType, vector<long> nums, char saveFile[]);

void SpellSaveFilePathTail(int fileType, char saveFile[]);

#endif


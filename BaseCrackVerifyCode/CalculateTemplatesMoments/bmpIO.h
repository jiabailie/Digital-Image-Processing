#ifndef _BMPIO_H_
#define _BMPIO_H_

#include "CalculateTemplatesMoments.h"

void bmpOpen(map<char, vector<vector<double> > >& moments, char imagePath[PathLength]);

void spellOpenFilePath(int fileType, vector<int> nums, char filePath[PathLength]);

void spellOpenFilePathTail(int fileType, char filePath[PathLength]);

FILE_ACCESS_STATUS Open(const char* path, BMPIMAGE image, BMPINFO info);

void binarization(long bmpSize, BYTE* data);

void singlizeimage(int width, int height, BYTE* data, BYTE* fake);

inline LL calPow(LL base, LL exponent);

inline LL calMpq(int p, int q, int width, int height, BYTE* matrix);

inline double calCenterMoment(int p, int q, int width, int height, BYTE* matrix);

inline void calRegularMoment(int width, int height, BYTE* matrix, double moments[MOMENTS_LENGTH]);

#endif
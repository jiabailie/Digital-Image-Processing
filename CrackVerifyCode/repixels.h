#pragma once
#include "graphics.h"
#include "common.h"

void repixels(int dt_change, int do_change, int width, int height, long bmpSize, unsigned char* data);

void averagepixels(long bmpSize, unsigned char* data);

void binarization(long bmpSize, unsigned char* data);

void singlizeimage(int width, int height, unsigned char* fake, unsigned char* data);

// kind: 0 not change white, 1 change white
void changepixels(int kind, int width, int height, long bmpSize, unsigned char* data);

void mergeimages(int width, int height, const unsigned char* dt_data, const unsigned char* do_data, unsigned char* data);

void findnearestpixel(int ih, int iw, int height, int width, unsigned char pixel, int& dis, const unsigned char* data);

void recoverimage(int width, int height, const unsigned char* fake, unsigned char* data);

// to one 3 by 3 block, if the center is one kind of pixel
// and the other 8 are another kind of pixel, then replace the center pixel
void removelonelypixel(int width, int height, unsigned char* data);

// stuff the edge with background color (black)
void filledges(int width, int height, unsigned char* data);
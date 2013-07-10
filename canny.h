#pragma once
#include "io.h"
#include "common.h"
#include "graphics.h"

#define DEBUG_PRINT_ANGLE
#define DEBUG_REMOVE_PIXEL

// using canny algorithm to do edge detection

// the main branch which controls the process
void cannyEdgesDetector(int filterBranch, int width, int height, long bmpSize, unsigned char*& data);

// from {-45, 0, 45, 90}, find nearest angle to input angle
int getCertainAngle(int gradientX, int gradientY);

// using Gaussian filter to do noise reduction
// param : the Gaussian filter array
void deNoiseFilter(int width, int height, long bmpSize, int param[], unsigned char* data);

// calculate the intensity gradient of the image
void calGradient(int width, int height, long bmpSize, const unsigned char* data, int* gradientX, int* gradientY);

// calculate the angle of edge gradient of the image
void calGradientAngle(int width, int height, const int* gradientX, const int* gradientY, int*& angle);

// calculate the gradient of edge gradient of the image
void calGradientMagnitude(int width, int height, const int* gradientX, const int* gradientY, double*& gradient);

// non-maximum suppression
void suppressionNonMaximum(int width, int height, const int* angle, double* gradient);

// using hysteresis thresholding to do edges tracing
void edgesTracing(int width, int height, const double* gradient, unsigned char* data);

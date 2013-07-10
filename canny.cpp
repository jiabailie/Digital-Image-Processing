// using canny algorithm to do edge detection

#include "canny.h"

#undef DEBUG_PRINT_ANGLE
#undef DEBUG_REMOVE_PIXEL

int CANNY_FACTOR[4][4] = {	{ 0,  -1,  0,  1}, 
				{-1,   0,  1,  0}, 
				{-1,  -1,  1,  1}, 
				{-1,   1,  1, -1}
			};

int filters[Filter_Num][9] = 
			{	{5,  5,  5, -3,  1, -3, -3, -3, -3}, 	// 0 kirsch1
				{-3, -3, -3, -3,  1, -3, 5, 5, 5}, 	// 1 kirsch2
				{ 5, -3, -3, 5,  1, -3, 5, -3, -3}, 	// 2 kirsch3
				{ 1,  2,  1, 0,  1,  0, -1, -2, -1}, 	// 3 robinson
				{ 1,  0, -1, 2,  1, -2, 1,  0, -1}, 	//  4 sobelx
				{ 1,  2,  1, 0,  1,  0, -1, -2, -1},	//  5 sobely
				{ 0, -1,  0, -1,  1,  1, 0,  1,  0},	//  6 rightbottom
				{ 0,  0,  0, 0,  1,  0, 0,  0,  0} 	//  7 original
			};

// the main branch which controls the process
void cannyEdgesDetector(int filterBranch, int width, int height, long bmpSize, unsigned char*& data)
{
	int i = 0;
	long pos = 0;
	bool* positions = 0;
	int* gradientAngle = 0;
	double* gradientMagnitude = 0;

	unsigned char* fake = (unsigned char*)malloc(sizeof(unsigned char) * bmpSize);

	int* gradientX = (int*)malloc(sizeof(int) * width * height);
	int* gradientY = (int*)malloc(sizeof(int) * width * height);

	memset(gradientX, 0, sizeof(int) * width * height);
	memset(gradientY, 0, sizeof(int) * width * height);

	findNonZeroBits(filterBranch, positions);

	for(pos = 0; pos < bmpSize; pos++)
	{
		fake[pos] = data[pos];
	}

	// noise reduction
	for(i = 0; i < Filter_Num; i++)
	{
		if(positions[i])
		{
			deNoiseFilter(width, height, bmpSize, filters[i], fake);
		}
	}

	// calculate the gradient
	calGradient(width, height, bmpSize, fake, gradientX, gradientY);

	// calculate the edge gradient angle
	calGradientAngle(width, height, gradientX, gradientY, gradientAngle);

#ifdef DEBUG_PRINT_ANGLE // write angle matrix

	char anglePath[PathLength] = PROCESS_TEXT_SAVE_PATH;
	spellSaveFilePath(TXTTYPE, filterBranch, anglePath);

	writeToFile(width, height, gradientAngle, anglePath);

#endif

	// calculate the edge gradient gradient
	calGradientMagnitude(width, height, gradientX, gradientY, gradientMagnitude);

	// suppress the non maximum gradient
	suppressionNonMaximum(width, height, gradientAngle, gradientMagnitude);

	// according to the gradient which has been processed through suppress the non maximum point, find which points are on the edges and which are not
	memset(fake, unsigned char(0), sizeof(unsigned char) * bmpSize);
	edgesTracing(width, height, gradientMagnitude, fake);

#ifdef DEBUG_REMOVE_PIXEL

#endif

	free(data);

	data = fake;
}

// from {-45, 0, 45, 90}, find nearest angle to input angle
int getCertainAngle(int gradientX, int gradientY)
{
	int i = 0;
	int ret = DIRECTIONS[0];
	double angle = getAngle(gradientX, gradientY);
	double minus = iABS(DIRECTIONS[0] - angle);

	for(i = 1; i < DIREC_ARRAY_LEN; i++)
	{
		if(iABS(DIRECTIONS[i] - angle) < minus)
		{
			ret = DIRECTIONS[i];
			minus = iABS(DIRECTIONS[i] - angle);
		}
	}
	return ret;
}

// using Gaussian filter to do noise reduction
// param : the Gaussian filter array
void deNoiseFilter(int width, int height, long bmpSize, int param[], unsigned char* data)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int m = 0;
	int n = 0;
	int z = 0;
	int tmp = 0;
	int tmpRGB[3];
	long cur = 0;

	unsigned char* fake = (unsigned char*)malloc(sizeof(unsigned char) * bmpSize);

	memset(tmpRGB, 0, sizeof(tmpRGB));

	for(cur = 0; cur < bmpSize; cur++)
	{
		fake[cur] = data[cur];
	}

	for(i = 1; i <= height - 2; i++)
	{
		for(j = 1; j <= width - 2; j++)
		{
			k = 0;			
			memset(tmpRGB, 0, sizeof(tmpRGB));

			// run the filter
			for(m = -1; m <= 1; m++)
			{
				for(n = -1; n <= 1; n++)
				{
					tmp = INDEX(i + m, j + n, width);
					for(z = 0; z < 3; z++)
					{
						tmpRGB[z] += (int)(fake[tmp + z] * param[k]);
					}
					k++;
				}
			}
			tmp = INDEX(i, j, width);
			for(z = 0; z < 3; z++)
			{
			    data[tmp + z] = (unsigned char)(roundPixelValue(tmpRGB[z]));
			}
		}
	}

	free(fake);
}

// calculate the intensity gradient of the image
// using sobel operator
void calGradient(int width, int height, long bmpSize, const unsigned char* data, int* gradientX, int* gradientY)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int m = 0;
	int n = 0;
	int tmp = 0;
	int tmpX = 0;
	int tmpY = 0;
	long pixelNum = width * height;

	int* fake = (int*)malloc(sizeof(int) * pixelNum);

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			tmp = INDEX(i, j, width);
			fake[i * width + j] = ((int)data[tmp] + (int)data[tmp + 1] + (int)data[tmp + 2]) / 3;
		}
	}

	for(i = 1; i < height - 1; i++)
	{
		for(j = 1; j < width - 1; j++)
		{
			k = 0;
			tmpX = 0;
			tmpY = 0;
			for(m = -1; m <= 1; m++)
			{
				for(n = -1; n <= 1; n++)
				{
					tmp = (i + m) * width + (j + n);
					tmpX += fake[tmp] * filters[SobelX][k];
					tmpY += fake[tmp] * filters[SobelY][k];
					k++;
				}
			}
			gradientX[i * width + j] = tmpX;
			gradientY[i * width + j] = tmpY;
		}
	}

	free(fake);
}

// calculate the angle of edge gradient of the image
void calGradientAngle(int width, int height, const int* gradientX, const int* gradientY, int*& angle)
{
	int i = 0;
	int j = 0;
	int tmp = 0;
	
	angle = (int*)malloc(sizeof(int) * width * height);

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			tmp = i * width + j;
			angle[tmp] = getCertainAngle(gradientX[tmp], gradientY[tmp]);
		}
	}
}

// calculate the gradient of edge gradient of the image
void calGradientMagnitude(int width, int height, const int* gradientX, const int* gradientY, double*& gradient)
{
	int i = 0; 
	int j = 0;
	int tmp = 0;
	
	gradient = (double*)malloc(sizeof(double) * width * height);

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			tmp = i * width + j;
			gradient[tmp] = sqrt(gradientX[tmp] * gradientX[tmp] + gradientY[tmp] * gradientY[tmp] * 1.0);
		}
	}
}

// non-maximum suppression
//  45 : (x - 1, y - 1), (x + 1, y + 1)
//  90 : (x    , y - 1), (x    , y + 1)
// -45 : (x - 1, y + 1), (x + 1, y - 1)
//   0 : (x - 1, y    ), (x + 1, y    )
void suppressionNonMaximum(int width, int height, const int* angle, double* gradient)
{
	int ih = 0;
	int iw = 0;
	int pos0 = 0;
	int pos1 = 0;
	int pos2 = 0;
	canny_direction dir = CANNY_HORIZONTAL;
	double* tgradient = (double*)malloc(sizeof(double) * width * height);

	memcpy(tgradient, gradient, sizeof(double) * width * height);

	for(ih = 1; ih < height - 1; ih++)
	{
		for(iw = 1; iw < width - 1; iw++)
		{
			pos0 = ih * width + iw;
			switch(angle[pos0])
			{
			case 0:
				dir = CANNY_VERTICAL;
				break;
			case 90:
				dir = CANNY_HORIZONTAL;
				break;
			case -45:
				dir = CANNY_RIGHT_DIAGONAL;
				break;
			case 45:
				dir = CANNY_LEFT_DIAGONAL;
				break;
			default:
				break;
			}

			pos1 = (ih + CANNY_FACTOR[dir][0]) * width + (iw + CANNY_FACTOR[dir][1]);
			pos2 = (ih + CANNY_FACTOR[dir][2]) * width + (iw + CANNY_FACTOR[dir][3]);

			if(tgradient[pos0] >= tgradient[pos1] && tgradient[pos0] >= tgradient[pos2])
			{
				continue;
			}
			gradient[pos0] = 0;
		}
	}

	free(tgradient);
}

// using hysteresis thresholding to do edges tracing
void edgesTracing(int width, int height, const double* gradient, unsigned char* data)
{
	int ih = 0;
	int iw = 0;
	int pos = 0;
	int value = 0;

	for(ih = 0; ih < height; ih++)
	{
		for(iw = 0; iw < width; iw++)
		{
			pos = INDEX(ih, iw, width);
			value = BLACK;
			if(gradient[ih * width + iw] == 0)
			{
				value = WHITE;
			}
			data[pos] = value;
			data[pos + 1] = value;
			data[pos + 2] = value;
		}
	}
}

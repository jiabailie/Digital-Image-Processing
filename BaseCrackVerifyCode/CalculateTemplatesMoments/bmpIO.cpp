#include "StdAfx.h"
#include "bmpIO.h"

void bmpOpen(map<char, vector<vector<double> > >& moments, char imagePath[PathLength])
{
	int i = 0;
	int j = 0;
	int k = 0;
	int width = 0;
	int height = 0;
	int curLen = 0;
	BYTE* data = 0;
	BYTE* fake = 0;
	FILE_ACCESS_STATUS status = SUCCESS;
	double _moments[MOMENTS_LENGTH];
	char _imagePath[PathLength];

	char template_name[TEMPLATE_COUNT] = 
							 {'0', '1', '2', '3', '4', '5', 
							  '6', '7', '8', '9', 'A', 'B',
							  'C', 'D', 'E', 'F', 'G', 'H',
							  'I', 'J', 'K', 'L', 'M', 'N',
							  'O', 'P', 'Q', 'R', 'S', 'T',
							  'U', 'V', 'W', 'X', 'Y', 'Z'};
	curLen = strlen(imagePath);
	for(i = 0; i < TEMPLATE_COUNT; i++)
	{
		for(j = 0; j < IMGPERCHAR; j++)
		{	
			vector<int> nums;
			nums.push_back(j);

			memset(_imagePath, 0, sizeof(_imagePath));
			memcpy(_imagePath, imagePath, sizeof(char) * strlen(imagePath));

			_imagePath[curLen] = template_name[i];
			_imagePath[curLen + 1] = 0;
			spellOpenFilePath(BMPTYPE, nums, _imagePath);

			BMPIMAGE image = (bmpImage*)malloc(sizeof(bmpImage));
			BMPINFO info = (infoFetch*)malloc(sizeof(infoFetch));
			
			status = Open(_imagePath, image, info);

			if(status == NOT_EXIST)
			{
				free(image);
				free(info);
				break;
			}

			width = info->w;
			height = info->h;

			data = (BYTE*)malloc(sizeof(BYTE) * width * height);
			fake = (BYTE*)malloc(sizeof(BYTE) * info->bmpSize);

			memcpy(fake, info->data, sizeof(BYTE) * info->bmpSize);
			binarization(info->bmpSize, fake);
			singlizeimage(width, height, data, fake);

			calRegularMoment(width, height, data, _moments);

			//for(k = 0; k < MOMENTS_LENGTH; k++)
			//{
			//	_moments[k] *= 10000;
			//}

			vector<double> v_moments(_moments, _moments + sizeof(_moments) / sizeof(double));

			moments[template_name[i]].push_back(v_moments);

			free(image);
			free(info);
			free(data);
			free(fake);
		}
	}
}

void spellOpenFilePath(int fileType, vector<int> nums, char filePath[PathLength])
{
	int i = 0;
	int curLen = strlen(filePath);
	char tmp[NumLength];

	filePath[curLen] = '(';
	curLen = curLen + 1;
	for(vector<int>::iterator it = nums.begin(); it != nums.end(); it++)
	{
		memset(tmp, 0, sizeof(tmp));

		_itoa_s((*it), tmp, 10);
		for(i = 0; i < int(strlen(tmp)); i++)
		{
			filePath[curLen] = tmp[i];
			curLen = curLen + 1;
		}
		filePath[curLen] = ')';
		curLen = curLen + 1;
	}
	spellOpenFilePathTail(fileType, filePath);
}

void spellOpenFilePathTail(int fileType, char filePath[PathLength])
{
	int curLen = strlen(filePath);
	if(filePath[curLen - 1] == '_')
	{
		curLen--;
	}
	filePath[curLen++] = '.';

	switch(fileType)
	{
	case BMPTYPE:
		filePath[curLen++] = 'b';
		filePath[curLen++] = 'm';
		filePath[curLen++] = 'p';
		break;
	case TXTTYPE:
		filePath[curLen++] = 't';
		filePath[curLen++] = 'x';
		filePath[curLen++] = 't';
		break;
	default:
		break;
	}

	filePath[curLen] = '\0';
}

FILE_ACCESS_STATUS Open(const char* path, BMPIMAGE image, BMPINFO info)
{
	int rgb = 0;
	long bmpSize = 0;

	FILE * pFile = 0;

	BYTE* cdata = 0;

	fopen_s(&pFile, path, "rb");

	if(pFile == NULL)
	{
		printf("File %s Not Exist.\n", path);
		return NOT_EXIST;
	}

	fseek(pFile, 0, SEEK_END);
	bmpSize = ftell(pFile);
	rewind(pFile);

	fread(&(image->fileHeader), 1, FILEHEADERSIZE, pFile);
	fread(&(image->infoHeader), 1, INFOHEADERSIZE, pFile);

	info->w = image->infoHeader.biWidth; 
	info->h = image->infoHeader.biHeight;
	info->bitCount = image->infoHeader.biBitCount; 

	switch(image->infoHeader.biBitCount)
	{
	case 1:
		rgb = 2;
		break;
	case 4:
		rgb = 16;
		break;
	case 8:
		rgb = 256;
		break;
	case 24:
		rgb = 0;
		break;
	default:
		break;
	}

	fread(&(image->rgbQuad), rgb * sizeof(RGBQUAD), 1, pFile);
	if(image->infoHeader.biBitCount == 24) 
	{
		info->bmpSize = bmpSize - FILEHEADERSIZE - INFOHEADERSIZE;	
		info->data = (BYTE *)malloc(sizeof(BYTE) * (bmpSize - FILEHEADERSIZE - INFOHEADERSIZE));
		fread(info->data, 1, bmpSize - FILEHEADERSIZE - INFOHEADERSIZE, pFile);
	}

	info->lenLine = ((image->infoHeader.biBitCount * image->infoHeader.biWidth) + 31) / 8;

	fclose(pFile);

	printf("Open File Success!\n\n");
	return SUCCESS;
}

void binarization(long bmpSize, BYTE* data)
{
	long isize = 0;
	for(isize = 0; isize < bmpSize; isize++)
	{
		if(data[isize] != c_WHITE)
		{
			data[isize] = c_BLACK;
		}
	}
}

void singlizeimage(int width, int height, BYTE* data, BYTE* fake)
{
	int i = 0;
	int j = 0;
	int tmp = 0;
	int value = 0;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			tmp = INDEX(i, j, width);
			value = ((fake[tmp] + fake[tmp + 1] + fake[tmp + 2]) / 3);
			value = (value < 255 - value) ? 0 : 255;
			data[i * width + j] = BYTE(value);
		}
	}
}

inline LL calPow(LL base, LL exponent)
{
	if(exponent == 0) { return 1; }
	if(exponent == 1) { return base; }
	if(exponent == 2) { return base * base; }

	LL tmp = calPow(base, exponent >> 1);
	tmp = tmp * tmp;

	if((exponent & 1) == 0) { return tmp; }
	if((exponent & 1) == 1) { return tmp * base; }

	return 1;
}

inline LL calMpq(int p, int q, int width, int height, BYTE* matrix)
{
	LL ret = 0;
	int i = 0;
	int j = 0;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			if(matrix[i * width + j] == c_WHITE)
			{
				ret += calPow(i, p) * calPow(j, q);
			}
		}
	}

	return ret;
}

inline double calCenterMoment(int p, int q, int width, int height, BYTE* matrix)
{
	LL i = 0, j = 0;
	LL U00 = 0, m10 = 0, m01 = 0;
	double Upq = 0;

	m10 = calMpq(1, 0, width, height, matrix);
	m01 = calMpq(0, 1, width, height, matrix);

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			if(matrix[i * width + j] == c_WHITE)
			{
				Upq += double(calPow((i - m10), p) * calPow((j - m01), q));
				U00 += 1;
			}
		}
	}

	if(U00 == 0) { U00 = 1; }
	U00 = calPow(U00, (p + q + 2) >> 1);

	return double(Upq * 1.0 / U00);
}

inline void calRegularMoment(int width, int height, BYTE* matrix, double moments[MOMENTS_LENGTH])
{
	double centerM[7] = {
		calCenterMoment(0, 2, width, height, matrix),
		calCenterMoment(0, 3, width, height, matrix),
		calCenterMoment(1, 1, width, height, matrix),
		calCenterMoment(1, 2, width, height, matrix),
		calCenterMoment(2, 0, width, height, matrix),
		calCenterMoment(2, 1, width, height, matrix),
		calCenterMoment(3, 0, width, height, matrix)	
	};

	double a1 = centerM[1] + centerM[4];
	double a2 = centerM[3] + centerM[6];
	double a3 = a2 * a2 - 3 * a1 * a1;
	double a4 = 3 * a2 * a2 - a1 * a1;
	double a5 = centerM[6] - 3 * centerM[3];
	double a6 = 3 * centerM[5] - centerM[1];
	double a7 = centerM[4] - centerM[0];

	double huM[7] = {
		iABS(centerM[0] + centerM[4]),
		a7 * a7 + 4 * centerM[2] * centerM[2],
		a5 * a5 + a6 * a6,
		a1 * a1 + a2 * a2,
		iABS(a2 * a3 * a5 + a1 * a4 * a6),
		iABS((a2 * a2 - a1 * a1) * a7 + 4 * centerM[2] * a1 * a2),
		iABS(a2 * a3 * a6 - a1 * a4 * a5)
	};

	double powHuM[4] = {};
	powHuM[0] = huM[0] * huM[0];
	powHuM[1] = powHuM[0] * huM[0];
	powHuM[2] = powHuM[1] * huM[0];
	powHuM[3] = powHuM[1] * powHuM[1];

	double lg2 = log((double)2);

	if(huM[0] != 0)
	{
		moments[0] = log(huM[1] * 1.0 / powHuM[0]) / lg2;
		moments[1] = log(huM[2] * 1.0 / powHuM[1]) / lg2;
		moments[2] = log(huM[3] * 1.0 / powHuM[1]) / lg2;
		moments[3] = log(huM[4] * 1.0 / powHuM[3]) / lg2;
		moments[4] = log(huM[5] * 1.0 / powHuM[2]) / lg2;
		moments[5] = log(huM[6] * 1.0 / powHuM[3]) / lg2;
	}
}
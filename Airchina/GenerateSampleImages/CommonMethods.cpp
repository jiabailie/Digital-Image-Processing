#include "StdAfx.h"
#include "CommonMethods.h"

LL calPow(LL base, LL exponent)
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

void WriteLog(char* message)
{
	if(NULL == message || strlen(message) == 0)
	{
		return;
	}

	FILE* pFile = 0;

	fopen_s(&pFile, LOG_FILE, "a+");

	if(NULL == pFile)
	{
		printf("Log path error.\n");
		return;
	}

	fprintf(pFile, "%s\n", message);

	fclose(pFile);
}

void reverseArray(int begin, int end, char* ary)
{
	char c = 0;

	while(begin < end)
	{
		c = ary[begin];
		ary[begin] = ary[end];
		ary[end] = c;

		begin++;
		end--;
	}
}

void changeInt2CharArray(int number, char*& aryNum)
{
	aryNum = (char*) malloc(sizeof(char) * NumLength);
	memset(aryNum, char(0), sizeof(char) * NumLength);

	int cur = 0;
	int sign = number >= 0 ? 1 : -1;
	number = number * sign;

	if(number == 0)
	{
		aryNum[cur] = '0';
		aryNum[cur + 1] = '\0';
	}
	else
	{
		while(number > 0)
		{
			aryNum[cur++] = char((number % 10) + '0');
			number /= 10;
		}

		if(sign < 0) { aryNum[cur++] = '-'; }
		aryNum[cur] = '\0';

		reverseArray(0, cur - 1, aryNum);
	}
}

void SpellSampleFilePath(int number, char samplePath[])
{
	int i = 0;
	int start  = 0;
	char* tmp = (char*)malloc(sizeof(char) * NumLength);

	memset(tmp, 0, sizeof(tmp));

	changeInt2CharArray(number, tmp);

	for(start = 0; start , strlen(samplePath) && samplePath[start] != '('; start++);

	start++;

	for(i = 0; i < int(strlen(tmp)); i++)
	{
		samplePath[start++] = tmp[i];
	}

	samplePath[start++] = ')';
	samplePath[start++] = '.';
	samplePath[start++] = 'b';
	samplePath[start++] = 'm';
	samplePath[start++] = 'p';
	samplePath[start] = '\0';
}


// len(data) = width * height
// len(fake) = width * height * 3
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

void averagepixels(long bmpSize, BYTE* data)
{
	long i = 0;
	int avg = 0;

	for(i = 0; i < bmpSize; i += 3)
	{
		avg = (int(data[i]) + int(data[i + 1]) + int(data[i + 2])) / 3;
		data[i] = data[i + 1] = data[i + 2] = BYTE(avg);
	}
	WriteLog("Run average pixels. E:\\Image\\GenerateSampleImages\\GenerateSampleImages\\CommonMethods.cpp\n");
}

void binarization(long bmpSize, BYTE* data)
{
	long i = 0; 
	for(i = 0; i < bmpSize; i++)
	{
		if(data[i] > p_value)
		{
			data[i] = 255;
		}
		else
		{
			data[i] = 0;
		}
	}
	WriteLog("Run binarization. E:\\Image\\GenerateSampleImages\\GenerateSampleImages\\CommonMethods.cpp\n");
}

void SpellSaveFilePathByNumbers(int fileType, vector<long> nums, char saveFile[])
{
	int i = 0;
	int curLen = strlen(saveFile);
	char tmp[NumLength];

	saveFile[curLen] = '_';
	curLen = curLen + 1;
	for(vector<long>::iterator it = nums.begin(); it != nums.end(); it++)
	{
		memset(tmp, 0, sizeof(tmp));

		_itoa_s((*it), tmp, 10);
		for(i = 0; i < int(strlen(tmp)); i++)
		{
			saveFile[curLen] = tmp[i];
			curLen = curLen + 1;
		}
		saveFile[curLen] = '_';
		curLen = curLen + 1;
	}
	saveFile[curLen] = '\0';
	SpellSaveFilePathTail(fileType, saveFile);
}

void SpellSaveFilePathTail(int fileType, char saveFile[])
{
	int curLen = strlen(saveFile);
	saveFile[curLen - 1] = '.';

	switch(fileType)
	{
	case BMPTYPE:
		saveFile[curLen++] = 'b';
		saveFile[curLen++] = 'm';
		saveFile[curLen++] = 'p';
		break;
	case TXTTYPE:
		saveFile[curLen++] = 't';
		saveFile[curLen++] = 'x';
		saveFile[curLen++] = 't';
		break;
	default:
		break;
	}

	saveFile[curLen] = '\0';
}
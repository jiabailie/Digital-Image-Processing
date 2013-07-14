#include "common.h"

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
	aryNum = (char*)malloc(sizeof(char) * NumLength);
	memset(aryNum, 0, sizeof(char) * NumLength);

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
			aryNum[cur++] = (char)((number % 10) + '0');
			number /= 10;
		}

		if(sign < 0) { aryNum[cur++] = '-'; }
		aryNum[cur] = '\0';

		reverseArray(0, cur - 1, aryNum);
	}
}

void findNonZeroBits(int number, bool*& bits)
{
	bits = (bool*)malloc(sizeof(bool) * (NumLength + 1));
	memset(bits, 0, sizeof(bool) * (NumLength + 1));
	for(int i = 0; i < NumLength + 1; i++)
	{
		if(number & MASK[i])
		{
			bits[i] = true;
		}
	}
}

// fileType: 0 bmp, 1 txt
void spellSaveFilePath1(int fileType, int filterBranch, char saveFile[])
{
	int i = 0;
	int j = 0;
	int curLen = strlen(saveFile);
	char* tmp = (char*)malloc(sizeof(char) * NumLength);
	bool* bits = 0;
	findNonZeroBits(filterBranch, bits);

	for(i = 0; i < NumLength + 1; i++)
	{
		if(bits[i])
		{
			memset(tmp, 0, sizeof(char) * NumLength);
			changeInt2CharArray(i, tmp);
			for(j = 0; j < int(strlen(tmp)); j++)
			{
				saveFile[curLen++] = tmp[j];
			}
			saveFile[curLen++] = '_';
		}
	}
	spellSaveFilePathTail(fileType, saveFile);
}

void spellSaveFilePath2(int fileType, vector<int> nums, char saveFile[])
{
	int i = 0;
	int curLen = strlen(saveFile);
	char tmp[NumLength];

	saveFile[curLen] = '_';
	curLen = curLen + 1;
	for(vector<int>::iterator it = nums.begin(); it != nums.end(); it++)
	{
		memset(tmp, 0, sizeof(tmp));

		itoa((*it), tmp, 10);
		for(i = 0; i < int(strlen(tmp)); i++)
		{
			saveFile[curLen] = tmp[i];
			curLen = curLen + 1;
		}
		saveFile[curLen] = '_';
		curLen = curLen + 1;
	}
	spellSaveFilePathTail(fileType, saveFile);
}

void spellSaveFilePathTail(int fileType, char saveFile[])
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
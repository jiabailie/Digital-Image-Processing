#include "stdafx.h"
#include "BaseCrackVerifyCode.h"

namespace CrackVerifyCode
{
	void common::reverseArray(int begin, int end, char* ary)
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

	void common::changeInt2CharArray(int number, char*& aryNum)
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

	void common::findNonZeroBits(int number, bool*& bits)
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

	void common::replaceFilePathForTemplate(char character, char filePath[])
	{
		int index = 0;
		int len = strlen(filePath);
		if(filePath == NULL || len == 0)
		{
			return;
		}
		for(index = len - 1; index >= 0; index--)
		{
			if(filePath[index] == '.' && index > 0)
			{
				filePath[index - 1] = character;
				break;
			}
		}
	}

	// fileType: 0 bmp, 1 txt
	void common::spellSaveFilePath1(int fileType, int filterBranch, char saveFile[])
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

	void common::spellSaveFilePath2(int fileType, vector<int> nums, char saveFile[])
	{
		int i = 0;
		int curLen = strlen(saveFile);
		char tmp[NumLength];

		saveFile[curLen] = '_';
		curLen = curLen + 1;
		for(vector<int>::iterator it = nums.begin(); it != nums.end(); it++)
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
		spellSaveFilePathTail(fileType, saveFile);
	}

	void common::spellSaveFilePathTail(int fileType, char saveFile[])
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

	void common::singlizeimage(int width, int height, BYTE* data, BYTE* fake)
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

	LL common::calPow(LL base, LL exponent)
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

	void common::transferData(const char* data, char& name, double moments[MOMENTS_LENGTH])
	{
		int i = 0;
		int j = 0;
		int k = 0;
		int start = 0;
		int end = 0;
		int sign = 1;
		int len = strlen(data);

		while(data[start] == ' ') { start++; }

		name = data[start];
		start++;
		
		for(i = 0; i < MOMENTS_LENGTH; i++)
		{
			while(data[start] == ' ') { start++; }

			end = start;

			while(data[end] != ' ' && end != len) { end++; }

			sign = 1;
			if(data[start] == '-')
			{
				sign = -1;
				start++;
			}

			char tmp[50];
			memset(tmp, 0, sizeof(tmp));
			for(j = start, k = 0; j <= end; j++, k++)
			{
				tmp[k] = data[j];
			}
			tmp[k - 1] = 0;
			moments[i] = atof(tmp) * sign;

			start = end + 1;
		}
	}
	void common::binarization(long bmpSize, BYTE* data)
	{
		long isize = 0;
		for(isize = 0; isize < bmpSize; isize++)
		{
			if(data[isize] > p_value)
			{
				data[isize] = 255;
			}
			else
			{
				data[isize] = 0;
			}
		}
	}

	double common::calSquare(int length, double* darray)
	{
		double ret = 0.0;
		for(int i = 0; i < length; i++)
		{
			ret += darray[i] * darray[i] * 1.0;
		}
		return ret;
	}
}
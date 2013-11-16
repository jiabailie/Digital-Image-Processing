#include "graphics.h"
#include "io.h"

// the data array is a matrix size of width by height
// 0 int, 1 bool, 2 unsigned char
template<class T>
void writeToFile(int type, int width, int height, T* data, const char* path)
{
	int i = 0;
	int j = 0;
	int status = FILE_OPEN_SUCCESS;
	FILE* fp = 0;

	fp = fopen(path, "w");

	if(fp == NULL)
	{
		status = FILE_NOT_EXIST;
	}
	else
	{
		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
				writeOper(type, data[i * width + j], fp);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
}

template<class T>
void writeOper(int type, T data, FILE* fp)
{
	switch(type)
	{
	case 0: // int
		fprintf(fp, "%d ", data);
		break;
	case 1: // bool
		fprintf(fp, (data ? "1" : " "));
		break;
	case 2: // unsigned char
		fprintf(fp, "d ", (int(data) == 0 ? 1 : 0));
		break;
	default:
		break;
	}
}
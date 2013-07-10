#include "graphics.h"
#include "canny.h"
#include "io.h"
#include "common.h"
#include "repixels.h"

//打开bmp文件
void Open(char* path, BMPIMAGE image, BMPINFO info)
{
  int rgb;
	long bmpSize;

	FILE * pFile;

	unsigned char* cdata = 0;

	pFile = fopen(path, "rb");

	if(pFile == NULL)
	{
		printf("file not exist.\n");
	}
	else
	{
		//读取bmp文件长度
		fseek(pFile, 0, SEEK_END);
		bmpSize = ftell(pFile);
		rewind(pFile);

		fread(&(image->fileHeader), 1, FILEHEADERSIZE, pFile);//读取bmp文件头
		fread(&(image->infoHeader), 1, INFOHEADERSIZE, pFile);//读取bmp信息头

		info->w = image->infoHeader.biWidth; //读取图片宽度
		info->h = image->infoHeader.biHeight;//读取图片高度
		info->bitCount = image->infoHeader.biBitCount; //读取biBitCount值

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
		if(image->infoHeader.biBitCount == 24) //如果图片为真彩色
		{
			info->bmpSize = bmpSize - FILEHEADERSIZE - INFOHEADERSIZE;	//计算bmp文件图形数据长度
			info->data = (BYTE *)malloc(sizeof(BYTE) * (bmpSize - FILEHEADERSIZE - INFOHEADERSIZE));
			fread(info->data, 1, bmpSize - FILEHEADERSIZE - INFOHEADERSIZE, pFile); //读取bmp图形数据部分

			repixels(info->w, info->h, info->bmpSize, info->data);
		}

		//计算每行的数据长度
		info->lenLine = ((image->infoHeader.biBitCount * image->infoHeader.biWidth) + 31) / 8;

		fclose(pFile);

		printf("Open File Success!\n\n");
	}
}

//保存为bmp文件(biBitCount为24)
void Save(char* path, BMPIMAGE image, BMPINFO info)
{
	FILE * pFile;
	long lineLength;
	long totalLength;

	int bytePerpixel = 3;
	long width = info->w;
	long height = info->h;

	unsigned char header[BMPHEADERSIZE] = {
		0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0,
		40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0};

	long fileSize = (long)(info->w) * (long)(info->h) * 3 + 54;

	header[2] = (unsigned char)(fileSize & 0x000000ff);
	header[3] = (fileSize >> 8) & 0x000000ff;
	header[4] = (fileSize >> 16) & 0x000000ff;
	header[5] = (fileSize >> 24) & 0x000000ff;

	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) & 0x000000ff;
	header[20] = (width >> 16) & 0x000000ff;
	header[21] = (width >> 24) & 0x000000ff;

	header[22] = height & 0x000000ff;
	header[23] = (height >> 8) & 0x000000ff;
	header[24] = (height >> 16) & 0x000000ff;
	header[25] = (height >> 24) & 0x000000ff;

	pFile = fopen(path, "wb");
	if(pFile == NULL)
	{
		printf("Error.\n");
	}
	else
	{
		fwrite(header, sizeof(unsigned char), 54, pFile);

		lineLength = width * bytePerpixel;	//每行数据长度大致为图像宽度乘以每像素的字节数

		while(lineLength % 4 != 0)			//修正lineLength使其为4的倍数
		{
			lineLength++;
		}

		totalLength = lineLength * height;	//数据总长 = 每行长度 * 图像高度

		fwrite(info->data, sizeof(unsigned char), (size_t)totalLength, pFile);
		fclose(pFile);

		printf("Save File Success!\n\n");
	}
}

void RemovePixels()
{
	char* openFile = PROCESS_FILE_OPEN_PATH;
	char* saveFile = PROCESS_REMOVEBLA_PATH;

	BMPIMAGE image = (bmpImage*)malloc(sizeof(bmpImage));
	BMPINFO info = (infoFetch*)malloc(sizeof(infoFetch));

	printf("Open the bmp file...\n");
	Open(openFile, image, info);

	switch(info->bitCount)
	{
	case 24:
		printf("Save the bmp file into file %s\n", saveFile);
		Save(saveFile, image, info);
		break;
	default:
		break;
	}
}

int main()
{
	printf("/*****************File Begin*****************/\n");

	RemovePixels();

	printf("/*****************File End*****************/\n");

	return 0;
}

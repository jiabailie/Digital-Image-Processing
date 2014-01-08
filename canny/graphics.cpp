#include "graphics.h"
#include "canny.h"
#include "io.h"
#include "common.h"

// open bitmap file
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
		// get the length this bitmap file
		fseek(pFile, 0, SEEK_END);
		bmpSize = ftell(pFile);
		rewind(pFile);

		fread(&(image->fileHeader), 1, FILEHEADERSIZE, pFile);// get the file header of this file
		fread(&(image->infoHeader), 1, INFOHEADERSIZE, pFile);// get the info header of this bitmap

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
			// calculate the data length of this image file
			info->bmpSize = bmpSize - FILEHEADERSIZE - INFOHEADERSIZE;
			
			info->data = (BYTE *)malloc(sizeof(BYTE) * (bmpSize - FILEHEADERSIZE - INFOHEADERSIZE));
			
			// get the data part this file
			fread(info->data, 1, bmpSize - FILEHEADERSIZE - INFOHEADERSIZE, pFile);

			repixels(info->w, info->h, info->bmpSize, info->data);
		}

		// calculate the data length per line
		info->lenLine = ((image->infoHeader.biBitCount * image->infoHeader.biWidth) + 31) / 8;

		fclose(pFile);

		printf("Open File Success!\n\n");
	}
}

// save the bitmap file(biBitCount=24)
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
		
		// data length per line = image width * bytes amount of each pixel
		lineLength = width * bytePerpixel;
		
		// modify lineLength, setting as the times of 4
		while(lineLength % 4 != 0)
		{
			lineLength++;
		}
		
		// data length = data length per line * image height
		totalLength = lineLength * height;

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

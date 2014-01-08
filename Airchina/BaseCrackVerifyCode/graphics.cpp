#include "stdafx.h"
#include "BaseCrackVerifyCode.h"

namespace CrackVerifyCode
{
	int graphics::Open(const char* path, BMPIMAGE image, BMPINFO info)
	{
		int rgb;
		long bmpSize;

		FILE * pFile;

		BYTE* cdata = 0;

		fopen_s(&pFile, path, "rb");

		if(pFile == NULL)
		{
			printf("File %s Not Exist.\n", path);
			return FILE_ACCESS_STATUS_NOT_EXIST;
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

		return FILE_ACCESS_STATUS_SUCCESS;
	}

	void graphics::Save(char* path, BMPIMAGE image, BMPINFO info)
	{
		FILE * pFile;
		long lineLength;
		long totalLength;

		int bytePerpixel = 3;
		long width = info->w;
		long height = info->h;

		BYTE header[BMPHEADERSIZE] = {
			0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0,
			40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0};

		long fileSize = (long)(info->w) * (long)(info->h) * 3 + 54;

		header[2] = (BYTE)(fileSize & 0x000000ff);
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

		fopen_s(&pFile, path, "wb");
		if(pFile == NULL)
		{
			printf("Error.\n");
		}
		else
		{
			fwrite(header, sizeof(BYTE), 54, pFile);

			lineLength = width * bytePerpixel;	

			while(lineLength % 4 != 0)			
			{
				lineLength++;
			}

			totalLength = lineLength * height;	

			fwrite(info->data, sizeof(BYTE), (size_t)totalLength, pFile);
			fclose(pFile);

			printf("Save File Success!\n\n");
		}
	}

	void graphics::RemovePixels(const char* openFile, BYTE ret[CHARACTER_NUM])
	{
		int i = 10;
		int j = 1;
		vector<int> nums;

		for(i = DT_CHANGE_WHITE_TIMES; i <= DT_CHANGE_WHITE_TIMES; i++)
		{
			for(j = DO_CHANGE_WHITE_TIMES; j <= DO_CHANGE_WHITE_TIMES; j++)
			{
				char saveFile[PathLength] = PROCESS_REMOVEBLA_PATH;

				nums.clear();
				nums.push_back(i);
				nums.push_back(j);

				BMPIMAGE image = (bmpImage*)malloc(sizeof(bmpImage));
				BMPINFO info = (infoFetch*)malloc(sizeof(infoFetch));

				printf("Open the bmp file...\n");
			
				if(Open(openFile, image, info) == FILE_ACCESS_STATUS_SUCCESS)
				{
					repixels(i, j, info->w, info->h, info->bmpSize, info->data, ret, openFile);
				}

				free(image);
				free(info);
			}
		}
	}
}
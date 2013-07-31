// GenerateSampleImages.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GenerateSampleImages.h"
#include "ReadVerifyCode.h"
#include "CommonMethods.h"
#include "ImageSegments.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int i = 0;
	char filePath[PathLength];

	for(i = 0; i < (1 << SAMPLE_AMOUNT); i++)
	{
		WriteLog("/*************File process begin*************/\n");

		strcpy_s(filePath, SAMPLE_INPUT_FILE);
		SpellSampleFilePath(i, filePath);

		EnhanceImage(i, filePath);

		WriteLog("/*************File process end*************/\n");
	}
	return 0;
}


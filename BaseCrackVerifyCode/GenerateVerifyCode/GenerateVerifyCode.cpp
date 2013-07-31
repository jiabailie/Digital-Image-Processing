// GenerateVerifyCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BaseCrackVerifyCode.h"
#include <iostream>
#include <cstring>

#define DEBUG_FORCORRECTNESS
//#undef	DEBUG_FORCORRECTNESS

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE ret[4] = {};

	if(argc < 2)
	{
		printf("There is no input parameter.\n");
		return 0;
	}

	int len = wcslen(argv[1]);
	
	char path[PathLength];
	CrackVerifyCode::crackVerifyCode _crackImage;

	for(int i = 0; i < len; i++)
	{
		path[i] = char(argv[1][i]);
	}
	path[len] = '\0';
	
	_crackImage.CrackVerifyCode(path, ret);

	printf("result:");

	for(int i = 0; i < 4; i++)
	{
		printf("%c", ret[i]);
	}

#ifdef DEBUG_FORCORRECTNESS

	FILE* fp = 0;
	fopen_s(&fp, "E:\\Projects\\CrackVerifyCode\\correctness\\results.txt", "at");

	if(NULL == fp)
	{
		printf("correctness record file not exist.\n");
	}

	for(int i = 0; i < 4; i++)
	{
		fprintf(fp, "%c", ret[i]);
	}
	fprintf(fp, "\n");

	fclose(fp);

#endif

	//getchar();
	//getchar();

	return 0;
}
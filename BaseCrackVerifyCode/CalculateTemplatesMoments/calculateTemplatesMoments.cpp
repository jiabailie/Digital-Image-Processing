// CalculateTemplatesMoments.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CalculateTemplatesMoments.h"
#include "bmpIO.h"
#include "txtIO.h"

int _tmain(int argc, _TCHAR* argv[])
{
	char imagePath[PathLength] = "..\\data\\";
	char txtPath[PathLength] = "..\\data\\moments.txt";

	map<char, vector<vector<double> > > moments;

	bmpOpen(moments, imagePath);

	writeMomentsToFile(moments, txtPath);

	return 0;
}


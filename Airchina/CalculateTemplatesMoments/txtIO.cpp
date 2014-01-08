#include "StdAfx.h"
#include "txtIO.h"

void writeMomentsToFile(map<char, vector<vector<double> > >& moments, char filePath[PathLength])
{
	FILE * pFile = 0;

	fopen_s(&pFile, filePath, "wb");
	if(!pFile)
	{
		printf("File %s not exist.\n", filePath);
		return;
	}

	for(map<char, vector<vector<double> > >::iterator it = moments.begin(); it != moments.end(); it++)
	{
		for(vector<vector<double> >::iterator iit = it->second.begin(); iit != it->second.end(); iit++)
		{
			fprintf(pFile, "%c ", it->first);
			for(vector<double>::iterator dit = (*iit).begin(); dit != (*iit).end(); dit++)
			{
				fprintf(pFile, "%.15f ", (*dit));
			}
			fprintf(pFile, "\n");
		}
	}

	fclose(pFile);
}
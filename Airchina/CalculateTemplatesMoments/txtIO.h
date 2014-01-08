#ifndef _TXTIO_H_
#define _TXTIO_H_

#include "CalculateTemplatesMoments.h"

void writeMomentsToFile(map<char, vector<vector<double> > >& moments, char filePath[PathLength]);

#endif


#pragma once

#define FILE_OPEN_SUCCESS	0
#define FILE_NOT_EXIST	   -1
#define FILE_WRITE_ERROR   -2

// write short array to a text file
template<class T>
void writeToFile(int type, int width, int height, T* data, const char* path);

template<class T>
void writeOper(int type, T data, FILE* fp);
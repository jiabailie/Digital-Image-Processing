//
//  common.h
//  ImageProcessing
//
//  Created by Yang Ruiguo on 14/2/19.
//  Copyright © 2019 Yang Ruiguo. All rights reserved.
//

#ifndef common_h
#define common_h

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <vector>
#include <iostream>

using namespace std;

namespace image_processing {
    
    typedef unsigned char  BYTE;
    typedef unsigned short WORD;
    typedef unsigned long  DWORD;
    typedef unsigned int   STATUS;

    const double PI          = 3.1415926;
    const BYTE c_BLACK       = 0;
    const BYTE c_WHITE       = 255;
    const int BMPHEADERSIZE  = 54 ;
    const int FILEHEADERSIZE = 14;
    const int INFOHEADERSIZE = 40;

    const STATUS SUCCESS = 0x0000;
    const STATUS FAILURE = 0x1000;


    // Reverse array from start to end.
    inline void ReverseArray(int start, int end, BYTE* array) {
        if(!array || start == end) {
            return;
        }

        while(start < end) {
            BYTE c = array[start];
            array[start] = array[end];
            array[end] = c;

            start += 1;
            end -= 1;
        }
    }

}; // END namespace image_processing

#endif /* common_h */

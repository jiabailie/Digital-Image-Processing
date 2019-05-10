//
//  graphics.h
//  ImageProcessing
//
//  Created by Yang Ruiguo on 14/2/19.
//  Copyright © 2019 Yang Ruiguo. All rights reserved.
//

#ifndef graphics_h
#define graphics_h

#include "common.h"

namespace image_processing {

typedef struct tagRgbQuad {
    BYTE m_cBlue;
    BYTE m_cGreen;
    BYTE m_cRed;
    BYTE m_cReserved;
} RgbQuad, *BitmapQuad;

typedef struct tagBitmapFileHeader {
    WORD  m_sType;               // type of bitmap, should be "BM"
    DWORD m_lSize;               // the size of this bitmap, counted by byte
    WORD  m_sReserved1;          // reserved segment, should be 0
    WORD  m_sReserved2;          // reserved segment, should be 0
    DWORD m_lOffBits;            // the start position of bitmap data, using the offset to file head, counted by byte.
} FileHeader, *BitmapFileHeader; // this data struct takes up 14 bytes.

typedef struct tagBitmapInfoHeader {
    DWORD  m_lSize;               // how many bytes this struct takes.
    DWORD  m_lWidth;              // the width of this bitmap, counted by pixels.
    DWORD  m_lHeight;             // the height of this bitmap, counted by pixels.
    WORD   m_sPlanes;             // this segment should be 1 for bitmap.
    WORD   m_sBitCount;           // how many bits each pixel needs, should be one of {1, 4, 8, 24}
    DWORD  m_lCompression;        // bitmap compressed type, should be 0(not compressed), 1(bi_rle8), 2(bi_rle4)
    DWORD  m_lImageSize;          // the size of this bitmap file, counted by bytes.
    DWORD  m_lXPixelsPerMeter;    // horizontal resolution, pixels per meter
    DWORD  m_lYPixelsPerMeter;    // vertical resolution, pixels per meter
    DWORD  m_lColorUsed;          // how many difference colors this image used
    DWORD  m_lColorImportant;     // the amount of important colors during drawing this image
} InfoHeader, *BitmapInfoHeader;  // this data struct takes up 40 bytes.

typedef struct image {
    FileHeader m_fileHeader;
    InfoHeader m_infoHeader;
    RgbQuad    m_rgbQuad;
} bmpImage, *BmpImage;

typedef struct imageInfo {
    DWORD m_lWidth;
    DWORD m_lHeight;
    WORD  m_sBitCount;
    DWORD m_lBytesPerLine;
    DWORD m_lBmpSize;
    BYTE *m_pszData;
    ~imageInfo() {
        if(m_pszData) {
            delete []m_pszData;
            m_pszData = 0;
        }
    }
} bmpInfo, *BmpInfo;

}; // END namespace image_processing

#endif /* graphics_h */

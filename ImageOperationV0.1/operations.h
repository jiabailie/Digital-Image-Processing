//
//  operations.h
//  ImageProcessing
//
//  Created by Yang Ruiguo on 16/2/19.
//  Copyright © 2019 Yang Ruiguo. All rights reserved.
//

#ifndef operations_h
#define operations_h

#include "common.h"
#include "graphics.h"

namespace image_processing {
    
class Operations {
public:
    STATUS OpenBmpImage(char*, BmpImage&, BmpInfo&);
    STATUS SaveBmpImage(char*, const BmpImage&, const BmpInfo&);
};
    
STATUS Operations::OpenBmpImage(char* pszPath, BmpImage& pImage, BmpInfo& pInfo) {
    int  iRGB = 0;
    DWORD lBmpSize = 0;
    
    FILE* pFile = fopen(pszPath, "rb");
    if(!pFile) {
        printf("File not exist.\n");
        return FAILURE;
    }
    
    // Read the data size of bmp file.
    fseek(pFile, 0, SEEK_END);
    lBmpSize = ftell(pFile);
    rewind(pFile);
    
    // Read the file header of bmp file.
    fread(&(pImage->m_fileHeader), 1, FILEHEADERSIZE, pFile);
    
    // Read the info header of bmp file.
    fread(&(pImage->m_infoHeader), 1, INFOHEADERSIZE, pFile);
    
    // Read the image width.
    pInfo->m_lWidth = pImage->m_infoHeader.m_lWidth;
    
    // Read the image height.
    pInfo->m_lHeight = pImage->m_infoHeader.m_lHeight;
    
    // Read the bit count.
    pInfo->m_sBitCount = pImage->m_infoHeader.m_sBitCount;
    
    switch(pInfo->m_sBitCount) {
        case 1:
            iRGB = 2;
            break;
        case 4:
            iRGB = 16;
            break;
        case 8:
            iRGB = 256;
            break;
        case 24:
            iRGB = 0;
            break;
        default:
            break;
    }
    
    // Read rgb quad.
    fread(&(pImage->m_rgbQuad), iRGB * sizeof(RgbQuad), 1, pFile);
    
    if(pInfo->m_sBitCount != 24) {
        printf("Only support true code bmp image.");
        return FAILURE;
    }
    
    // If image is true color picture.
    pInfo->m_lBmpSize = lBmpSize - FILEHEADERSIZE - INFOHEADERSIZE;
    
    // Allocate the memory for image data.
    pInfo->m_pszData = (BYTE*)malloc(sizeof(BYTE) * pInfo->m_lBmpSize);
    
    // Read bmp data.
    fread(pInfo->m_pszData, 1, pInfo->m_lBmpSize, pFile);
    
    // Calculate the bytes per line.
    pInfo->m_lBytesPerLine = (pInfo->m_sBitCount * pInfo->m_lWidth + 31) / 8;
    
    fclose(pFile);
    
    printf(">>>>>>>>>>> Open File Success.\n\n");
    return SUCCESS;
}

STATUS Operations::SaveBmpImage(char* pszPath, const BmpImage& pImage, const BmpInfo& pInfo) {
    int iBytesPerPixel = 3;
    DWORD lWidth = pInfo->m_lWidth;
    DWORD lHeight = pInfo->m_lHeight;
    
    BYTE pszHeader[BMPHEADERSIZE] = {
        0x42, 0x4d,  0, 0,  0, 0,  0, 0,
        0,       0, 54, 0,  0, 0, 40, 0,
        0,       0,  0, 0,  0, 0,  0, 0,
        0,       0,  1, 0, 24, 0,  0, 0,
        0,       0,  0, 0,  0, 0,  0, 0,
        0,       0,  0, 0,  0, 0,  0, 0,
        0,       0,  0, 0,  0, 0};
    
    DWORD lDataSize = lWidth * lHeight * iBytesPerPixel + BMPHEADERSIZE;
    
    pszHeader[2] = (BYTE)((lDataSize)      &  0x000000ff);
    pszHeader[3] = (BYTE)((lDataSize >> 8) &  0x000000ff);
    pszHeader[4] = (BYTE)((lDataSize >> 16) & 0x000000ff);
    pszHeader[5] = (BYTE)((lDataSize >> 24) & 0x000000ff);
    
    pszHeader[18] = (BYTE)((lWidth)      &  0x000000ff);
    pszHeader[19] = (BYTE)((lWidth >> 8) &  0x000000ff);
    pszHeader[20] = (BYTE)((lWidth >> 16) & 0x000000ff);
    pszHeader[21] = (BYTE)((lWidth >> 24) & 0x000000ff);
    
    pszHeader[22] = (BYTE)((lHeight)       & 0x000000ff);
    pszHeader[23] = (BYTE)((lHeight >> 8)  & 0x000000ff);
    pszHeader[24] = (BYTE)((lHeight >> 16) & 0x000000ff);
    pszHeader[25] = (BYTE)((lHeight >> 24) & 0x000000ff);

    FILE *pFile = fopen(pszPath, "wb");
    if(!pFile) {
        printf("Error: Cannot create the file!\n");
        return FAILURE;;
    }

    fwrite(pszHeader, sizeof(unsigned char), 54, pFile);

    // data size of each row = (image width) * (byte size of each pixel)
    DWORD lBytesPerLine = lWidth * iBytesPerPixel;

    // correct the lBytesPerLine, making it divisible by 4
    while(lBytesPerLine % 4 != 0) {
        lBytesPerLine += 1;
    }

    // image size = (bytes per line) * (image height)
    DWORD lBmpSize = lBytesPerLine * lHeight;

    fwrite(pInfo->m_pszData, sizeof(unsigned char), (size_t)lBmpSize, pFile);
    fclose(pFile);

    printf("Save File Success\n");
    return SUCCESS;
}

}; // END namespace image_processing

#endif /* operations_h */

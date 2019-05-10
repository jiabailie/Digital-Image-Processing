//
//  main.cpp
//  ImageProcessing
//
//  Created by Yang Ruiguo on 14/2/19.
//  Copyright © 2019 Yang Ruiguo. All rights reserved.
//

#include "helper.h"
#include "operations.h"

using namespace image_processing;

int main(int argc, const char * argv[]) {
    char pszPath[] = "/Users/ruiguo.yang/Downloads/sample.bmp";

    Operations operation;
    
    BmpImage pImage = new bmpImage();
    BmpInfo pInfo = new bmpInfo();

    STATUS status1 = operation.OpenBmpImage(pszPath, pImage, pInfo);

    if(status1 == FAILURE) {
        if(pImage) {
            delete pImage;
        }
        if(pInfo) {
            delete pInfo;
        }
        return 0;
    }

    char pszTargetPath[] = "/Users/ruiguo.yang/Downloads/copy";
    operation.SaveBmpImage(pszTargetPath, pImage, pInfo);

    if(pImage) {
        delete pImage;
    }
    if(pInfo) {
        delete pInfo;
    }

    return 0;
}

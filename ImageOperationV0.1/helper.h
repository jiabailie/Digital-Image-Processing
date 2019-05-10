//
//  helper.h
//  ImageProcessing
//
//  Created by Yang Ruiguo on 14/2/19.
//  Copyright © 2019 Yang Ruiguo. All rights reserved.
//

#ifndef helper_h
#define helper_h

#include "common.h"

namespace image_processing {

template<class T>
inline T iabs(T x) {
    return x > 0 ? x : ((-1) * x);
}

template<class T>
inline T imax(T x, T y) {
    return x > y ? x : y;
}

template<class T>
inline T imin(T x, T y) {
    return x < y ? x : y;
}

inline double GetAngle(int gradientX, int gradientY) {
    return 180 * atan(gradientY * 1.0 / gradientX) / PI;
}

}; // END namespace image_processing

#endif /* helper_h */

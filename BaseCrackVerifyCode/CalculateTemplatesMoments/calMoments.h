#ifndef	_CALMOMENT_H_
#define _CALMOMENT_H_

#include "CalculateTemplatesMoments.h"

inline LL calPow(LL base, LL exponent);

inline LL calMpq(int p, int q, int width, int height, BYTE* matrix);

inline double calCenterMoment(int p, int q, int width, int height, BYTE* matrix);

inline void calRegularMoment(int width, int height, BYTE* matrix, double moments[MOMENTS_LENGTH]);

#endif
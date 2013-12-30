#ifndef _SL_HISTOGRAM_VISUALIZATION_H_
#define _SL_HISTOGRAM_VISUALIZATION_H_

#include "cv.h"
#include "sltypes.h"

IplImage *slCreateHistogramImage();
void slCalHistogramImage(const IplImage *src, IplImage *dst, SlBound *bound, sl_int32 interval);



#endif
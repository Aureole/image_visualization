#include "slhistogram_visualization.h"
#include "highgui.h"
#include <stdio.h>

#define SL_HIS_START_X 50
#define SL_HIS_START_Y 50
#define SL_HIS_MAX_WIDTH 3
#define SL_HIS_WIDTH (256 * SL_HIS_MAX_WIDTH + SL_HIS_START_X * 2)
#define SL_HIS_R_HEIGHT 400
#define SL_HIS_HEIGHT (SL_HIS_R_HEIGHT + SL_HIS_START_Y * 2) 

IplImage *slCreateHistogramImage()
{
	IplImage *tmp = cvCreateImage(cvSize(SL_HIS_WIDTH, SL_HIS_HEIGHT), 8, 3);
	tmp->origin = IPL_ORIGIN_BL;
	return tmp;
}

static void slDrawHistogramBoundary(IplImage *img)
{
	sl_int32 i;
	sl_int8 text[256];
	CvFont font;

	cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX ,0.25f,0.25f,0,1,CV_AA);
	cvZero(img);
	cvLine(img
		, cvPoint(SL_HIS_START_X, SL_HIS_START_Y)
		, cvPoint(SL_HIS_WIDTH - SL_HIS_START_X, SL_HIS_START_Y)
		, CV_RGB(255, 255, 255), 1, 8, 0);

	cvLine(img
		, cvPoint(SL_HIS_START_X, SL_HIS_START_Y)
		, cvPoint(SL_HIS_START_X, SL_HIS_HEIGHT - SL_HIS_START_Y)
		, CV_RGB(255, 255, 255), 1, 8, 0);

	for(i = 0; i < 26; i++)
	{
		sprintf(text, "%d", i * 10);
		cvPutText(img, text, cvPoint(SL_HIS_START_X + i * SL_HIS_MAX_WIDTH * 10, SL_HIS_START_Y / 4 * 3), &font, CV_RGB(255, 255, 255));
	}
}

static SlBound *slGetDefaultBoundary(SlBound *bound, const IplImage *img)
{
	assert(bound != sl_null && img != sl_null);
	bound->left = 0;
	bound->top = 0;
	bound->right = img->width - 1;
	bound->bottom = img->height - 1;
	return bound;
}

static sl_int32 slCalSingleChannelHisDim(sl_int32 interval, sl_int32 nchannels)
{
	assert(interval > 0 && nchannels > 0);
	assert(256 % interval == 0);
	return 256 / interval * nchannels;
}

//tmp s
extern int thre_out;
//tmp e

static void slCalSepChannelHistogram(float *histogram, const IplImage *img, sl_int32 *max_value, SlBound *bound, sl_int32 interval)
{
	sl_int32 i, j, k, num, nchannels;
	sl_uint8 *data;
	sl_uint8 value;
	sl_int32 size;
	float sum, avg;
	float preavg;
	assert(histogram != sl_null && img != sl_null && bound != sl_null && max_value != sl_null);
	assert(interval > 0);
	assert(256 % interval == 0);

	nchannels = img->nChannels;
	num = slCalSingleChannelHisDim(interval, nchannels);
	data = (sl_uint8 *)img->imageData + bound->top * img->widthStep;

	for(i = 0; i < num; i++)
	{
		histogram[i] = 0;
	}
	
	for(i = bound->top; i <= bound->bottom; i++)
	{
		for(j = bound->left; j <= bound->right; j++)
		{
			for(k = 0; k < nchannels; k++)
			{
				value = data[j * nchannels + k] / interval;
				histogram[value * nchannels + k] += 1;
			}
		}
		data += img->widthStep;
	}

	size = (bound->right - bound->left + 1) * (bound->bottom - bound->top + 1);
	*max_value = 0;
	//size -= histogram[0];
	//histogram[0] = 0;
	for(i = 0; i < num; i++)
	{
		if(histogram[i] > *max_value)
		{
			*max_value = histogram[i];
		}
	}

	//tmp start
	sum = 0;
	avg = 0;
	for(i = 0; i < num; i++)
	{
		sum += i * histogram[i] / size;
		preavg = avg;
		avg += histogram[i] / size;
		if(avg > 0.95 && avg <= 1)
		{
			printf("pre_avg %f", preavg);
			thre_out = i;
			avg = i;
		}
	}
	printf("sum %f, avg %f \n", sum, avg);

	//tmp e


	for(i = 0; i < num; i++)
	{
		histogram[i] /= *max_value;
	}




}


static void slDrawSepChannelHistogram(float *histogram, IplImage *img, sl_int32 interval, sl_int32 nchannels)
{
	sl_int32 num;
	sl_int32 i;
	const CvScalar colors[3] = {{255, 0,0}, {0, 255, 0}, {0, 0, 255}};
	const CvScalar gray[2] = {{255, 255, 255}, {128, 128, 128}};


	assert(histogram != sl_null && img != sl_null && interval > 0);
	num = slCalSingleChannelHisDim(interval, nchannels);

	if(nchannels == 1)
	{
		for(i = 0; i < num; i++)
		{
			cvLine(img
				, cvPoint(SL_HIS_START_X + 1 + i * SL_HIS_MAX_WIDTH, SL_HIS_START_Y + histogram[i] * SL_HIS_R_HEIGHT)
				, cvPoint(SL_HIS_START_X + 1 + i * SL_HIS_MAX_WIDTH, SL_HIS_START_Y)
				, gray[i % 2], 3 * interval, 8, 0);
		}
	}
	else
	{
		for(i = 0; i < num; i++)
		{
			cvLine(img
				, cvPoint(SL_HIS_START_X + 1 + i * interval, SL_HIS_START_Y + histogram[i] * SL_HIS_R_HEIGHT)
				, cvPoint(SL_HIS_START_X + 1 + i * interval, SL_HIS_START_Y)
				, colors[i % 3], interval, 8, 0);
		}
	}
}

static void slPutMaxValue(IplImage *img, sl_int32 max_value)
{
	CvFont font;
	char text[256];
	cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX ,0.5f,0.5f,0,1,CV_AA);
	sprintf(text, "%d", max_value);
	cvPutText(img, text, cvPoint(SL_HIS_START_X / 2, SL_HIS_HEIGHT - SL_HIS_START_Y / 2), &font, CV_RGB(255, 255, 255));
}

void slCalHistogramImage(const IplImage *src, IplImage *dst, SlBound *bound, sl_int32 interval)
{
	SlBound rbound;
	sl_int32 max_value;
	float histogram[256 * 3];
	assert(src != sl_null && dst != sl_null);
	assert(dst->width == SL_HIS_WIDTH
		&& dst->height == SL_HIS_HEIGHT
		&& dst->nChannels == 3);
	assert(256 % interval == 0);
	slDrawHistogramBoundary(dst);

	if(bound == sl_null)
	{
		slGetDefaultBoundary(&rbound, src);
	}
	else
	{
		rbound = *bound;
	}
	slCalSepChannelHistogram(histogram, src, &max_value, &rbound, interval);
	slDrawSepChannelHistogram(histogram, dst, interval, src->nChannels);
	slPutMaxValue(dst, max_value);
}
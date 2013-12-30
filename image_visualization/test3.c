#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include "slhistogram_visualization.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned int ubool;
#define ufalse 0
#define utrue 1

int thre_out;

static void slGetUpAndDownImage(IplImage *preImage, IplImage *curImage, IplImage *upImage, IplImage *downImage)
{
	int i, j;
	int index_u;
	int index_d;
	int index_c;
	int index_p;
	unsigned char curValue, preValue;
	cvZero(upImage);
	cvZero(downImage);
	for(i = 0; i < preImage->height; i++)
	{
		for(j = 0; j < preImage->width; j++)
		{
			index_u = i * upImage->widthStep + j;
			index_d = i * downImage->widthStep + j;
			index_c = i * curImage->widthStep + j;
			index_p = i * downImage->widthStep + j;

			curValue = ((unsigned char *)curImage->imageData)[index_c];
			preValue = ((unsigned char *)preImage->imageData)[index_p];
			if(curValue > preValue)
			{
				((unsigned char *)upImage->imageData)[index_u] = curValue - preValue;
			}
			else
			{
				((unsigned char *)downImage->imageData)[index_u] = preValue - curValue;
			}
		}
	}
}
int main()
{
	IplImage *img, *image;
	IplImage *red, *green, *blue;
	IplImage *mask;
	IplImage *gray;
	IplImage *pre;
	IplImage *fore;
	IplImage *his;
	IplImage *up, *down;
	SlBound bound;
	uint wait_time = 1;
	int key;
	float threshold = 0;
	float tmp;
	CvCapture *cap = cvCaptureFromFile("D:/fire46.avi");
	if(cap == NULL)
	{
		return -1;
	}

	img = NULL;
	image = cvQueryFrame(cap);
	bound.left = 0;
	bound.top = 240 - 180;
	bound.right = 320;
	bound.bottom = 240;
	cvNamedWindow("color", 1);
	while(1)
	{
		image = cvQueryFrame(cap);
		image = cvQueryFrame(cap);
		image = cvQueryFrame(cap);
		if(image == NULL)
		{
			cvSetCaptureProperty(cap, CV_CAP_PROP_POS_FRAMES, 0);
			image = cvQueryFrame(cap);
		}

		if(img == NULL)
		{
			img = cvCreateImage(cvSize(image->width, image->height), 8, 3);
			pre = cvCreateImage(cvGetSize(img), 8, 1);
			gray = cvCreateImage(cvGetSize(img), 8, 1);
			red = cvCreateImage(cvGetSize(img), 8, 1);
			fore = cvCreateImage(cvGetSize(img), 8, 1);
			green = cvCreateImage(cvGetSize(img), 8, 1);
			up = cvCreateImage(cvGetSize(img), 8, 1);
			down = cvCreateImage(cvGetSize(img), 8, 1);
			blue = cvCreateImage(cvGetSize(img), 8, 1);
			mask = cvCreateImage(cvGetSize(img), 8, 1);
			img->origin = image->origin;
			fore->origin = image->origin;
			his = slCreateHistogramImage();
		}

		//cvSmooth(image, image, CV_MEDIAN, 3, 0, 0, 0);
		cvResize(image, img, 1);
		cvShowImage("color", img);
		cvCvtColor(img, gray, CV_RGB2GRAY);
		cvSplit(img, blue, green, red, NULL);
		cvAbsDiff(red, pre, fore);
		slGetUpAndDownImage(pre, red, up, down);
		//slCalHistogramImage(fore, his, &bound, 1);
		slCalHistogramImage(up, his, sl_null, 1);
		cvNamedWindow("up", 1);
		cvShowImage("up", his);
		slCalHistogramImage(down, his, sl_null, 1);
		cvNamedWindow("down", 1);
		cvShowImage("down", his);

		if(threshold == 0)
		{
			threshold = thre_out;
		}
		else
		{
			threshold = 0.5 * threshold + 0.5 * thre_out;
		}

		//thre_out = max(thre_out, 135);
		//thre_out = min(thre_out, 200);
		if(threshold < 10)
		{
			tmp = 10;//threshold * 2;
		}
		else
		{
			tmp = threshold;
		}
		cvThreshold(fore, fore, tmp, 255, CV_THRESH_BINARY);
		cvNamedWindow("thre", 0);
		cvShowImage("thre", fore);
		cvCopy(red, pre, NULL);


		key = cvWaitKey(wait_time);
		cvNamedWindow("tmp", 1);
		cvShowImage("tmp", his);
		if(key == 27)
		{
			break;
		}
		else if(key == '1')
		{
			wait_time = 100;
		}
		else if(key == 'f')
		{
			wait_time = 1;
		}
		else if(key > 0)
		{
			wait_time = 0;
		}
	}
	cvReleaseCapture(&cap);
	
	return 0;
}
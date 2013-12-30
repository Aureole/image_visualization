#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned int ubool;
#define ufalse 0
#define utrue 1

static uint his[3][352];
static const uint max_index = 352;
static const int base_x = 100;
static const int base_y = 300;
static const uint interval = 2;
static uint index[3];
static IplImage *show; 
static CvPoint org;
static uint choose = ufalse;
static CvPoint cur;

static void CreateHis()
{
	show = cvCreateImage(cvSize(900, 400), 8, 3);
}

static void cvMouseMoveCallback(int mouseEvent,int x,int y,int flags,void* param)
{
	switch(mouseEvent)
	{
	case CV_EVENT_MOUSEMOVE:
		cur.x = x;
		cur.y = y;
		break;
	}
	return;
}

static void ShowHis(int n, CvScalar color)
{
	uint i;
	char text[256];
	CvFont font;
	float mean_diff;
	int diff;
	int max_diff;
	uint pre_index;
	cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX ,0.5f,0.5f,0,1,CV_AA);

	cvZero(show);
	for(i = 0; i < 26; i++)
	{
		cvLine(show, cvPoint(base_x, base_y - i * 10), cvPoint(base_x + interval * max_index, base_y - i * 10), CV_RGB(0, 128, 0), 1, 8, 0);
	}
	cvLine(show, cvPoint(base_x, 40), cvPoint(base_x, base_y), CV_RGB(0,128,0), 1, 8, 0);
	for(i = 1; i < index[n]; i++)
	{
		cvLine(show, 
			cvPoint(base_x + (i - 1) * interval, base_y - his[n][i - 1]),
			cvPoint(base_x + i * interval, base_y - his[n][i]),
			color, 1, 8, 0);
		cvCircle(show, cvPoint(base_x + i * interval, base_y - his[n][i]), 2, color, -1, 8, 0); 
	}

	if(cur.x < base_x)
	{
		cur.x = base_x;
	}
	sprintf(text, "x : %d", (cur.x - base_x) / interval);
	cvPutText(show, text, cvPoint(15, 315), &font, CV_RGB(255, 255, 255));
	sprintf(text, "y : %d", base_y - cur.y);
	cvPutText(show, text, cvPoint(15, 330), &font, CV_RGB(255, 255, 255));


	pre_index = max_index - 1;
	if(index[n] != 0) pre_index = index[n] - 1;

	mean_diff = 0;
	max_diff = 0;
	for(i = 1; i < index[n]; i++)
	{
		diff =  his[n][i] - his[n][i - 1];
		if(diff < 0) diff = 0 - diff;
		mean_diff += diff;
		if(diff > max_diff)
		{
			max_diff = diff;
		}
	}
	mean_diff /= max_index;
	sprintf(text, "diff mean : %.2f", mean_diff);
	cvPutText(show, text, cvPoint(15, 360), &font, CV_RGB(255, 255, 255));
	sprintf(text, "diff max : %d", max_diff);
	cvPutText(show, text, cvPoint(15, 345), &font, CV_RGB(255, 255, 255));
	//sprintf(text, "entropy : %.2f", entropy());
	//cvPutText(show, text, cvPoint(15, 375), &font, CV_RGB(255, 255, 255));


	cvNamedWindow("his", 1);
	cvSetMouseCallback("his", cvMouseMoveCallback, 0);
	cvShowImage("his", show);
	cvWaitKey(1);
}

static CvScalar colors[3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};

static void ShowAllHis()
{
	uint i;
	int n;
	char text[256];
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX ,0.5f,0.5f,0,1,CV_AA);

	cvZero(show);
	for(i = 0; i < 26; i++)
	{
		cvLine(show, cvPoint(base_x, base_y - i * 10), cvPoint(base_x + interval * max_index, base_y - i * 10), CV_RGB(0, 128, 0), 1, 8, 0);
	}
	cvLine(show, cvPoint(base_x, 40), cvPoint(base_x, base_y), CV_RGB(0,128,0), 1, 8, 0);

	for(n = 0; n < 3; n++)
	{
		for(i = 1; i < index[n]; i++)
		{
			cvLine(show, 
				cvPoint(base_x + (i - 1) * interval, base_y - his[n][i - 1]),
				cvPoint(base_x + i * interval, base_y - his[n][i]),
				colors[n], 1, 8, 0);
			cvCircle(show, cvPoint(base_x + i * interval, base_y - his[n][i]), 2, colors[n], -1, 8, 0); 
		}
	}

	if(cur.x < base_x)
	{
		cur.x = base_x;
	}
	sprintf(text, "x : %d", (cur.x - base_x) / interval);
	cvPutText(show, text, cvPoint(15, 315), &font, CV_RGB(255, 255, 255));
	sprintf(text, "y : %d", base_y - cur.y);
	cvPutText(show, text, cvPoint(15, 330), &font, CV_RGB(255, 255, 255));

	cvNamedWindow("his", 1);
	cvSetMouseCallback("his", cvMouseMoveCallback, 0);
	cvShowImage("his", show);
	cvWaitKey(1);
}


static void ResetHis()
{
	index[0] = 0;
	index[1] = 0;
	index[2] = 0;
	memset(his[0], 0, sizeof(uint) * max_index);
	memset(his[1], 0, sizeof(uint) * max_index);
	memset(his[2], 0, sizeof(uint) * max_index);
}

static void SetHisNext(uint value, int n)
{
	if(index[n] != max_index)
	{
		his[n][index[n]] = value;
		index[n]++;
	}
	else
	{
		memcpy(his[n], his[n] + 1, sizeof(uint) * (max_index - 1));
		his[n][max_index - 1] = value;	
	}
}

static void cvMouseCallback(int mouseEvent,int x,int y,int flags,void* param)
{
	switch(mouseEvent)
	{
	case CV_EVENT_LBUTTONDOWN:
		choose = utrue;
		org.x = x;
		org.y = y;
		ResetHis();
		break;
	}
	return;
}


int main2()
{
	IplImage *img, *image;
	uint value;
	uint i, j, k;
	uint wait_time = 1;
	int key;
	CvCapture *cap = cvCaptureFromFile("D:/forest2.avi");
	if(cap == NULL)
	{
		return -1;
	}

	CreateHis();
	img = NULL;
	image = cvQueryFrame(cap);
	ResetHis();
	cvNamedWindow("color", 1);
	cvSetMouseCallback("color",cvMouseCallback,0);
	while(1)
	{
		//image = cvQueryFrame(cap);
		image = cvQueryFrame(cap);
		//cvQueryFrame(cap);
		//cvQueryFrame(cap);
		//cvQueryFrame(cap);
		if(image == NULL)
		{
			cvSetCaptureProperty(cap, CV_CAP_PROP_POS_FRAMES, 0);
			image = cvQueryFrame(cap);
		}

		if(img == NULL)
		{
			//img = cvCreateImage(cvSize(image->width / 2, image->height / 2), 8, 3);
			img = cvCreateImage(cvSize(image->width, image->height), 8, 3);
			img->origin = image->origin;
		}

		cvResize(image, img, 1);
		cvShowImage("color", img);
		key = cvWaitKey(wait_time);
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

		if(choose)
		{
			ResetHis();
			/*j = (img->height - org.y) * img->widthStep;
			for(i = 0; i < img->width; i++)
			{
				value = (uchar)img->imageData[j + i * 3];
				SetHisNext(value, 0);
				value = (uchar)img->imageData[j + i * 3 + 1];
				SetHisNext(value, 1);
				value = (uchar)img->imageData[j + i * 3 + 2];
				SetHisNext(value, 2);
			}*/

			j = org.x;
			for(i = 0; i < img->height; i++)
			{
				value = (uchar)img->imageData[i * img->widthStep + j * 3];
				SetHisNext(value, 0);
				value = (uchar)img->imageData[i * img->widthStep + j * 3 + 1];
				SetHisNext(value, 1);
				value = (uchar)img->imageData[i * img->widthStep + j * 3 + 2];
				SetHisNext(value, 2);
			}
			ShowAllHis();
		}
	}
	cvReleaseCapture(&cap);
	
	return 0;
}
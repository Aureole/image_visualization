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

uint his[3][300];
const uint max_index = 300;
const int base_x = 100;
const int base_y = 300;
const uint interval = 2;
uint index[3];
IplImage *show; 
CvPoint org;
uint choose = ufalse;
CvPoint cur;

void CreateHis()
{
	show = cvCreateImage(cvSize(800, 400), 8, 3);
}

void cvMouseMoveCallback(int mouseEvent,int x,int y,int flags,void* param)
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

/*
float entropy()
{
	float entro[256];
	int i;
	int n;
	float result = 0;;
	for(i = 0; i < 256; i++)
	{
		entro[i] = 0;
	}

	for(i = 0; i < index; i++)
	{
		entro[his[n][i] / 1] += 1;	
	}
	for(i = 0; i < 256; i++)
	{
		if(entro[i] == 0)
		{
			continue;
		}
		entro[i] /= index;
		result -= entro[i] * log(entro[i]) / log(2);
	}

	return result;
}*/

void ShowHis(int n, CvScalar color)
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
		//cvCircle(show, cvPoint(base_x + i, base_y - his[i]), 1, CV_RGB(255, 0, 0), 1, 8, 0); 
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

CvScalar colors[3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};

void ShowAllHis()
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


void ResetHis()
{
	index[0] = 0;
	index[1] = 0;
	index[2] = 0;
	memset(his[0], 0, sizeof(uint) * max_index);
	memset(his[1], 0, sizeof(uint) * max_index);
	memset(his[2], 0, sizeof(uint) * max_index);
}

void SetHisNext(uint value, int n)
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

void cvMouseCallback(int mouseEvent,int x,int y,int flags,void* param)
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


int main()
{
	IplImage *img, *image;
	uint value;
	uint i;
	CvCapture *cap = cvCaptureFromFile("D:/fire21.avi");
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
		if(cvWaitKey(1) == 27)
		{
			break;
		}

		if(choose)
		{
			i = (img->height - org.y) * img->widthStep + org.x * 3;
			value = (uchar)img->imageData[i];
			SetHisNext(value, 0);
			value = (uchar)img->imageData[i + 1];
			SetHisNext(value, 1);
			value = (uchar)img->imageData[i + 2];
			SetHisNext(value, 2);
			ShowAllHis();
		}
	}
	cvReleaseCapture(&cap);
	
	return 0;
}
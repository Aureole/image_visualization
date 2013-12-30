#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "slvisualization.h"
#include "sltypes.h"

/*
sl_int32 inc[4][3] = {{0, 4, 0}, {0, 0, -4}, {4, 0, 0}, {0, -4, 0}};

int main()
{
	int i, j;
	int r, g, b;
	int count = 1;
	FILE *f;
	f = fopen("d://color.txt", "w");
	if(f == NULL)
	{
		return -1;
	}

	fprintf(f, "static sl_uint8 color[256][3] = {{  0,   0, 255}");
	
	r = -1;
	g = -1;
	b = 255;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 64; j++)
		{
			r += inc[i][0];
			g += inc[i][1];
			b += inc[i][2];
			fprintf(f, ", {%3d, %3d, %3d}", max(r, 0), max(g, 0), max(b, 0));
			count++;
			if(count % 8 == 0)
			{
				fprintf(f, "\n                                 ");
			}
		}
	}
	fclose(f);


}*/

static unsigned char slCalAbsPixelGradient( unsigned char upValue
										, unsigned char leftValue
										, unsigned char curValue)
{
	//求横竖两个方向的梯度
	int dx = (int)curValue - (int)leftValue;
	int dy = (int)curValue - (int)upValue;

	//计算强度
	//double square = (dx * dx + dy * dy);
	//return (unsigned char)sqrt(square);
	return (abs(dx) + abs(dy)) / 2;
}


void slCalAbsMagnitudeImage(IplImage* magnitudeImage, IplImage* img)
{
	int i, j;
	unsigned char up, left, cur;
	if(img == NULL || img->nChannels != 1)
	{
		return ;
	}
	if(magnitudeImage == NULL || magnitudeImage->nChannels != 1)
	{
		return ;
	}
	if(magnitudeImage->width != img->width || magnitudeImage->height != img->height)
	{
		return ;
	}

	cvZero(magnitudeImage);
	for(i = 1; i < img->height; i++)
	{
		for(j = 1; j < img->width; j++)
		{
			up = img->imageData[(i - 1) * img->widthStep + j];
			left = img->imageData[i * img->widthStep + j - 1];
			cur = img->imageData[i * img->widthStep + j];

			((unsigned char *)(magnitudeImage->imageData))[i * magnitudeImage->widthStep + j] 
				= slCalAbsPixelGradient(up, left, cur);
		}
	}
	return ;

}

int mainx()
{
	IplImage *img = sl_null;
	IplImage *mag;
	IplImage *mag_pre;
	IplImage *image;
	IplImage *fore_single;
	IplImage *fore;
	IplImage *pre;
	IplImage *rImage, *gImage, *bImage, *gray;
	CvCapture *cap;
	IplImage *showImage;
	int wait_time = 1;
	int wait_value;

	cap = cvCaptureFromFile("d://fire43.avi");
	if(cap == sl_null)
	{
		return -1;
	}

	while(1)
	{
		image = cvQueryFrame(cap);
		image = cvQueryFrame(cap);
		if(image == sl_null)
		{
			cvSetCaptureProperty(cap, CV_CAP_PROP_POS_FRAMES, 0);
			image = cvQueryFrame(cap);
		}

		if(img == sl_null)
		{
			//img = cvCreateImage(cvSize(image->width / 2, image->height / 2), 8, 3);
			img = cvCreateImage(cvGetSize(image), 8, 3);
			pre = cvCreateImage(cvGetSize(img), 8, 3);
			fore = cvCreateImage(cvGetSize(img), 8, 3);
			rImage = cvCreateImage(cvGetSize(img), 8, 1);
			gImage = cvCreateImage(cvGetSize(img), 8, 1);
			bImage = cvCreateImage(cvGetSize(img), 8, 1);
			gray = cvCreateImage(cvGetSize(img), 8, 1);
			mag = cvCreateImage(cvGetSize(img), 8, 1);
			mag_pre = cvCreateImage(cvGetSize(img), 8, 1);
			fore_single = cvCreateImage(cvGetSize(img), 8, 1);
			showImage = cvCreateImage(cvGetSize(img), 8, 3);
			img->origin = image->origin;
			rImage->origin = img->origin;
			gImage->origin = img->origin;
			bImage->origin = img->origin;
			showImage->origin = img->origin;
			gray->origin = img->origin;
			pre->origin = img->origin;
			fore->origin = img->origin;
			mag->origin = img->origin;
			mag_pre->origin = img->origin;
			fore_single->origin = img->origin;
			cvResize(image, img, CV_INTER_LINEAR);
			cvCopy(img, pre, sl_null);
			cvCvtColor(img, gray, CV_RGB2GRAY);
			slCalAbsMagnitudeImage(mag_pre, gray);
			continue;
		}

		cvResize(image, img, CV_INTER_LINEAR);
		cvCvtColor(img, gray, CV_RGB2GRAY);
		cvAbsDiff(img, pre, fore);
		//cvSplit(fore, bImage, gImage, rImage, sl_null);
		cvSplit(img, bImage, gImage, rImage, sl_null);

		slCalAbsMagnitudeImage(mag, gray);
		cvAbsDiff(mag, mag_pre, fore_single);

		cvNamedWindow("mag_fore", 0);
		cvShowImage("mag_fore", fore_single);
		slCalHeatImage(fore_single, showImage);
		cvNamedWindow("mag_fore_heat", 0);
		cvShowImage("mag_fore_heat", showImage);
		//cvNamedWindow("mag", 0);
		//cvShowImage("mag", mag);
		//slCalHeatImage(mag, showImage);
		//cvNamedWindow("magheat", 0);
		//cvShowImage("magheat", showImage);
		cvNamedWindow("diff", 0);
		cvShowImage("diff", fore);
		slCalHeatImage(rImage, showImage);
		cvNamedWindow("rheat", 0);
		cvShowImage("rheat", showImage);
		slCalHeatImage(bImage, showImage);
		cvNamedWindow("bheat", 0);
		cvShowImage("bheat", showImage);
		slCalHeatImage(gImage, showImage);
		cvNamedWindow("gheat", 0);
		cvShowImage("gheat", showImage);
		slCalHeatImage(gray, showImage);
		cvNamedWindow("grayheat", 0);
		cvShowImage("grayheat", showImage);
		cvNamedWindow("img", 0);
		cvShowImage("img", img);
		cvCopy(img, pre, sl_null);
		cvCopy(mag, mag_pre, sl_null);
		
		wait_value = cvWaitKey(wait_time);
		if(wait_value == 'f')
		{
			wait_time = 1;
		}
		else if(wait_value == 27)
		{
			break;
		}
		else if(wait_value == '0')
		{
			wait_time = 0;
		}
		else if(wait_value ==  '1')
		{
			wait_time = 100;
		}
		else if(wait_value > 0)
		{
			wait_time = 1000;
		}

	}

	if(img != sl_null)
	{
		cvReleaseImage(&img);
		cvReleaseImage(&rImage);
		cvReleaseImage(&gImage);
		cvReleaseImage(&bImage);
		cvReleaseImage(&showImage);
		cvReleaseImage(&gray);
		cvReleaseImage(&pre);
		cvReleaseImage(&fore);
	}
	cvReleaseCapture(&cap);
	return 0;
}
/*
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "highgui.lib")
int Otsu(IplImage* src);

int main()
{
	int threshold;
	int i, j, ii, jj;
	int minValue, maxValue, curValue, value;
	int minThreshold, minV;
	float tmpThreshold;
	IplImage* img = cvLoadImage("d:\\5.jpg",1);
	IplImage* dst = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *blue = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *green = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *red = cvCreateImage(cvGetSize(img), 8, 1);
	cvSplit(img, blue, green, red, NULL);
	cvSmooth(blue, blue, CV_BLUR, 3, 0, 0, 0);
	threshold = Otsu(blue);
	//threshold = 200;
	printf("threshold = %d\n", threshold);
	//cvThreshold(blue, dst, threshold, 255, CV_THRESH_BINARY);

	cvZero(dst);
	minThreshold = 255;
	for(i = 0; i < blue->height; i++)
	{
		for(j = 0; j < blue->width; j++)
		{
			curValue = ((unsigned char *)blue->imageData)[i * blue->widthStep + j];
			maxValue = 0;
			minValue = 255;
			for(ii = i - 1; ii <= i + 1; ii++)
			{
				if(ii < 0)
				{
					continue;
				}
				for(jj = j - 1; jj <= j + 1; jj++)
				{
					if(jj < 0 || (ii == i && jj == j))
					{
						continue;
					}

					value = ((unsigned char *)blue->imageData)[ii * blue->widthStep + jj];
					if(value > maxValue)
					{
						maxValue = value;
					}
					if(value < minValue)
					{
						minValue = value;
					}
				}
			}
			if(maxValue - minValue < minThreshold)
			{
				minThreshold = maxValue - minValue;
				minV = minValue;
			}
			//tmpThreshold = threshold / 255.0 * (maxValue - minValue) + minValue;
			//if(curValue > tmpThreshold)
			//{
			//	((unsigned char *)dst->imageData)[i * dst->widthStep + j] = 255;
			//}
			
		}
	}
	cvThreshold(blue, dst, threshold / 255.0 * minThreshold + minV, 255, CV_THRESH_BINARY);


	cvNamedWindow( "img", 1 );
	cvShowImage("img", dst);


	cvWaitKey(-1);

	cvReleaseImage(&img);
	cvReleaseImage(&dst);
	
	cvDestroyWindow( "dst" );
	return 0;
}

int Otsu(IplImage* src)  
{  
	int height=src->height;  
	int width=src->width;      
	int i, j;
	unsigned char *p;
	int size;

	float avgValue=0;  
	int threshold;    
	float maxVariance=0;  
	float w = 0, u = 0;  
	float t, variance;

	//histogram  
	float histogram[256] = {0};  
	for(i=0; i < height; i++)
	{  
		p=(unsigned char*)src->imageData + src->widthStep * i;  
		for(j = 0; j < width; j++) 
		{  
			histogram[*p++]++;  
		}  
	}  
	//normalize histogram  
	size = height * width;  
	for(i = 0; i < 256; i++)
	{  
		histogram[i] = histogram[i] / size;  
	}  

	//average pixel value  
	for(i=0; i < 256; i++)
	{  
		avgValue += i * histogram[i];  //整幅图像的平均灰度
	}   

	for(i = 0; i < 256; i++) 
	{  
		w += histogram[i];  //假设当前灰度i为阈值, 0~i 灰度的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例
		u += i * histogram[i];  // 灰度i 之前的像素(0~i)的平均灰度值： 前景像素的平均灰度值

		t = avgValue * w - u;  
		variance = t * t / (w * (1 - w) );  
		if(variance > maxVariance)
		{  
			maxVariance = variance;  
			threshold = i;  
		}  
	}  

	return threshold;  
} 
*/

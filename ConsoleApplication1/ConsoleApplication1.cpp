// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <stdio.h>

IplImage* doCanny (IplImage* in, double lowThresh,double highThresh,double aperture)
{
	if(in->nChannels!=1)
	{
		printf("in do Canny: channels!=1\n");
		return 0;
	}
	IplImage * out = cvCreateImage(cvGetSize(in),IPL_DEPTH_8U,1);

	cvCanny(in,out,(double)lowThresh,(double)highThresh,(double)aperture);
	return out;
}

// 内轮廓填充 
// 参数: 
// 1. pBinary: 输入二值图像，单通道，位深IPL_DEPTH_8U。
// 2. dAreaThre: 面积阈值，当内轮廓面积小于等于dAreaThre时，进行填充。 
// 3. appThre: 多边形近似阈值
IplImage* FillInternalContours(IplImage *pBinary, double dAreaThre, double appThre = 2) 
{ 
	double dConArea; 
	CvSeq *pContour = NULL; 
	CvSeq *pConInner = NULL; 
	CvMemStorage *pStorage = NULL; 
	IplImage* res = NULL;
	// 执行条件 
	if (pBinary) 
	{ 
		res = cvCreateImage(cvGetSize(pBinary),IPL_DEPTH_8U,3);
		cvSetZero(res);
		// 查找所有轮廓 
		pStorage = cvCreateMemStorage(0); 
		cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); 
		cvApproxPoly(pContour,sizeof(CvContour),pStorage,CV_POLY_APPROX_DP,appThre,1);
		// 填充所有轮廓 
		cvDrawContours(res, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
		// 外轮廓循环 
		for (; pContour != NULL; pContour = pContour->h_next) 
		{ 
			// 内轮廓循环 
			for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next) 
			{ 
				// 内轮廓面积 
				dConArea = fabs(cvContourArea(pConInner, CV_WHOLE_SEQ)); 
				if (dConArea <= dAreaThre) 
				{ 
					cvDrawContours(res, pConInner, CV_RGB(255,100,0),CV_RGB(255,100,0), 0, CV_FILLED, 8, cvPoint(0, 0));
				} 
			} 
		} 
		cvReleaseMemStorage(&pStorage); 
		pStorage = NULL; 
	} 
	return res;
} 

int _tmain(int argc, _TCHAR* argv[])
{
	IplImage * img = cvLoadImage("C:\\Users\\Clover27\\Desktop\\cv\\1.jpg");

	IplImage * grey = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);

	cvCvtColor(img,grey,CV_RGB2GRAY);


	IplImage* out3 = doCanny(grey,100,200,3);

	IplImage* outc = cvCloneImage(out3);

	cvNamedWindow("in",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("result",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("grey",CV_WINDOW_AUTOSIZE);

	CvMemStorage *mem = cvCreateMemStorage();
	CvSeq * con = NULL;



	int cnum = cvFindContours(out3,mem,&con,88,CV_RETR_TREE);

	printf("con num : %d\n",cnum);
	/*
	IplImage* co1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	cvSetZero(co1);
	cvDrawContours(co1,con,cvScalarAll(100),cvScalarAll(255),0);

	IplImage* co2 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	cvSetZero(co2);
	cvDrawContours(co2,con,cvScalarAll(100),cvScalarAll(255),1);

	cvShowImage("co1",co1);
	cvShowImage("co2",co2);
	*/
	CvSeq * app = cvApproxPoly(con,sizeof(CvContour),mem,CV_POLY_APPROX_DP,1,1);

	IplImage* co1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
	cvSetZero(co1);
	cvDrawContours(co1,app,CV_RGB(10,100,200),CV_RGB(255,100,0),100,1);
	cvShowImage("co1",co1);

	IplImage* res = FillInternalContours(outc,300);

	cvShowImage("fill",res);

	cvShowImage("grey",grey);
	cvShowImage("in",img);
	cvShowImage("result",out3);



	//cvSaveImage("C:\\Users\\Clover27\\Desktop\\res.jpg",out3);

	cvWaitKey(0);

	cvReleaseImage(&grey);
	cvReleaseImage(&out3);
	cvReleaseImage(&img);



	return 0;
}
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "denoise.h"

using namespace std;

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

/*
IplImage* FillInternalContours(IplImage *pBinary, double dAreaThre, double appThre = 2,int low =50,int high =200)
{
    double dConArea;
    CvSeq *pContour = NULL;
    CvSeq *pConInner = NULL;
    CvMemStorage *pStorage = NULL;
    IplImage* res = NULL;
    if (pBinary)
    {
        IplImage * grey = cvCreateImage(cvGetSize(pBinary),IPL_DEPTH_8U,1);
        
        cvCvtColor(pBinary,grey,CV_RGB2GRAY);
        
        
        IplImage* out = doCanny(grey,low,high,3);
        
        res = cvCreateImage(cvGetSize(pBinary),IPL_DEPTH_8U,3);
        cvSetZero(res);
        // ≤È’“À˘”–¬÷¿™
        pStorage = cvCreateMemStorage(0);
        cvFindContours(out, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        cvApproxPoly(pContour,sizeof(CvContour),pStorage,CV_POLY_APPROX_DP,appThre,1);
        // ÃÓ≥‰À˘”–¬÷¿™
        cvDrawContours(res, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
        // Õ‚¬÷¿™—≠ª∑
        for (; pContour != NULL; pContour = pContour->h_next)
        {
            // ƒ⁄¬÷¿™—≠ª∑
            for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
            {
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
}*/

//传入灰度或二值图像
IplImage* func(IplImage* img, double maxarea = 3000,int low=100,int high=200)
{
    
    
    IplImage* out3 = doCanny(img,low,high,3);
    
    CvMemStorage *mem = cvCreateMemStorage();
    
    IplImage* co1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
    cvSetZero(co1);

    //画
    CvContourScanner scanner = NULL;
    scanner = cvStartFindContours(out3,mem,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE,cvPoint(0,0));
    //开始遍历轮廓树
    CvRect rect;
    CvSeq* contour = NULL;

    while (contour=cvFindNextContour(scanner))
    {
        CvRect rect = cvBoundingRect(contour,0);
        double tmparea = rect.height*rect.width;
        rect = cvBoundingRect(contour,0);
        bool f = 1;
        int ry;
        ry = rect.y+rect.height/2;
        if(ry<img->height/3)
            f=0;
        if (tmparea < maxarea && tmparea > 40 && rect.width>5 && rect.height >5 && f)
        {
            
            cvDrawContours(co1, contour,CV_RGB(255,100,0),CV_RGB(255,100,0),0,2);
            
        }
        else
        {
            CvSeq* ac = cvApproxPoly(contour, sizeof(CvContour), mem, CV_POLY_APPROX_DP, 3);
            cvDrawContours(co1, ac,CV_RGB(10,100,200),CV_RGB(10,100,200),0,2);
        }
        
    }
    cvReleaseMemStorage(&mem);
    cvReleaseImage(&out3);
    return co1;

}

/*****主函数
参数:
    imgpath:图像路径
    thre:阈值化阈值
    denoise_area:噪点外接矩形面积最大值
    body_maxarea:人形外接矩形面积最小值
 */
IplImage* scanImage(char* imgpath, int thre=125,int denoise_area = 400 ,int body_maxarea =3000)
{
    if(imgpath == NULL)
    {
        printf("imgpath = NULL!\n");
        return NULL;
    }
    if(thre <0 || thre >255)
    {
        printf("thre invalid!\n");
        return NULL;
    }
    IplImage * img = cvLoadImage(imgpath); //打开
    
    IplImage * grey = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);//转灰度
    
    cvCvtColor(img,grey,CV_RGB2GRAY);//转灰度
    
    IplImage* gt =cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);//储存阈值化图像
    
    cvThreshold(grey, gt, thre, 255, CV_THRESH_BINARY); //阈值化
    
    IplImage* dimg2 = denoise2(gt,denoise_area); //降噪
    
    IplImage* b=cvCreateImage(cvSize(dimg2->width+10,dimg2->height+10), IPL_DEPTH_8U, 1);//储存加边
    
    cvCopyMakeBorder(dimg2, b, cvPoint(5, 5), IPL_BORDER_CONSTANT,CV_RGB(255, 255, 255));//加边
    
    IplImage* bto = cvCreateImage(cvGetSize(b), IPL_DEPTH_8U, 1);//储存开运算
    
    cvMorphologyEx(b, bto, NULL,NULL ,CV_MOP_OPEN,1);//开运算
    
    IplImage * btoc = func(bto,body_maxarea); //轮廓处理

    cvReleaseImage(&grey);
    cvReleaseImage(&gt);
    cvReleaseImage(&dimg2);
    cvReleaseImage(&img);
    cvReleaseImage(&b);
    cvReleaseImage(&bto);
    //cvReleaseImage(&btoc);
    return btoc;
}

int main()
{
    char path[100];
    
    for (int i = 1; i<=64; i++) {
        sprintf(path, "/Users/Clover/Desktop/1/%d.bmp",i);

        IplImage* src = cvLoadImage(path);
        IplImage* res = scanImage(path);
        cvNamedWindow("res");
        cvNamedWindow("src");
        cvShowImage("res", res);
        cvShowImage("src", src);
        cvWaitKey(0);
        cvReleaseImage(&res);
        cvReleaseImage(&src);
        
        
    }
}






//
//  Header.h
//  test_opencv
//
//  Created by 晟昱 陈 on 15/7/23.
//  Copyright (c) 2015年 晟昱 陈. All rights reserved.
//

#ifndef test_opencv_denoise_h
#define test_opencv_denoise_h

/*
IplImage* denoise(IplImage* img,double areaThre)
{
    IplImage *img_contour = cvCreateImage(cvGetSize(img),8,1);
    IplImage *img_contour_3 = cvCreateImage(cvGetSize(img),8,3);
    cvCopy(img,img_contour);
    cvCvtColor(img,img_contour_3,CV_GRAY2BGR);
    CvMemStorage *storage= cvCreateMemStorage();
    CvSeq * first_contour = NULL;
    int Nc = cvFindContours(
                            img_contour,
                            storage,
                            &first_contour,
                            sizeof(CvContour),
                            CV_RETR_LIST
                            );
    int num_contours = 0;
    struct min_max_list *head = NULL;
    double area = 0;
    //double areaOfImg = img->width *img->height;
    CvSeq *cPrev = first_contour;
    CvSeq *cNext = NULL;
    bool first= true;
    for(CvSeq *c = first_contour;c!= NULL;c= cNext)
    {
        
        area = fabs(cvContourArea(c) );
        if(area <areaThre)
        {
            cNext = c->h_next;
            cvClearSeq(c);
            //cvClearMemStorage(c->storage);//回收内存
            continue;
        }//if
        else
        {
            if(first)
                first_contour = c;
            first = false;
            
            cvDrawContours(img_contour_3,c,CV_RGB(255, 100, 0),CV_RGB(255, 100, 0),0,1,8);
        }
        cNext= c->h_next;
    }//for first_constours;
    return img_contour_3;
}*/

IplImage* denoise2(IplImage* img_src,double minarea)
{
    CvSeq* contour = NULL;
    double tmparea = 0.0;
        CvMemStorage* storage = cvCreateMemStorage(0);
        IplImage* img_Clone=cvCloneImage(img_src);
        //访问二值图像每个点的值
        uchar *pp;
        //------------搜索二值图中的轮廓，并从轮廓树中删除面积小于某个阈值minarea的轮廓-------------//
        CvContourScanner scanner = NULL;
        scanner = cvStartFindContours(img_src,storage,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE,cvPoint(0,0));
        //开始遍历轮廓树
        CvRect rect;
        while ((contour=cvFindNextContour(scanner)))
        {
            //tmparea = fabs(cvContourArea(contour));
            rect = cvBoundingRect(contour,0);
            tmparea = rect.width*rect.height;
            if (tmparea < minarea/*||tmparea>4900*/)
            {
                if ((tmparea-0.0)<0.01) {
                    CvSeq* hn = contour->h_next;
                    CvSeq* hp = contour->h_prev;
                    CvSeq* vn = contour->v_next;
                    CvSeq* vp = contour->v_prev;
                    if(hp!=NULL )
                      
                    hp->h_next = hn;
                    if(hn!=NULL)
                    hn->h_prev = hp;
                    if(vp!=NULL)
                    vp->v_next = vn;
                    if(vn!=NULL)
                    vn->v_prev = vp;
                    
                    //free(contour);
                    for(int y = rect.y;y<rect.y+rect.height;y++)
                    {
                        for(int x =rect.x;x<rect.x+rect.width;x++)
                        {
                            pp=(uchar*)(img_Clone->imageData + img_Clone->widthStep*y+x);
                            
                            if (pp[0]==0)
                            {
                                pp[0]=255;
                            }
                        }
                    }
                    
                    continue;
                }
                //当连通域的中心点为黑色时，而且面积较小则用白色进行填充
                pp=(uchar*)(img_Clone->imageData + img_Clone->widthStep*(rect.y+rect.height/2)+rect.x+rect.width/2);
                if (pp[0]!=255)
                {
                    for(int y = rect.y;y<rect.y+rect.height;y++)
                    {
                        for(int x =rect.x;x<rect.x+rect.width;x++)
                        {
                            pp=(uchar*)(img_Clone->imageData + img_Clone->widthStep*y+x);
                            
                            if (pp[0]==0)
                            {
                                pp[0]=255;
                            }
                        }   
                    }
                }   
                
            }
        }
    cvReleaseMemStorage(&storage);
    return img_Clone;
}
#endif


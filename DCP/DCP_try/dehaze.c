/*dehaze.cpp 
 *
 *Author Huaijin
 *Email  huaijin511@gmail.com
 *
*/
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <string>
#define PATCH 15
IplImage*  GetDarkChannel(IplImage* src,int patchSize){
	int block = patchSize;
	//spilt RGB
	IplImage* imageR = NULL;
	IplImage* imageG = NULL;
	IplImage* imageB = NULL;
	IplImage* darkChannel = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);

	int addP = (block-1)/2;

	imageR = cvCreateImage(cvSize(src->width+addP*2,src->height+addP*2),IPL_DEPTH_8U,1);
	imageG = cvCreateImage(cvSize(src->width+addP*2,src->height+addP*2),IPL_DEPTH_8U,1);
	imageB = cvCreateImage(cvSize(src->width+addP*2,src->height+addP*2),IPL_DEPTH_8U,1);

	cvSet(imageR,cvScalar(255));
	cvSet(imageG,cvScalar(255));
	cvSet(imageB,cvScalar(255));

	cvSetImageROI(imageR,cvRect(addP,addP,src->width,src->height));
	cvSetImageROI(imageG,cvRect(addP,addP,src->width,src->height));
	cvSetImageROI(imageB,cvRect(addP,addP,src->width,src->height));

	cvSplit(src,imageR,imageG,imageB,NULL);
	cvResetImageROI(imageR);
	cvResetImageROI(imageG);
	cvResetImageROI(imageB);

	//get the min values from R G B int the same pacth & save these values to a IplImage prt
	CvRect pacth = cvRect(0,0,block,block);

	double min=0;
	double min1=0;
	double max1=0;
	double min2=0;
	double max2=0;
	double min3=0;
	double max3=0;

	int i,j;
	for(i=0;i<src->height;i++)
	{
		for(j=0;j<src->width;j++)
		{
			cvSetImageROI(imageR,pacth);
			cvMinMaxLoc(imageR,&min1,&max1,NULL,NULL);

			cvSetImageROI(imageG,pacth);
			cvMinMaxLoc(imageG,&min2,&max2,NULL,NULL);

			cvSetImageROI(imageB,pacth);
			cvMinMaxLoc(imageB,&min3,&max3,NULL,NULL);

			min = min1 < min2 ? min1:min2;
			min = min < min3 ? min:min3;

			cvResetImageROI(imageR);
			cvResetImageROI(imageG);
			cvResetImageROI(imageB);

			cvSet2D(darkChannel,i,j,cvScalar(min));

			//next pixl
			pacth.x = j;
			pacth.y = i;

		}
	}
	cvNamedWindow("darkChannel",CV_WINDOW_AUTOSIZE);
	cvShowImage("darkchannel",darkChannel);
	cvWaitKey(0);
	cvSaveImage("./dark_channel_prior.jpg",darkChannel);
	cvReleaseImage(&imageR);
	cvReleaseImage(&imageG);
	cvReleaseImage(&imageB);
	cvDestroyWindow( "darkChannel" );
	return darkChannel;
}

//IplImage* SoftMatting_DC(IplImage* dc){}
double AL_R = 0;
double AL_G = 0;
double AL_B = 0;
CvScalar  EAL(IplImage* imageR,IplImage* imageG,IplImage* imageB)
{
	double temp = 0;
	cvMinMaxLoc(imageR,&temp,&AL_R,NULL,NULL);
	cvMinMaxLoc(imageG,&temp,&AL_G,NULL,NULL);
	cvMinMaxLoc(imageB,&temp,&AL_B,NULL,NULL);
	printf("----%f ---%f---%f--\n",AL_R,AL_G,AL_B);
	return cvScalar(AL_R,AL_G,AL_B);
}



IplImage*  ET(IplImage* DC)
{
	IplImage* transmission = cvCreateImage(cvSize(DC->width,DC->height),IPL_DEPTH_8U,1);

	double w = 0.95;
	double AL_max =0;
	AL_max = AL_R >= AL_G ? AL_R:AL_G;
	AL_max = AL_max >= AL_B ? AL_max:AL_B;

	CvScalar m,n;
	int i,j;
	for(i=0;i<DC->height;i++)
	{
		for(j=0;j<DC->width;j++)
		{
			m = cvGet2D(DC,i,j);
			n = cvScalar((1-w*m.val[0]/AL_max)*255);
			cvSet2D(transmission,i,j,n);
		}
	}
	cvNamedWindow("Transmission",CV_WINDOW_AUTOSIZE);
	cvShowImage("Transmission",transmission);
	cvWaitKey(0);
	cvSaveImage("./transMission.jpg",transmission);
	cvDestroyWindow( "Transmission" );
	return transmission;


}


//esstime J(x)
IplImage* EJ(IplImage* src,CvScalar A,IplImage* transmission)
{
	double t_min = 0.1;
	//printf("CVMAXS----\n");
	IplImage* J = cvCreateImage(cvSize(src->width,src->height),src->depth,3);
	int i,j,k;
	CvScalar tP,jP,iP;
	for( i = 0; i < src->height; i++ )
	{
		for( j = 0; j < src->width; j++ )
		{
			for( k = 0; k<3; k++)
			{
				tP = cvGet2D(transmission,i,j);
				iP = cvGet2D(src,i,j);
				tP.val[0] = tP.val[0]/255 < t_min ? t_min: tP.val[0]/255;
				jP.val[k] =( iP.val[k] - A.val[k] )/tP.val[0]+ A.val[k];
			}
			cvSet2D(J,i,j,jP);
		}
	}

	cvSaveImage("./result.jpg",J);
	cvNamedWindow("j(x)",CV_WINDOW_AUTOSIZE);
	cvShowImage("j(x)",J);
	cvWaitKey(0);
	printf("----huaijian----\n");
	cvDestroyWindow("j(x)");
	return J;

}

//void RSR(){}

int main(int argc,char** argv)
{
	IplImage* src = cvLoadImage(argv[1]);

	IplImage* darkChannel = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);
	IplImage* transmission = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_32F,1);
	IplImage* J = cvCreateImage(cvSize(src->width,src->height),src->depth,3);
	IplImage* imageR = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);
	IplImage* imageG = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);
	IplImage* imageB = cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);
	cvSplit(src,imageR,imageG,imageB,NULL);
	//recove the image without haze.
	darkChannel =  GetDarkChannel(src,3);
	CvScalar AirLigth =EAL(imageR, imageG, imageB);
	transmission = ET(darkChannel);
	J = EJ(src, AirLigth, transmission);

	printf("-----mian :release source--------\n");
	cvReleaseImage(&src);
	cvReleaseImage(&darkChannel);
	cvReleaseImage(&transmission);
	cvReleaseImage(&J);
	cvReleaseImage(&imageR);
	cvReleaseImage(&imageG);
	cvReleaseImage(&imageB);

	return 0;
}



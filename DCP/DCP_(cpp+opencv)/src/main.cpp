/*
	The main function is an example of video dehazing 
	The core algorithm is in "dehazing.cpp," "guidedfilter.cpp," and "transmission.cpp". 
	You may modify the code to improve the results.

	The detailed description of the algorithm is presented
	in "http://mcl.korea.ac.kr/projects/dehazing". See also 
	J.-H. Kim, W.-D. Jang, Y. Park, D.-H. Lee, J.-Y. Sim, C.-S. Kim, "Temporally
	coherent real-time video dehazing," in Proc. IEEE ICIP, 2012.

	Last updated: 2013-02-14
	Author: Jin-Hwan, Kim.
 */

#include "dehazing.h"
#include <time.h>
#include <conio.h>

int main(int argc, char** argv)
{	
	CvCapture* cvSequence = cvCaptureFromFile(argv[1]);

	int nWid = (int)cvGetCaptureProperty(cvSequence,CV_CAP_PROP_FRAME_WIDTH); //atoi(argv[3]);
	int nHei = (int)cvGetCaptureProperty(cvSequence,CV_CAP_PROP_FRAME_HEIGHT); //atoi(argv[4]);

	cv::VideoWriter vwSequenceWriter(argv[2], 0, 25, cv::Size(nWid, nHei), true);
	
	IplImage *imInput;
	IplImage *imOutput = cvCreateImage(cvSize(nWid, nHei),IPL_DEPTH_8U, 3);

	int nFrame;

	dehazing dehazingImg(nWid, nHei, 16, false, false, 5.0f, 1.0f, 40);

	time_t start_t = clock();

	for( nFrame = 0; nFrame < atoi(argv[3]); nFrame++ )
	{
		imInput = cvQueryFrame(cvSequence);

		dehazingImg.HazeRemoval(imInput,imOutput,nFrame);
		
		vwSequenceWriter.write(imOutput);
	}

	cout << nFrame <<" frames " << (float)(clock()-start_t)/CLOCKS_PER_SEC << "secs" <<endl;

	getch();

	cvReleaseCapture(&cvSequence); 
 	cvReleaseImage(&imOutput);
	
	return 0;
}

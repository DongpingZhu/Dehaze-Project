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
/*
#include "dehazing.h"
#include <time.h>
#include <conio.h>

int main(int argc, char** argv)
{	
	IplImage *imInput = cvLoadImage(argv[1], 1);
	
	int nWid = imInput->width;
	int nHei = imInput->height;
	
	IplImage *imOutput = cvCreateImage(cvSize(nWid, nHei),IPL_DEPTH_8U, 3);

	dehazing dehazingImg(nWid, nHei, 30, false, false, 5.0f, 1.0f, 40);

	//dehazingImg.ImageHazeRemoval(imInput, imOutput);
	//dehazingImg.ImageHazeRemovalYUV(imInput, imOutput);
	dehazingImg.ImageHazeRemoval(imInput, imOutput);
		
	cvSaveImage(argv[2], imOutput);

	_getch();

	cvReleaseImage(&imInput); 
 	cvReleaseImage(&imOutput);
	
	return 0;
}
*/

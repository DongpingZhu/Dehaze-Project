/*
	This source file contains additional function for dehazing algorithm.

	The detailed description of the algorithm is presented
	in "http://mcl.korea.ac.kr/projects/dehazing". See also 
	J.-H. Kim, W.-D. Jang, Y. Park, D.-H. Lee, J.-Y. Sim, C.-S. Kim, "Temporally
	coherent real-time video dehazing," in Proc. IEEE ICIP, 2012.

	Last updated: 2013-02-06
	Author: Jin-Hwan, Kim.
 */

#include "dehazing.h"

/* 
	Function: IplImageToInt
	Description: Convert the opencv type IplImage to integer

	Parameters: 
		imInput - input IplImage
	Return:
		m_pnYImg - output integer array
*/
void dehazing::IplImageToInt(IplImage* imInput)
{
	int nY, nX;
	int nStep;

	nStep = imInput->widthStep;

	for(nY=0; nY<m_nHei; nY++)
	{
		for(nX=0; nX<m_nWid; nX++)
		{
			// (1) IplImage 를 YUV의 Y채널로 변환 하여 int형 배열 m_pnYImg에 저장
			m_pnYImg[nY*m_nWid+nX] =
				(19595 * (uchar)imInput->imageData[nY*nStep+nX*3+2]
			+ 38469 * (uchar)imInput->imageData[nY*nStep+nX*3+1] 
			+ 7471 * (uchar)imInput->imageData[nY*nStep+nX*3]) >> 16;
		}
	}
}

/* 
	Function: IplImageToIntColor
	Description: Convert the opencv type IplImage to integer (3 arrays)

	Parameters: 
		imInput - input IplImage
	Return:
		m_pnRImg - output integer arrays R
		m_pnGImg - output integer arrays G
		m_pnBImg - output integer arrays B
*/
void dehazing::IplImageToIntColor(IplImage* imInput)
{
	int nY, nX;
	int nStep;

	nStep = imInput->widthStep;

	for(nY=0; nY<m_nHei; nY++)
	{
		for(nX=0; nX<m_nWid; nX++)
		{
			m_pnBImg[nY*m_nWid+nX] = (uchar)imInput->imageData[nY*nStep+nX*3];
			m_pnGImg[nY*m_nWid+nX] = (uchar)imInput->imageData[nY*nStep+nX*3+1];
			m_pnRImg[nY*m_nWid+nX] = (uchar)imInput->imageData[nY*nStep+nX*3+2];
		}
	}
}

/* 
	Function: DownsampleImage
	Description: Downsample the image to fixed sized image (320 x 240)

	Parameters:(hidden) 
		m_pnYImg - input Y Image
	Return:
		m_pnSmallYImg - output down sampled image
*/
void dehazing::DownsampleImage()
{
	int nX, nY;

	float fRatioY, fRatioX;
	// 다운샘플링 비율 결정
	fRatioX = (float)m_nWid/(float)320;
	fRatioY = (float)m_nHei/(float)240;

	for(nY=0; nY<240; nY++)
	{
		for(nX=0; nX<320; nX++)
		{
			// (1) 멤버 변수인 m_pnYImg를 m_pnSmallYImg로 다운샘플링(크기는 320x240)
			m_pnSmallYImg[nY*320+nX] = m_pnYImg[(int)(nY*fRatioY)*m_nWid+(int)(nX*fRatioX)];
		}
	}
}

/* 
	Function: DownsampleImageColor
	Description: Downsample the image to fixed sized image (320 x 240) ** for color

	Parameters:(hidden) 
		m_pnRImg - input R Image
		m_pnGImg - input G Image
		m_pnBImg - input B Image
	Return:
		m_pnSmallRImg - output down sampled image
		m_pnSmallGImg - output down sampled image
		m_pnSmallBImg - output down sampled image
*/
void dehazing::DownsampleImageColor()
{
	int nX, nY;

	float fRatioY, fRatioX;
	// 다운샘플링 비율 결정
	fRatioX = (float)m_nWid/(float)320;
	fRatioY = (float)m_nHei/(float)240;

	for(nY=0; nY<240; nY++)
	{
		for(nX=0; nX<320; nX++)
		{
			// (1) 멤버 변수인 m_pnYImg를 m_pnSmallYImg로 다운샘플링(크기는 320x240)
			m_pnSmallRImg[nY*320+nX] = m_pnRImg[(int)(nY*fRatioY)*m_nWid+(int)(nX*fRatioX)];
			m_pnSmallGImg[nY*320+nX] = m_pnGImg[(int)(nY*fRatioY)*m_nWid+(int)(nX*fRatioX)];
			m_pnSmallBImg[nY*320+nX] = m_pnBImg[(int)(nY*fRatioY)*m_nWid+(int)(nX*fRatioX)];
		}
	}
}


/* 
	Function: UpsampleImage
	Description: upsample the fixed sized transmission to original size

	Parameters:(hidden) 
		m_pfSmallTransR - input transmission (320 x 240)
	Return:
		m_pfTransmission - output transmission 
		
*/
void dehazing::UpsampleTransmission()
{
	int nX, nY;

	float fRatioY, fRatioX;
	// 업샘플링 비율 결정
	fRatioX = (float)320/(float)m_nWid;
	fRatioY = (float)240/(float)m_nHei;

	for(nY=0; nY<m_nHei; nY++)
	{
		for(nX=0; nX<m_nWid; nX++)
		{
			// (1) 멤버 변수인 m_pfSmallTransR를 m_pfTransmission로 업샘플링
			m_pfTransmission[nY*m_nWid+nX] = m_pfSmallTrans[(int)(nY*fRatioY)*320+(int)(nX*fRatioX)];
		}
	}
}

/* 
	Function: MakeExpLUT
	Description: Make a Look Up Table(LUT) for applying previous information.

	Return:
		m_pfExpLUT - output table 
		
*/
void dehazing::MakeExpLUT()
{
	int nIdx;

	for ( nIdx = 0 ; nIdx < 256; nIdx++ )
	{
		m_pfExpLUT[nIdx] = exp(-(float)(nIdx*nIdx)/10.0f);
	}
}

/* 
	Function: GuideLUTMaker
	Description: Make a Look Up Table(LUT) for guided filtering

	Return:
		m_pfGuidedLUT - output table
		
*/
void dehazing::GuideLUTMaker()
{
	int nX, nY;

	for ( nX = 0 ; nX < m_nGBlockSize/2; nX++ )
	{
		for ( nY = 0 ; nY < m_nGBlockSize/2 ; nY++ )
		{
			m_pfGuidedLUT[nY*m_nGBlockSize+nX]=(exp(-((nX-m_nGBlockSize/2+1)*(nX-m_nGBlockSize/2+1)+(nY-m_nGBlockSize/2+1)*(nY-m_nGBlockSize/2+1))/(2*m_fGSigma*m_fGSigma)));
			m_pfGuidedLUT[(m_nGBlockSize-nY-1)*m_nGBlockSize+nX]=(exp(-((nX-m_nGBlockSize/2+1)*(nX-m_nGBlockSize/2+1)+(nY-m_nGBlockSize/2+1)*(nY-m_nGBlockSize/2+1))/(2*m_fGSigma*m_fGSigma)));
			m_pfGuidedLUT[nY*m_nGBlockSize+m_nGBlockSize-nX-1]=(exp(-((nX-m_nGBlockSize/2+1)*(nX-m_nGBlockSize/2+1)+(nY-m_nGBlockSize/2+1)*(nY-m_nGBlockSize/2+1))/(2*m_fGSigma*m_fGSigma)));
			m_pfGuidedLUT[(m_nGBlockSize-nY-1)*m_nGBlockSize+m_nGBlockSize-nX-1]=(exp(-((nX-m_nGBlockSize/2+1)*(nX-m_nGBlockSize/2+1)+(nY-m_nGBlockSize/2+1)*(nY-m_nGBlockSize/2+1))/(2*m_fGSigma*m_fGSigma)));
		}
	}
}
/* 
	Function: GammaLUTMaker
	Description: Make a Look Up Table(LUT) for gamma correction

	parameter:
		fParameter - gamma value.
	Return:
		m_pfGuidedLUT - output table
		
*/
void dehazing::GammaLUTMaker(float fParameter)
{
	int nIdx;

	for(nIdx=0; nIdx<256; nIdx++)
	{
		m_pucGammaLUT[nIdx] = (uchar)(pow((float)nIdx/255, fParameter)*255.0f);
	}
}
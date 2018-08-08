#include "dehazing.h"

//////////////////////////////////////////////////////////////////////////
// 기능 : 생성자
// 동작
//		(1) 입력 파라미터에 맞추어 dehazing 클래스 생성
// 입력
//		- nW : 너비
//		- nH : 높이
//		- bPrevFlag : 이전 프레임 사용여부
//		- bPosFlag : 후처리 사용여부
// 초기값 세팅
//		- m_fLambda1 : 5.0f (손실 cost를 조절하는 사용자 변수)
//		- m_fLambda2 : 1.0f (이전 프레임에대한 시간적 cost를 조절하는 사용자 변수)
//		- m_nTBlockSize : 16 (Transmission 블록 크기)
//		- m_nGBlockSize : 40 (Guided Filter에서 사용되는 block 크기)
//		- m_nStepSize : 2 (guided filter의 step size)
//		- m_fGSigma : 10.0f (Look-Up Table 용 파라미터)
//		- m_nTopLeftX,Y & m_nBottomRightX,Y : Airlight 검출 범위
//////////////////////////////////////////////////////////////////////////

dehazing::dehazing(int nW, int nH, bool bPrevFlag, bool bPosFlag)
{
	// (1) 입력 파라미터에 맞추어 dehazing 클래스 생성
	// nW : 너비
	m_nWid			= nW;
	// nH : 높이
	m_nHei			= nH;

	// bPrevFlag : 이전 프레임 사용여부
	m_bPreviousFlag	= bPrevFlag;
	// bPosFlag : 후처리 사용여부
	m_bPostFlag		= bPosFlag;

	// m_fLambda1 : 5.0f (손실 cost를 조절하는 사용자 변수)
	m_fLambda1		= 5.0f;
	// m_fLambda2 : 1.0f (이전 프레임에대한 시간적 cost를 조절하는 사용자 변수)
	m_fLambda2		= 1.0f;

	// m_nTBlockSize : 16 (Transmission 블록 크기)
	m_nTBlockSize		= 16;

	// m_nGBlockSize : 40 (Guided Filter에서 사용되는 block 크기)
	m_nGBlockSize		= 40;
	// m_nStepSize : 2 (guided filter의 step size)
	m_nStepSize			= 2;
	// m_fGSigma : 10.0f (Look-Up Table 용 파라미터)
	m_fGSigma			= 10.0f;

	// m_nTopLeftX,Y & m_nBottomRightX,Y : Airlight 검출 범위
	m_nTopLeftX		= 0;
	m_nTopLeftY		= 0;
	m_nBottomRightX	= m_nWid;
	m_nBottomRightY	= m_nHei;

	// 동적 메모리 할당
	m_pfSmallTransP	= new float [320*240];
	m_pfSmallTrans	= new float [320*240];
	m_pfSmallTransR	= new float [320*240];
	m_pnSmallYImg		= new int [320*240];
	m_pnSmallYImgP	= new int [320*240];
	m_pfSmallInteg	= new float[320*240];
	m_pfSmallDenom	= new float[320*240];
	m_pfSmallY		= new float[320*240];

	m_pfTransmission	= new float [m_nWid*m_nHei];
	m_pfTransmissionR = new float[m_nWid*m_nHei];
	m_pnYImg			= new int [m_nWid*m_nHei];
	m_pfInteg			= new float[m_nWid*m_nHei];
	m_pfDenom			= new float[m_nWid*m_nHei];
	m_pfY				= new float[m_nWid*m_nHei];

	m_pfSmallPk_p		= new float[m_nGBlockSize*m_nGBlockSize];
	m_pfSmallNormPk	= new float[m_nGBlockSize*m_nGBlockSize];
	m_pfPk_p			= new float[m_nGBlockSize*m_nGBlockSize];
	m_pfNormPk		= new float[m_nGBlockSize*m_nGBlockSize];	
	m_pfGuidedLUT		= new float[m_nGBlockSize*m_nGBlockSize];	
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 생성자
// 동작
//		(1) 입력 파라미터에 맞추어 dehazing 클래스 생성
// 입력
//		- nW : 너비
//		- nH : 높이
//		- bPrevFlag : 이전 프레임 사용여부
//		- bPosFlag : 후처리 사용여부
//		- m_fLambda1 : 손실 cost를 조절하는 사용자 변수
//		- m_fLambda2 : 이전 프레임에대한 시간적 cost를 조절하는 사용자 변수
// 초기값 세팅
//		- m_nTBlockSize : 16 (Transmission 블록 크기)
//		- m_nGBlockSize : 40 (Guided Filter에서 사용되는 block 크기)
//		- m_nStepSize : 2 (guided filter의 step size)
//		- m_fGSigma : 10.0f (Look-Up Table 용 파라미터)
//		- m_nTopLeftX,Y & m_nBottomRightX,Y : Airlight 검출 범위
//////////////////////////////////////////////////////////////////////////
dehazing::dehazing(int nW, int nH, bool bPrevFlag, bool bPosFlag, float fL1, float fL2)
{
	// (1) 입력 파라미터에 맞추어 dehazing 클래스 생성
	// nW : 너비
	m_nWid			= nW;
	// nH : 높이
	m_nHei			= nH;

	// bPrevFlag : 이전 프레임 사용여부
	m_bPreviousFlag	= bPrevFlag;
	// bPosFlag : 후처리 사용여부
	m_bPostFlag		= bPosFlag;

	// m_fLambda1 : 손실 cost를 조절하는 사용자 변수
	m_fLambda1		= fL1;
	// m_fLambda2 : 이전 프레임에대한 시간적 cost를 조절하는 사용자 변수
	m_fLambda2		= fL2;

	// m_nTBlockSize : 16 (Transmission 블록 크기)
	m_nTBlockSize		= 16;

	// m_nGBlockSize : 40 (Guided Filter에서 사용되는 block 크기)
	m_nGBlockSize		= 40;
	// m_nStepSize : 2 (guided filter의 step size)
	m_nStepSize			= 2;
	// m_fGSigma : 10.0f (Look-Up Table 용 파라미터)
	m_fGSigma			= 10.0f;

	// m_nTopLeftX,Y & m_nBottomRightX,Y : Airlight 검출 범위
	m_nTopLeftX		= 0;
	m_nTopLeftY		= 0;
	m_nBottomRightX	= m_nWid;
	m_nBottomRightY	= m_nHei;

	// 동적 메모리 할당
	m_pfSmallTransP		= new float [320*240];
	m_pfSmallTrans		= new float [320*240];
	m_pfSmallTransR		= new float [320*240];
	m_pnSmallYImg		= new int [320*240];
	m_pnSmallYImgP		= new int [320*240];
	m_pfSmallInteg		= new float[320*240];
	m_pfSmallDenom		= new float[320*240];
	m_pfSmallY			= new float[320*240];

	m_pfTransmission	= new float [m_nWid*m_nHei];
	m_pfTransmissionR	= new float[m_nWid*m_nHei];
	m_pnYImg			= new int [m_nWid*m_nHei];
	m_pfInteg			= new float[m_nWid*m_nHei];
	m_pfDenom			= new float[m_nWid*m_nHei];
	m_pfY				= new float[m_nWid*m_nHei];

	m_pfSmallPk_p		= new float[m_nGBlockSize*m_nGBlockSize];
	m_pfSmallNormPk		= new float[m_nGBlockSize*m_nGBlockSize];
	m_pfPk_p			= new float[m_nGBlockSize*m_nGBlockSize];
	m_pfNormPk			= new float[m_nGBlockSize*m_nGBlockSize];	
	m_pfGuidedLUT		= new float[m_nGBlockSize*m_nGBlockSize];	
}

dehazing::~dehazing(void)
{
	delete []m_pfSmallInteg;
	delete []m_pfSmallDenom;
	delete []m_pfSmallNormPk;
	delete []m_pfSmallPk_p;
	delete []m_pfSmallY;

	delete []m_pfSmallTransP;
	delete []m_pfSmallTrans;
	delete []m_pfSmallTransR;
	delete []m_pnSmallYImg;
	delete []m_pnSmallYImgP;

	if(m_pnYImg != NULL)
		delete []m_pnYImg; 
	if(m_pfTransmission != NULL)
		delete []m_pfTransmission;

	m_pfTransmission	= NULL;
	m_pfSmallTransP	= NULL;
	m_pfSmallTrans	= NULL;
	m_pfSmallTransR	= NULL;
	m_pnSmallYImg		= NULL;
	m_pnSmallYImgP	= NULL;
	m_pnYImg			= NULL;

	m_pfSmallInteg	= NULL;
	m_pfSmallDenom	= NULL;
	m_pfSmallPk_p		= NULL;
	m_pfSmallNormPk	= NULL;
	m_pfSmallY		= NULL;

	delete []m_pfInteg;
	delete []m_pfDenom;
	delete []m_pfNormPk;
	delete []m_pfPk_p;
	delete []m_pfY;
	delete []m_pfGuidedLUT;

	m_pfInteg			= NULL;
	m_pfDenom			= NULL;
	m_pfPk_p			= NULL;
	m_pfNormPk		= NULL;
	m_pfY				= NULL;
	m_pfGuidedLUT		= NULL;
}

//////////////////////////////////////////////////////////////////////////
// 기능 : IplImage를 int형 배열에 입력
// 동작
//		(1) IplImage 를 YUV의 Y채널로 변환 하여 int형 배열 m_pnYImg에 저장
// 입력
//		- imInput : 입력영상
// 출력
//		- m_pnYImg :  int로 변환된 영상(private 멤버 변수)
//////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////
// 기능 : 하늘영역에 대해 안개 값 검출 수행
// 동작
//		(1) 입력 영상 4분할
//		(2) 입력 영상의 넓이가 200보다 작으면 (255-r, 255-g, 255-b) 가 가장 작은 픽셀(가장 밝은 값) 선택
//
//		(3) 4분할 된 영상에서 각각 stddev(표준편차) 와 mean(평균)값 계산
//		(4) 대표값 = mean - stddev 계산 및 4분할 된 영상의 대표값 비교하여 가장 큰 대표값 선택
//		(5) 선택된 영상을 가지고 함수 재귀호출
// 입력
//		- imInput : 입력영상
// 출력
//		- m_anAirlight : 검출된 3채널 (RGB) 안개값
// 참고 :	재귀 함수
//			
//////////////////////////////////////////////////////////////////////////
void dehazing::AirlightEstimation(IplImage* imInput)
{
	// Distance를 구하기 위한 변수 값
	int nMinDistance = 65536;
	int nDistance;
	// iteration용 변수
	int nX, nY;
	// mean, std-dev 변수
	int nMaxIndex;
	double dpScore[3];
	double dpMean[3];
	double dpStds[3];
	float afMean[4] = {0};
	float afScore[4] = {0};
	float nMaxScore = 0;

	// 입력 너비, 높이, 3채널을 위한 widthstep
	int nW = imInput->width;
	int nH = imInput->height;

	int nStep = imInput->widthStep;

	// (1) 입력 영상 4분할
	// Image를 4분할 (upper left, upper right, lower left, lower right)하기 위한 변수
	IplImage *iplUpperLeft = cvCreateImage(cvSize(nW/2, nH/2),IPL_DEPTH_8U, 3);
	IplImage *iplUpperRight = cvCreateImage(cvSize(nW/2, nH/2),IPL_DEPTH_8U, 3);
	IplImage *iplLowerLeft = cvCreateImage(cvSize(nW/2, nH/2),IPL_DEPTH_8U, 3);
	IplImage *iplLowerRight = cvCreateImage(cvSize(nW/2, nH/2),IPL_DEPTH_8U, 3);

	IplImage *iplR = cvCreateImage(cvSize(nW/2, nH/2),IPL_DEPTH_8U, 1);
	IplImage *iplG = cvCreateImage(cvSize(nW/2, nH/2),IPL_DEPTH_8U, 1);
	IplImage *iplB = cvCreateImage(cvSize(nW/2, nH/2),IPL_DEPTH_8U, 1);

	// Image 4분할
	cvSetImageROI(imInput, cvRect(0, 0, nW/2, nH/2));
	cvCopyImage(imInput, iplUpperLeft);
	cvSetImageROI(imInput, cvRect(nW/2+nW%2, 0, nW, nH/2));
	cvCopyImage(imInput, iplUpperRight);
	cvSetImageROI(imInput, cvRect(0, nH/2+nH%2, nW/2, nH));
	cvCopyImage(imInput, iplLowerLeft);
	cvSetImageROI(imInput, cvRect(nW/2+nW%2, nH/2+nH%2, nW, nH));
	cvCopyImage(imInput, iplLowerRight);

	// (0) 입력 영상의 넓이가 200보다 작으면 (255-r, 255-g, 255-b) 가 가장 작은 픽셀(가장 밝은 값) 선택
	// 크기 측정 너비 * 높이의 넓이가 200이 넘을 경우 분할
	if(nH*nW > 200)
	{
		// (3) 4분할 된 영상에서 각각 stddev(표준편차) 와 mean(평균)값 계산
		cvCvtPixToPlane(iplUpperLeft, iplR, iplG, iplB, 0);

		// dpMean : 평균
		// dpStds : 표준편차
		cvMean_StdDev(iplR, dpMean, dpStds);
		cvMean_StdDev(iplG, dpMean+1, dpStds+1);
		cvMean_StdDev(iplB, dpMean+2, dpStds+2);
		// dpScore: 대표값 = 평균 - 표준편차
		dpScore[0] = dpMean[0]-dpStds[0];
		dpScore[1] = dpMean[1]-dpStds[1];
		dpScore[2] = dpMean[2]-dpStds[2];

		afScore[0] = (float)(dpScore[0]+dpScore[1]+dpScore[2]);

		// 최대값
		nMaxScore = afScore[0];
		nMaxIndex = 0;

		cvCvtPixToPlane(iplUpperRight, iplR, iplG, iplB, 0);

		// dpMean : 평균
		// dpStds : 표준편차
		cvMean_StdDev(iplR, dpMean, dpStds);
		cvMean_StdDev(iplG, dpMean+1, dpStds+1);
		cvMean_StdDev(iplB, dpMean+2, dpStds+2);
		// dpScore: 대표값 = 평균 - 표준편차
		dpScore[0] = dpMean[0]-dpStds[0];
		dpScore[1] = dpMean[1]-dpStds[1];
		dpScore[2] = dpMean[2]-dpStds[2];

		afScore[1] = (float)(dpScore[0]+dpScore[1]+dpScore[2]);
		// 최대값 추출
		if(afScore[1] > nMaxScore)
		{
			nMaxScore = afScore[1];
			nMaxIndex = 1;
		}

		cvCvtPixToPlane(iplLowerLeft, iplR, iplG, iplB, 0);

		// dpMean : 평균
		// dpStds : 표준편차
		cvMean_StdDev(iplR, dpMean, dpStds);
		cvMean_StdDev(iplG, dpMean+1, dpStds+1);
		cvMean_StdDev(iplB, dpMean+2, dpStds+2);
		// dpScore: 대표값 = 평균 - 표준편차
		dpScore[0] = dpMean[0]-dpStds[0];
		dpScore[1] = dpMean[1]-dpStds[1];
		dpScore[2] = dpMean[2]-dpStds[2];

		afScore[2] = (float)(dpScore[0]+dpScore[1]+dpScore[2]);
		// 최대값 추출
		if(afScore[2] > nMaxScore)
		{
			nMaxScore = afScore[2];
			nMaxIndex = 2;
		}

		cvCvtPixToPlane(iplLowerRight, iplR, iplG, iplB, 0);

		// dpMean : 평균
		// dpStds : 표준편차
		cvMean_StdDev(iplR, dpMean, dpStds);
		cvMean_StdDev(iplG, dpMean+1, dpStds+1);
		cvMean_StdDev(iplB, dpMean+2, dpStds+2);
		// dpScore: 대표값 = 평균 - 표준편차
		dpScore[0] = dpMean[0]-dpStds[0];
		dpScore[1] = dpMean[1]-dpStds[1];
		dpScore[2] = dpMean[2]-dpStds[2];

		afScore[3] = (float)(dpScore[0]+dpScore[1]+dpScore[2]);

		// 최대값 추출
		if(afScore[3] > nMaxScore)
		{
			nMaxScore = afScore[3];
			nMaxIndex = 3;
		}

		// (4) 대표값 = mean - stddev 계산 및 4분할 된 영상의 대표값 비교하여 가장 큰 대표값 선택
		// 최대값을 가진 블록 선택
		switch (nMaxIndex)
		{
		case 0:
			// (5) 선택된 영상을 가지고 함수 재귀호출
			AirlightEstimation(iplUpperLeft); break;
		case 1:
			// (5) 선택된 영상을 가지고 함수 재귀호출
			AirlightEstimation(iplUpperRight); break;
		case 2:
			// (5) 선택된 영상을 가지고 함수 재귀호출
			AirlightEstimation(iplLowerLeft); break;
		case 3:
			// (5) 선택된 영상을 가지고 함수 재귀호출
			AirlightEstimation(iplLowerRight); break;
		}
	}
	// (0) 입력 영상의 넓이가 200보다 작으면 (255-r, 255-g, 255-b) 가 가장 작은 픽셀(가장 밝은 값) 선택
	else
	{
		for(nY=0; nY<nH; nY++)
		{
			for(nX=0; nX<nW; nX++)
			{
				// 255-r, 255-g, 255-b의 거리계산
				nDistance = int(sqrt(float(255-(uchar)imInput->imageData[nY*nStep+nX*3])*float(255-(uchar)imInput->imageData[nY*nStep+nX*3])
					+float(255-(uchar)imInput->imageData[nY*nStep+nX*3+1])*float(255-(uchar)imInput->imageData[nY*nStep+nX*3+1])
					+float(255-(uchar)imInput->imageData[nY*nStep+nX*3+2])*float(255-(uchar)imInput->imageData[nY*nStep+nX*3+2])));
				if(nMinDistance > nDistance)
				{
					// 255-r, 255-g, 255-b의 거리로 가장 밝은 RGB 픽셀 검색
					nMinDistance = nDistance;
					m_anAirlight[0] = (uchar)imInput->imageData[nY*nStep+nX*3];
					m_anAirlight[1] = (uchar)imInput->imageData[nY*nStep+nX*3+1];
					m_anAirlight[2] = (uchar)imInput->imageData[nY*nStep+nX*3+2];
				}
			}
		}
	}
	cvReleaseImage(&iplUpperLeft);
	cvReleaseImage(&iplUpperRight);
	cvReleaseImage(&iplLowerLeft);
	cvReleaseImage(&iplLowerRight);

	cvReleaseImage(&iplR);
	cvReleaseImage(&iplG);
	cvReleaseImage(&iplB);
}


//////////////////////////////////////////////////////////////////////////
// 기능 : YImage를 320, 240영상으로 다운샘플링
// 동작
//		(1) 멤버 변수인 m_pnYImg를 m_pnSmallYImg로 다운샘플링(크기는 320x240)
// 입력
//		- m_pnYImg : 입력영상(멤버변수)
// 출력
//		- m_pnSmallYImg : 출력영상(멤버변수)
//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
// 기능 : 320x240크기의 transmission(전달량)을  원래 영상 크기로 복원
// 동작
//		(1) 멤버 변수인 m_pfSmallTransR를 m_pfTransmission로 업샘플링
// 입력
//		- m_pfSmallTransR : 입력영상(멤버변수)
// 출력
//		- m_pfTransmission : 출력영상(멤버변수)
//////////////////////////////////////////////////////////////////////////
void dehazing::UpsampleImage()
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
			m_pfTransmission[nY*m_nWid+nX] = m_pfSmallTransR[(int)(nY*fRatioY)*320+(int)(nX*fRatioX)];
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 전달량(Transmission) 검출 알고리즘
// 동작
//		(0) 320x240 영상에 대해 블록 단위(nBlocksize)의 트랜스미션 검출 수행
//		(1) 이전 프레임 데이터를 사용할것인지 확인하여 사용하지 않을 경우(또는 첫 프레임일 경우)
//		(2-1) 프레임 마다 개별적으로 전달량 계산
//		(2-2) 이전 프레임의 데이터를 이용하여 전달량 계산
//		(3) 전달량 계산 함수 호출(블록시작 지점을 넘겨줌)
//		(4) 계산된 전달량을 저장
// 입력
//		- nFrame : 초기 프레임을 검출하기 위한 변수
//		- m_pnSmallYImg : 입력 Y영상 (멤버변수)
// 출력
//		- pfSmallTrans : 검출된 전달량(멤버변수)
//			
//////////////////////////////////////////////////////////////////////////
void dehazing::TransmissionEstimation(int nFrame)
{
	int nX, nY, nXstep, nYstep;
	float fTrans;

	// (1) 이전 프레임 데이터를 사용할것인지 확인
	if(m_bPreviousFlag == true&&nFrame>0)
	{
		// 이전 프레임 데이터 사용시 동작
		// (0) 320x240 영상에 대해 블록 단위(nBlocksize)의 트랜스미션 검출 수행
		for(nY=0; nY<240; nY+=m_nTBlockSize)
		{
			for(nX=0; nX<320; nX+=m_nTBlockSize)
			{
				// (2-2) 이전 프레임의 데이터를 이용하여 전달량 계산
				// (3)전달량 계산 함수 호출
				fTrans = NFTrsEstimationP(__max(nX, 0), __max(nY, 0));
				for(nYstep=nY; nYstep<nY+m_nTBlockSize; nYstep++)
				{
					for(nXstep=nX; nXstep<nX+m_nTBlockSize; nXstep++)
					{
						//		(4) 계산된 전달량을 저장
						m_pfSmallTrans[nYstep*320+nXstep] = fTrans;
					}
				}
			}
		}
	}
	else
	{
		// 초기 프레임일경우 또는 이전 프레임데이터 사용하지 않을 경우
		// (0) 320x240 영상에 대해 블록 단위(nBlocksize)의 트랜스미션 검출 수행
		for(nY=0; nY<240; nY+=m_nTBlockSize)
		{
			for(nX=0; nX<320; nX+=m_nTBlockSize)
			{
				// (2-1) 프레임 마다 개별적으로 전달량 계산
				// (3)전달량 계산 함수 호출
				fTrans = NFTrsEstimation(__max(nX, 0), __max(nY, 0));

				for(nYstep=nY; nYstep<nY+m_nTBlockSize; nYstep++)
				{
					for(nXstep=nX; nXstep<nX+m_nTBlockSize; nXstep++)
					{
						//		(4) 계산된 전달량을 저장
						m_pfSmallTrans[nYstep*320+nXstep] = fTrans;
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 전달량(Transmission) 계산 알고리즘
// 동작
//		(1) 지정된 블록에서 전달량 계산
//		(2) 전달량을 0.3~1 까지 0.1씩 이동해가면서 영상을 복원
//		(3) 복원된 영상의 variance 및 loss 계산
//		(4) -variance 및 loss를 최소로 하는 전달량 검출
//		(5) 계산된 전달량을 리턴
// 입력
//		- nStartX : 블록의 첫 위치(x)
//		- nStartY : 블록의 첫 위치(y)
// 출력
//		- fOptTrs : 최적의 전달량
//////////////////////////////////////////////////////////////////////////
float dehazing::NFTrsEstimation(int nStartX, int nStartY)
{
	int nCounter;	
	int nX, nY;		
	int nEndX;
	int nEndY;

	int nOut;						// 복원 영상 값
	int nSquaredOut;				// 복원 영상의 제곱
	int nSumofOuts;					// 복원 영상의 합
	int nSumofSquaredOuts;			// 복원 영상 제곱의 합
	float fTrans, fOptTrs;			// fTrans : 전달량(nCounter에 따라 증가), fOptTrs : 최적 전달량 (리턴값)
	int nTrans;						// 정수화 시킨 전달량
	int nSumofSLoss;				// nSumofLoss 복원 영상의 loss 합
	float fCost, fMinCost, fMean;	// fCost : 비용함수 계산, fMinCost : 비용함수 최소값, fMean:복원 영상의 평균값
	int nNumberofPixels, nLossCount;// nNumberofPixels : 블록 크기, nLossCount: 손실 픽셀 개수

	nEndX = __min(nStartX+m_nTBlockSize, 320); // 블록의 끝점 (x)
	nEndY = __min(nStartY+m_nTBlockSize, 240); // 블록의 끝점 (y)

	nNumberofPixels = (nEndY-nStartY)*(nEndX-nStartX);	// 블록 크기 계산

	fTrans = 0.3f;	// 초기 전달량 값 0.3 부터 시작
	nTrans = 427;//1280;

	//	(2) 전달량을 0.3~1 까지 0.1씩 이동해가면서 영상을 복원
	for(nCounter=0; nCounter<7; nCounter++)
	{
		// 초기화 cumulative variables
		nSumofSLoss = 0;
		nLossCount = 0;
		nSumofSquaredOuts = 0;
		nSumofOuts = 0;
		// (1) 지정된 블록에서 전달량 계산
		for(nY=nStartY; nY<nEndY; nY++)
		{
			for(nX=nStartX; nX<nEndX; nX++)
			{
				// (3) 복원된 영상의 variance 및 loss 계산
				nOut = ((m_pnSmallYImg[nY*320+nX] - m_nAirlight)*nTrans + 128*m_nAirlight)>>7;		// 전달량에 따른 복원 픽셀값 계산 (I-A)/t + A를 --> ((I-A)*k*128 + A*128)/128 꼴로 변경하여 속도향상
				nSquaredOut = nOut * nOut;	// 제곱 값 저장

				// 손실 값 계산 (255보다 큰 손실)
				if(nOut>255)
				{
					nSumofSLoss += (nOut - 255)*(nOut - 255);
					nLossCount++;
				}
				else if(nOut < 0)
				{
					// 손실 값 계산 (0보다 작은 손실)
					nSumofSLoss += nSquaredOut;
					nLossCount++;
				}
				// cumulate
				nSumofSquaredOuts += nSquaredOut;
				nSumofOuts += nOut;
			}
		}
		fMean = (float)(nSumofOuts)/(float)(nNumberofPixels);  // 평균 계산
		fCost = m_fLambda1 * (float)nSumofSLoss/(float)(nNumberofPixels) // 손실 계산
			- ((float)nSumofSquaredOuts/(float)nNumberofPixels - fMean*fMean); // 대조비 계산(분산 variance)
		// (4) -variance 및 loss를 최소로 하는 전달량 검출
		// 최적의 전달량 검색
		if(nCounter==0 || fMinCost > fCost)
		{
			fMinCost = fCost;
			fOptTrs = fTrans;
		}

		fTrans += 0.1f;
		nTrans = (int)(1.0f/fTrans*128.0f);
	}
	// (5) 계산된 전달량을 리턴
	return fOptTrs; // 리턴
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 전달량(Transmission) 계산 알고리즘
// 동작
//		(1) 지정된 블록에서 이전 프레임의 전달량 업데이트(밝기 변화에 따른 전달량 보정)
//		(2) 지정된 블록에서 전달량 계산
//		(3) 전달량을 0.3~1 까지 0.1씩 이동해가면서 영상을 복원
//		(4) 복원된 영상의 variance 및 loss 계산
//		(5) -variance, loss, 이전 프레임간의 차이를 최소로 하는 전달량 검출
//		(6) 계산된 전달량을 리턴
// 입력
//		- nStartX : 블록의 첫 위치(x)
//		- nStartY : 블록의 첫 위치(y)
// 출력
//		- fOptTrs : 최적의 전달량
//////////////////////////////////////////////////////////////////////////
float dehazing::NFTrsEstimationP(int nStartX, int nStartY)
{
	int nCounter;	// for find out transmission 0.1~1.0, 10 iteration 
	int nX, nY;		// variable for index
	int nEndX;
	int nEndY;

	float fMean;

	int nOut;								// 복원 영상 값
	float fPreTrs;							// 이전 블록의 전달량				
	int nSquaredOut;						// 복원 영상의 제곱
	int nSumofOuts;							// 복원 영상 값의 합
	int nSumofSquaredOuts;					// 복원 영상 제곱의 합
	int nTrans;								// 정수화한 전달량	
	int nSumofSLoss;						// 손실의 합
	float fCost, fMinCost, fTrans, fOptTrs;	// fCost: 비용함수, fMinCost: 최소의 비용함수, fTrans: 전달량(nCounter에 따라 증가), fOptTrs(최적의 전달량)
	int nNumberofPixels;					// nNumberofPixels: 블록의 크기

	nEndX = __min(nStartX+m_nTBlockSize, 320); // 블록의 끝점 (x)
	nEndY = __min(nStartY+m_nTBlockSize, 240); // 블록의 끝점 (y)

	nNumberofPixels = (nEndY-nStartY)*(nEndX-nStartX);	// 블록 크기 계산

	fTrans = 0.3f;	// initial transmission value
	nTrans = 427;	//1280;
	fPreTrs = 0;

	float fNewKSum = 0;						// 가중치 적용한 kappa 합 (트랜스 미션 변화량)
	float fNewK;							// 가중치 적용한 새로운 Kappa값
	float fWi;								// 가중치
	float fPreJ;							// 이전 프레임 데이터(에러처리 위해)
	float fWsum = 0;						// 가중치 합
	int nIdx = 0;	
	int nLossCount;
	// (1) 지정된 블록에서 이전 프레임의 전달량 업데이트(밝기 변화에 따른 전달량 보정)
	for(nY=nStartY; nY<nEndY; nY++)
	{
		for(nX=nStartX; nX<nEndX; nX++)
		{
			fPreJ = (float)(m_pnSmallYImgP[nY*320+nX]-m_nAirlight);	// 에러 처리위한 계산(분모가 0이되는 상황을 피하기 위함. 분모가 0일경우 계산하지 않음)
			if(fPreJ != 0){
				fWi = m_pfExpLUT[abs(m_pnSmallYImg[nY*320+nX]-m_pnSmallYImgP[nY*320+nX])];//가중치 계산
				fWsum += fWi;	
				fNewKSum += fWi*(float)(m_pnSmallYImg[nY*320+nX]-m_nAirlight)/fPreJ;
			}
		}
	}
	fNewK = fNewKSum/fWsum;			// 새 kappa 계산
	fPreTrs = m_pfSmallTransP[nStartY*320+nStartX]*fNewK;	// kappa 적용한 이전프레임의 전달량 갱신

	// (3) 전달량을 0.3~1 까지 0.1씩 이동해가면서 영상을 복원
	for(nCounter=0; nCounter<7; nCounter++)
	{
		//초기 값 cumulative variables
		nSumofSLoss = 0;
		nLossCount = 0;
		nSumofSquaredOuts = 0;
		nSumofOuts = 0;
		// (2) 지정된 블록에서 전달량 계산
		for(nY=nStartY; nY<nEndY; nY++)
		{
			for(nX=nStartX; nX<nEndX; nX++)
			{
				// (4) 복원된 영상의 variance 및 loss 계산
				nOut = ((m_pnSmallYImg[nY*320+nX] - m_nAirlight)*nTrans + 128*m_nAirlight)>>7; // 전달량에 따른 복원 픽셀값 계산 (I-A)/t + A를 --> ((I-A)*k*128 + A*128)/128 꼴로 변경하여 속도향상
				nSquaredOut = nOut * nOut;	// 복원 한 값의 제곱
				// 255보다 큰 손실, 0보다 작은 손실 계산
				if(nOut>255)
				{
					nSumofSLoss += (nOut - 255)*(nOut - 255);
					nLossCount++;
				}
				else if(nOut < 0)
				{
					nSumofSLoss += nSquaredOut;
					nLossCount++;
				}
				// 손실의 합 계산
				nSumofSquaredOuts += nSquaredOut;
				nSumofOuts += nOut;
			}
		}
		// 복원 값의 평균
		fMean = (float)(nSumofOuts)/(float)(nNumberofPixels);
		// 비용함수 계산
		fCost = m_fLambda1 * (float)nSumofSLoss/(float)(nNumberofPixels)// 손실
			- ((float)nSumofSquaredOuts/(float)nNumberofPixels - fMean*fMean)	// 대조비
			+ m_fLambda2/fPreTrs/fPreTrs*fWsum/(float)nNumberofPixels*((fPreTrs-fTrans)*(fPreTrs-fTrans)*255.0f*255.0f);//이전 프레임 데이터 fPreTrs/fPreTrs*fWsum/(float)nNumberofPixels : 크기를 맞추어주기위한 셋팅값

		// (5) -variance, loss, 이전 프레임간의 차이를 최소로 하는 전달량 검출
		// 최적 전달량 검색
		if(nCounter==0 || fMinCost > fCost)
		{
			fMinCost = fCost;
			fOptTrs = fTrans;
		}

		fTrans += 0.1f;
		nTrans = (int)(1/fTrans*128.0f);
	}
	// (6) 계산된 전달량을 리턴
	return fOptTrs;
}

//////////////////////////////////////////////////////////////////////////
// 기능 : LUT 생성
// 동작
//		(1) 이전프레임 데이터를 이용할 때 쓰는 exp함수를 위한 LUT 생성
// 출력
//		- m_pfExpLUT: 256크기의 LUT
//////////////////////////////////////////////////////////////////////////
void dehazing::MakeExpLUT()
{
	int nIdx;

	for ( nIdx = 0 ; nIdx < 256; nIdx++ )
	{
		// (1) 이전프레임 데이터를 이용할 때 쓰는 exp함수를 위한 LUT 생성
		m_pfExpLUT[nIdx] = exp(-(float)(nIdx*nIdx)/10.0f);
	}
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 가이드 필터를 위한 LUT 생성
// 동작
//		(1) 가이드 필터에서 사용하는 블록 크기에 맞추어 LUT 생성
// 입력
//		- m_nGBlockSize : 가이드 필터의 블록 사이즈(멤버변수)
// 출력
//		- m_pfGuidedLUT : 가이드 필터를 위한 LUT
//////////////////////////////////////////////////////////////////////////
void dehazing::GuideLUTMaker()
{
	int nX, nY;

	// (1) 가이드 필터에서 사용하는 블록 크기에 맞추어 LUT 생성
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
//////////////////////////////////////////////////////////////////////////
// 기능 : 감마 복원을 위한 LUT
// 동작
//		(1) 감마 복원을 위한 LUT 생성
// 입력
//		- fParameter : 감마값
// 출력
//		- m_pucGammaLUT : 감마 복원을 위한 LUT
//////////////////////////////////////////////////////////////////////////
void dehazing::GammaLUTMaker(float fParameter)
{
	int nIdx;

	// (1) 감마 복원을 위한 LUT 생성
	for(nIdx=0; nIdx<256; nIdx++)
	{
		m_pucGammaLUT[nIdx] = (uchar)(pow((float)nIdx/255, fParameter)*255.0f);
	}
}

//////////////////////////////////////////////////////////////////////////
// 기능 : Downsampling된 영상에 대해 edge를 보존하면서 전달량(transmission)을 계산
// 배경 이론:
//		guided filter를 projection 개념으로 접근 (Least Square fitting을 projection으로 해결)
//		영상 특성을 가지는 p축과 unit vector 1의 조합으로 pixel based q == q_bar == ap + b1 임을 이용하여
//		q - q_bar가 최소가 되는 a, b 계산.
//		따라서		a = q'p (p는 normalize 되고 1vector와 수직)
//					b = q'1 (1은 normalize 된 vector)
//
// 동작
//		(1) 각 블록마다 step size만큼 이동하면서 다음 연산을 수행
//		(2) 1 vector normalize -> fNormV1 생성
//		(3) fNormV1과 수직인 p vector 생성(입력 guidance image(p)기준)
//		(4) p vector normalize -> norm_p 생성
//		(5) a = q'norm_p
//		(6) b = q'norm_v1
//		(7) a와 b를 통해 얻은 q_bar에 gaussian weight를 곱
//		(8) 분모에 gaussian weight를 곱 --> weighted_denom
//		(9) m_pfSmallInteg 에 블록의 q_bar를 중첩하여 더함
//		(10) m_pfSmallDenom 에 블록의 weighted_denom 더함
//		(11) m_pfSmallTransR = m_pfSmallInteg / m_pfSmallDenom
//
// 입력
//		- nW : Downsampling된 영상의 width
//		- nH : Downsampling된 영상의 height
//		- m_pnSmallYImg : Downsampling된 입력영상
//		- pnSmallTrans : Downsampling된 transmission 영상
// 출력
//		- m_pfSmallTransR : Edge가 보존된 transmission 영상
// 참고 : 전체 코드에 대해 __m128을 이용하여 SSE가 적용되어있음
//////////////////////////////////////////////////////////////////////////
void dehazing::GuidedFilter(int nW, int nH)
{

	int nStep = m_nStepSize;				
	int nWstep = m_nGBlockSize/nStep;		// 스텝 사이즈 x축
	int nHstep = m_nGBlockSize/nStep;		// 스템 사이즈 y축

	int nYb, nXb, nYa, nXa;
	int nIdxA, nIdxB, nI;
	float fDenom, fNormV1, fMeanI;		// fDenom:분모, fNormV1: b를 위한 축, fMeanI: 입력 영상 평균
	float fAk, fBk;						// fAk : a*I+b 중 a , fBk : a*I+b 중 b

	__m128 sseMean, sseY, ssePk_p, sseDenom, sseInteg, sseNormPk,	// SSE를 위한 변수
		sseAk, sseBk, sseP, sseNormV1, sseGauss, sseBkV1, sseQ;

	// 변수 초기화 및 float 형 변환
	for ( nYb = 0 ; nYb < nH; nYb++ )
	{
		for ( nXb = 0; nXb < nW; nXb++ )
		{
			m_pfSmallY[nYb*nW+nXb]=(float)m_pnSmallYImg[nYb*nW+nXb];
			m_pfSmallInteg[nYb*nW+nXb]=0;
			m_pfSmallDenom[nYb*nW+nXb]=0;
		}
	}
	// (1) 각 블록마다 step size만큼 이동하면서 다음 연산을 수행
	for ( nYa = 0 ; nYa < (nH-m_nGBlockSize)/nHstep+1 ; nYa++ )
	{
		for ( nXa = 0 ; nXa < (nW-m_nGBlockSize)/nWstep+1 ; nXa++ )
		{
			nIdxA=nYa*nHstep*nW+nXa*nWstep;

			// (2) 1 vector normalize -> fNormV1 생성
			fNormV1 = 1.0f/m_nGBlockSize;
			fMeanI = 0.0f;

			// (3) fNormV1과 수직인 p vector(m_pfSmallPk_p) 생성(입력 guidance image(p)기준)
			//	fNormV1과 수직인 p vector == m_pfSmallY - fMeanI;

			sseMean = _mm_setzero_ps();
			sseY = _mm_setzero_ps();

			// mean 값 계산
			for ( nYb = 0 ; nYb < m_nGBlockSize; nYb++ )
			{
				for ( nXb = 0; nXb < m_nGBlockSize; nXb+=4 )
				{
					sseY = _mm_loadu_ps((m_pfSmallY+(nIdxA+nYb*nW+nXb)));
					sseMean = _mm_add_ps( sseMean, sseY );
				}
			}

			for ( nI = 0 ; nI < 4; nI++ )
			{
				fMeanI += *((float*)(&sseMean)+nI);
			}

			fMeanI = fMeanI/(m_nGBlockSize*m_nGBlockSize);

			sseMean = _mm_set1_ps(fMeanI);
			sseY = _mm_setzero_ps();

			ssePk_p;
			
			// m_pfSmallY - fMeanI 계산
			for ( nYb = 0 ; nYb < m_nGBlockSize; nYb++ )
			{
				for ( nXb = 0; nXb < m_nGBlockSize; nXb+=4 )
				{
					sseY = _mm_loadu_ps(m_pfSmallY+(nIdxA+nYb*nW+nXb));
					ssePk_p = _mm_sub_ps( sseY, sseMean );

					for ( nI = 0 ; nI < 4; nI++)
					{
						m_pfSmallPk_p[nYb*m_nGBlockSize+nXb+nI] = *((float*)(&ssePk_p)+nI);
					}
				}
			}

			// (4) p vector normalize -> m_pfSmallNormPk 생성
			// norm 값 계산을 위한 분모 설정, m_pfSmallPk_p^2
			fDenom=0.0f;

			sseDenom =_mm_setzero_ps();

			for ( nIdxB = 0 ; nIdxB < m_nGBlockSize*m_nGBlockSize; nIdxB+=4 )
			{
				ssePk_p = _mm_loadu_ps(m_pfSmallPk_p+nIdxB);
				sseDenom = _mm_add_ps( sseDenom, _mm_mul_ps( ssePk_p, ssePk_p ));
			}

			for ( nI = 0 ; nI < 4; nI++ )
			{
				fDenom += *((float*)(&sseDenom)+nI);
			}

			sseDenom = _mm_set1_ps(sqrt(fDenom));

			sseNormPk;

			fAk = 0.0f;
			fBk = 0.0f;

			sseAk = _mm_setzero_ps();
			sseBk = _mm_setzero_ps();
			sseP;
			
			
			// (5) a = q'norm_p
			// (6) b = q'norm_v1
			// 예외 처리(분모가 0일 때)
			if(fDenom == 0)
			{
				// a = 0;
				fAk = 0;

				sseNormV1 = _mm_set1_ps(fNormV1);
				for ( nYb = 0 ; nYb < m_nGBlockSize; nYb++ )
				{
					for ( nXb = 0; nXb < m_nGBlockSize; nXb+=4 )
					{
						sseP = _mm_loadu_ps(m_pfSmallTrans+(nIdxA+nYb*nW+nXb));
						sseBk = _mm_add_ps( _mm_mul_ps( sseP , sseNormV1 ), sseBk );
					}
				}

				// (6) b = q'norm_v1
				for ( nI = 0 ; nI < 4; nI++ )
					fBk += *((float*)(&sseBk)+nI);
			}
			else
			{
				// m_pfSmallNormPk 계산
				for ( nIdxB = 0 ; nIdxB < m_nGBlockSize*m_nGBlockSize; nIdxB+=4 )
				{
					ssePk_p = _mm_loadu_ps(m_pfSmallPk_p+nIdxB);
					sseNormPk = _mm_div_ps(ssePk_p, sseDenom);

					for ( nI = 0 ; nI < 4; nI++ )
					{
						m_pfSmallNormPk[nIdxB+nI] = *((float*)(&sseNormPk)+nI);
					}
				}

				// a = q'norm_p 계산
				// b = q'norm_v1 계산 
				sseNormV1 = _mm_set1_ps(fNormV1);

				for ( nYb = 0 ; nYb < m_nGBlockSize; nYb++ )
				{
					for ( nXb = 0; nXb < m_nGBlockSize; nXb+=4 )
					{
						sseP = _mm_loadu_ps(m_pfSmallTrans+(nIdxA+nYb*nW+nXb));
						sseNormPk = _mm_loadu_ps(m_pfSmallNormPk+(nYb*m_nGBlockSize+nXb));

						sseAk = _mm_add_ps( _mm_mul_ps( sseP , sseNormPk ), sseAk );
						sseBk = _mm_add_ps( _mm_mul_ps( sseP , sseNormV1 ), sseBk );
					}
				}

				for ( nI = 0 ; nI < 4; nI++ )
				{
					fAk += *((float*)(&sseAk)+nI);
					fBk += *((float*)(&sseBk)+nI);
				}
			}

			sseGauss;
			sseInteg;
			sseBkV1 = _mm_set1_ps(fBk*fNormV1);
			sseAk = _mm_set1_ps(fAk);
			// (7) a와 b를 통해 얻은 q_bar에 gaussian weight를 곱
			// (8) 분모에 gaussian weight를 곱 --> weighted_denom

			for ( nYb = 0 ; nYb < m_nGBlockSize; nYb++ )
			{
				for ( nXb = 0; nXb < m_nGBlockSize; nXb+=4 )
				{ 
					sseNormPk = _mm_loadu_ps(m_pfSmallNormPk+(nYb*m_nGBlockSize+nXb));
					sseGauss = _mm_loadu_ps(m_pfGuidedLUT+(nYb*m_nGBlockSize+nXb));

					sseInteg = _mm_mul_ps(_mm_add_ps( _mm_mul_ps(sseNormPk, sseAk), sseBkV1 ), sseGauss);
					// (9) m_pfSmallInteg 에 블록의 q_bar를 중첩하여 더함
					// (10) m_pfSmallDenom 에 블록의 weighted_denom 더함
					for ( nI = 0 ; nI < 4; nI++ )
					{
						m_pfSmallInteg[nIdxA+nYb*nW+nXb+nI] += *((float*)(&sseInteg)+nI);
						m_pfSmallDenom[nIdxA+nYb*nW+nXb+nI] += *((float*)(&sseGauss)+nI);
					}
				}
			}
		}
	}

	// (11) m_pfSmallTransR = m_pfSmallInteg / m_pfSmallDenom
	for( nIdxA = 0 ; nIdxA < nW*nH; nIdxA+=4 )
	{
		sseInteg = _mm_loadu_ps(m_pfSmallInteg+nIdxA);
		sseDenom = _mm_loadu_ps(m_pfSmallDenom+nIdxA);

		sseQ = _mm_div_ps(sseInteg, sseDenom);

		for( nI = 0; nI < 4; nI++)
			m_pfSmallTransR[nIdxA+nI] = *((float*)(&sseQ)+nI);
	}
}
//////////////////////////////////////////////////////////////////////////
// 기능 : Downsampling된 영상에 대해 edge를 보존하면서 전달량(transmission)을 계산
// 배경 이론:
//		guided filter를 projection 개념으로 접근 (Least Square fitting을 projection으로 해결)
//		영상 특성을 가지는 p축과 unit vector 1의 조합으로 pixel based q == q_bar == ap + b1 임을 이용하여
//		q - q_bar가 최소가 되는 a, b 계산.
//		따라서		a = q'p (p는 normalize 되고 1vector와 수직)
//					b = q'1 (1은 normalize 된 vector)
//
// 동작
//		(1) 각 블록마다 step size만큼 이동하면서 다음 연산을 수행
//		(2) 1 vector normalize -> fNormV1 생성
//		(3) fNormV1과 수직인 p vector 생성(입력 guidance image(p)기준)
//		(4) p vector normalize -> norm_p 생성
//		(5) a = q'norm_p
//		(6) b = q'norm_v1
//		(7) a와 b를 통해 얻은 q_bar에 gaussian weight를 곱
//		(8) 분모에 gaussian weight를 곱 --> weighted_denom
//		(9) m_pfSmallInteg 에 블록의 q_bar를 중첩하여 더함
//		(10) m_pfSmallDenom 에 블록의 weighted_denom 더함
//		(11) m_pfSmallTransR = m_pfSmallInteg / m_pfSmallDenom
//
// 입력
//		- m_pnSmallYImg : Downsampling된 입력영상
//		- pnSmallTrans : Downsampling된 transmission 영상
// 출력
//		- m_pfSmallTransR : Edge가 보존된 transmission 영상
// 참고 : 전체 코드에 대해 __m128을 이용하여 SSE가 적용되어있음
//		  Class 내부의 member variable을 바로 불러서 쓸 경우, complexity 증가가 발생하여
//		  local한 변수를 선언하여 member variable의 pointer를 받아서 사용
//////////////////////////////////////////////////////////////////////////
void dehazing::GuidedFilter()
{
	int nStep = m_nStepSize;					// step size

	int nEPBlockSizeL = m_nGBlockSize;			// 블록 크기
	int nHeiL = m_nHei;							// 높이
	int nWidL = m_nWid;							// 너비
	// local 변수 선언
	float* pfYL = m_pfY;						
	float* pfIntegL = m_pfInteg;
	float* pfDenomL = m_pfDenom;
	int* pnYImgL = m_pnYImg;
	float* pfPk_pL = m_pfPk_p;
	float* pfNormPkL = m_pfNormPk;
	float* pfGuidedLUTL = m_pfGuidedLUT;
	float* pfTransmissionL = m_pfTransmission;

	int nWstep = nEPBlockSizeL/nStep;			// 스텝 사이즈 x축
	int nHstep = nEPBlockSizeL/nStep;			// 스텝 사이즈 y축

	float fDenom, fNormV1, fMeanI;				// fDenom:분모, fNormV1: b를 위한 축, fMeanI: 입력 영상 평균
	float fAk = 0.0f;							// fAk : a*I+b 중 a , fBk : a*I+b 중 b
	float fBk = 0.0f;
	int nIdxA, nYa, nXa, nYb, nXb, nI, nIdxB;	//  indexing용 변수
	__m128 sseMean, sseY, ssePk_p, sseDenom, sseInteg, sseNormPk, 
		sseAk, sseBk, sseP, sseNormV1, sseGauss, sseBkV1,	sseQ;
	
	// 변수 초기화 및 float 형 변환
	for ( nYb = 0 ; nYb < nHeiL; nYb++ )
	{
		for ( nXb = 0; nXb < nWidL; nXb++ )
		{
			pfYL[nYb*nWidL+nXb]=(float)pnYImgL[nYb*nWidL+nXb];
			pfIntegL[nYb*nWidL+nXb]=0;
			pfDenomL[nYb*nWidL+nXb]=0;
		}
	}
	// (1) 각 블록마다 step size만큼 이동하면서 다음 연산을 수행
	for ( nYa = 0 ; nYa < (nHeiL-nEPBlockSizeL)/nHstep+1 ; nYa++ )
	{
		for ( nXa = 0 ; nXa < (nWidL-nEPBlockSizeL)/nWstep+1 ; nXa++ )
		{
			nIdxA=nYa*nHstep*nWidL+nXa*nWstep;
			// (2) 1 vector normalize -> fNormV1 생성
			fNormV1 = 1.0f/nEPBlockSizeL;
			fMeanI = 0.0f;

			// (3) fNormV1과 수직인 p vector(m_pfSmallPk_p) 생성(입력 guidance image(p)기준)
			//	fNormV1과 수직인 p vector == m_pfSmallY - fMeanI;
			sseMean = _mm_setzero_ps();
			sseY = _mm_setzero_ps();

			// mean 값 계산
			for ( nYb = 0 ; nYb < nEPBlockSizeL; nYb++ )
			{
				for ( nXb = 0; nXb < nEPBlockSizeL; nXb+=4 )
				{
					sseY = _mm_loadu_ps((pfYL+(nIdxA+nYb*nWidL+nXb)));
					sseMean = _mm_add_ps( sseMean, sseY );
				}
			}

			for ( nI = 0 ; nI < 4; nI++ )
			{
				fMeanI += *((float*)(&sseMean)+nI);
			}

			fMeanI = fMeanI/(nEPBlockSizeL*nEPBlockSizeL);
			
			sseMean = _mm_set1_ps(fMeanI);
			sseY = _mm_setzero_ps();

			// m_pfSmallY - fMeanI 계산				
			for ( nYb = 0 ; nYb < nEPBlockSizeL; nYb++ )
			{
				for ( nXb = 0; nXb < nEPBlockSizeL; nXb+=4 )
				{
					sseY = _mm_loadu_ps(pfYL+(nIdxA+nYb*nWidL+nXb));
					ssePk_p = _mm_sub_ps( sseY, sseMean );

					for ( nI = 0 ; nI < 4; nI++)
					{
						pfPk_pL[nYb*nEPBlockSizeL+nXb+nI] = *((float*)(&ssePk_p)+nI);
					}
				}
			}
			// (4) p vector normalize -> m_pfSmallNormPk 생성
			// norm 값 계산을 위한 분모 설정, m_pfSmallPk_p^2
			fDenom=0.0f;

			sseDenom =_mm_setzero_ps();

			for ( nIdxB = 0 ; nIdxB < nEPBlockSizeL*nEPBlockSizeL; nIdxB+=4 )
			{
				ssePk_p = _mm_loadu_ps(pfPk_pL+nIdxB);
				sseDenom = _mm_add_ps( sseDenom, _mm_mul_ps( ssePk_p, ssePk_p ));
			}

			for ( nI = 0 ; nI < 4; nI++ )
			{
				fDenom += *((float*)(&sseDenom)+nI);
			}

			sseDenom = _mm_set1_ps(sqrt(fDenom));
			
			sseAk = _mm_setzero_ps();
			sseBk = _mm_setzero_ps();
			// (5) a = q'norm_p
			// (6) b = q'norm_v1
			// 예외 처리(분모가 0일 때)
			// a = 0;
			fAk = 0.0f;
			fBk = 0.0f;

			if(fDenom == 0)
			{
				sseNormV1 = _mm_set1_ps(fNormV1);
				for ( nYb = 0 ; nYb < nEPBlockSizeL; nYb++ )
				{
					for ( nXb = 0; nXb < nEPBlockSizeL; nXb+=4 )
					{
						sseP = _mm_loadu_ps(pfTransmissionL+(nIdxA+nYb*nWidL+nXb));
						sseBk = _mm_add_ps( _mm_mul_ps( sseP , sseNormV1 ), sseBk );
					}
				}
				// (6) b = q'norm_v1
				for ( nI = 0 ; nI < 4; nI++ )
					fBk += *((float*)(&sseBk)+nI);
			}
			else
			{
				// m_pfSmallNormPk 계산
				for ( nIdxB = 0 ; nIdxB < nEPBlockSizeL*nEPBlockSizeL; nIdxB+=4 )
				{
					ssePk_p = _mm_loadu_ps(pfPk_pL+nIdxB);
					sseNormPk = _mm_div_ps(ssePk_p, sseDenom);

					for ( int nI = 0 ; nI < 4; nI++ )
					{
						pfNormPkL[nIdxB+nI] = *((float*)(&sseNormPk)+nI);
					}
				}

				sseNormV1 = _mm_set1_ps(fNormV1);
				// a = q'norm_p 계산
				// b = q'norm_v1 계산 
				for ( nYb = 0 ; nYb < nEPBlockSizeL; nYb++ )
				{
					for ( nXb = 0; nXb < nEPBlockSizeL; nXb+=4 )
					{
						sseP = _mm_loadu_ps(pfTransmissionL+(nIdxA+nYb*nWidL+nXb));
						sseNormPk = _mm_loadu_ps(pfNormPkL+(nYb*nEPBlockSizeL+nXb));

						sseAk = _mm_add_ps( _mm_mul_ps( sseP , sseNormPk ), sseAk );
						sseBk = _mm_add_ps( _mm_mul_ps( sseP , sseNormV1 ), sseBk );
					}
				}

				for ( nI = 0 ; nI < 4; nI++ )
				{
					fAk += *((float*)(&sseAk)+nI);
					fBk += *((float*)(&sseBk)+nI);
				}
			}
					
			sseBkV1 = _mm_set1_ps(fBk*fNormV1);
			sseAk = _mm_set1_ps(fAk);
			// (7) a와 b를 통해 얻은 q_bar에 gaussian weight를 곱
			// (8) 분모에 gaussian weight를 곱 --> weighted_denom
			for ( nYb = 0 ; nYb < nEPBlockSizeL; nYb++ )
			{
				for ( nXb = 0; nXb < nEPBlockSizeL; nXb+=4 )
				{ 
					sseNormPk = _mm_loadu_ps(pfNormPkL+(nYb*nEPBlockSizeL+nXb));
					sseGauss = _mm_loadu_ps(pfGuidedLUTL+(nYb*nEPBlockSizeL+nXb));

					sseInteg = _mm_mul_ps(_mm_add_ps( _mm_mul_ps(sseNormPk, sseAk), sseBkV1 ), sseGauss);
					// (9) m_pfSmallInteg 에 블록의 q_bar를 중첩하여 더함
					// (10) m_pfSmallDenom 에 블록의 weighted_denom 더함
					for ( nI = 0 ; nI < 4; nI++ )
					{
						pfIntegL[nIdxA+nYb*nWidL+nXb+nI] += *((float*)(&sseInteg)+nI);
						pfDenomL[nIdxA+nYb*nWidL+nXb+nI] += *((float*)(&sseGauss)+nI);
					}
				}
			}
		}
	}

	// (11) m_pfSmallTransR = m_pfSmallInteg / m_pfSmallDenom
	for( int nIdx = 0 ; nIdx < nWidL*nHeiL; nIdx+=4 )
	{
		sseInteg = _mm_loadu_ps(pfIntegL+nIdx);
		sseDenom = _mm_loadu_ps(pfDenomL+nIdx);

		sseQ = _mm_div_ps(sseInteg, sseDenom);

		for( int nI = 0; nI < 4; nI++)
			m_pfTransmissionR[nIdx+nI] = *((float*)(&sseQ)+nI);
	}
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 복사
// 동작
//		(1) pnSrc-> pnDst로 복사
// 입력
//		- pnSrc : 입력영상
// 출력
//		- pnDst : 출력영상
//////////////////////////////////////////////////////////////////////////
void dehazing::CopyTo(int* pnSrc, int* pnDst)
{
	int nIdx;

	// (1) pnSrc-> pnDst로 복사
	for(nIdx=0; nIdx<320*240; nIdx++)
	{
		pnDst[nIdx] = pnSrc[nIdx];
	}
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 복사
// 동작
//		(1) pfSrc-> pfDst로 복사
// 입력
//		- pfSrc : 입력영상
// 출력
//		- pfDst : 출력영상
//////////////////////////////////////////////////////////////////////////
void dehazing::CopyTo(float* pfSrc, float* pfDst)
{
	int nIdx;

	// (1) pfSrc-> pfDst로 복사
	for(nIdx=0; nIdx<320*240; nIdx++)
	{
		pfDst[nIdx] = pfSrc[nIdx];
	}
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 안개 제거 영상을 저장
// 동작
//		(1) 후처리 결과 영상 획득 유무 결정
//		(2) I' == (I - Airlight)/Transmission + Airlight 의 식을 통해 안개 제거 영상을 얻음
//		(3) Gamma correction LUT을 이용하여 값 보정
// 입력
//		- imInput : 입력영상
//		- m_pfTransmissionR : Transmission 영상
//		- m_anAirlight : RGB 채널 각각에 대한 안개값
// 출력
//		- imOutput : 출력영상(안개제거 영상)
// 참고 :	for문에 대해서 OpenMP가 적용되어 있음
//			m_bPostFlag == true 인 경우, 하늘영역에 대해 선택적으로 blur를 수행(단, 속도저하가 발생)
//////////////////////////////////////////////////////////////////////////
void dehazing::RestoreImage(IplImage* imInput, IplImage* imOutput)
{
	int nStep = imInput->widthStep;

	int nX, nY;
	float fA_R, fA_G, fA_B;

	fA_B = (float)m_anAirlight[0];
	fA_G = (float)m_anAirlight[1];
	fA_R = (float)m_anAirlight[2];

	// (1) 후처리 결과 영상 획득 유무 결정
	if(m_bPostFlag == true)
	{
		PostProcessing(imInput,imOutput);
	}
	else
	{
#pragma omp parallel for
		// (2) I' = (I - Airlight)/Transmission + Airlight 의 식을 통해 안개 제거 영상을 얻음
		for(nY=0; nY<m_nHei; nY++)
		{
			for(nX=0; nX<m_nWid; nX++)
			{			
				// (3) Gamma correction LUT을 이용하여 값 보정
				imOutput->imageData[nY*nStep+nX*3]	 = (uchar)m_pucGammaLUT[(uchar)CLIP((((float)((uchar)imInput->imageData[nY*nStep+nX*3+0])-fA_B)/CLIP_Z(m_pfTransmissionR[nY*m_nWid+nX]) + fA_B))];
				imOutput->imageData[nY*nStep+nX*3+1] = (uchar)m_pucGammaLUT[(uchar)CLIP((((float)((uchar)imInput->imageData[nY*nStep+nX*3+1])-fA_G)/CLIP_Z(m_pfTransmissionR[nY*m_nWid+nX]) + fA_G))];
				imOutput->imageData[nY*nStep+nX*3+2] = (uchar)m_pucGammaLUT[(uchar)CLIP((((float)((uchar)imInput->imageData[nY*nStep+nX*3+2])-fA_R)/CLIP_Z(m_pfTransmissionR[nY*m_nWid+nX]) + fA_R))];
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 하늘영역에 대해 blur를 수행
// 동작
//		(1)  I' == (I - Airlight)/Transmission + Airlight 의 식을 통해 안개 제거 영상을 얻음 (이 때 감마 보정, gamma correction LUT를 이용한 보정 통합
//		(2) 안개제거 영상 획득과 동시에 nDisPos만큼 떨어진 픽셀 값을 관찰
//		(3) Transmission이 0.3이고, 인접한 픽셀간의 R,G,B 채널값의 차이중 최대 차이가 20이하 이면서
//			nNumStep만큼 떨어진 위치의 픽셀값과 차이가 거의 없을 경우, blur를 수행
// 입력
//		- imInput : 입력영상
//		- m_pfTransmissionR : Transmission 영상
//		- m_anAirlight : RGB 채널 각각에 대한 안개값
//		- nNumStep : blur를 수행하는 범위
//		- nDisPos : blur 대상의 위치
//		- nAD : 현재 픽셀에서 nDisPos, nDisPos-1만큼 전의 픽셀값 간의 차이
// 출력
//		- imOutput : 출력영상(안개제거 영상)
// 참고 :	for문에 대해서 OpenMP가 적용되어 있음
//////////////////////////////////////////////////////////////////////////
void dehazing::PostProcessing(IplImage* imInput, IplImage* imOutput)
{
	const int nStep = imInput->widthStep;
	const int nNumStep= 10;
	const int nDisPos= 20;

	float nAD0, nAD1, nAD2;
	int nS, nX, nY;
	float fA_R, fA_G, fA_B;

	fA_B = (float)m_anAirlight[0];
	fA_G = (float)m_anAirlight[1];
	fA_R = (float)m_anAirlight[2];

#pragma omp parallel for private(nAD0, nAD1, nAD2, nS)
	for(nY=0; nY<m_nHei; nY++)
	{
		for(nX=0; nX<m_nWid; nX++)
		{			
			// (1)  I' = (I - Airlight)/Transmission + Airlight 의 식을 통해 안개 제거 영상을 얻음 (이 때 감마 보정, gamma correction LUT를 이용한 보정 통합
			imOutput->imageData[nY*nStep+nX*3]	 = (uchar)m_pucGammaLUT[(uchar)CLIP((((float)((uchar)imInput->imageData[nY*nStep+nX*3+0])-fA_B)/CLIP_Z(m_pfTransmissionR[nY*m_nWid+nX]) + fA_B))];
			imOutput->imageData[nY*nStep+nX*3+1] = (uchar)m_pucGammaLUT[(uchar)CLIP((((float)((uchar)imInput->imageData[nY*nStep+nX*3+1])-fA_G)/CLIP_Z(m_pfTransmissionR[nY*m_nWid+nX]) + fA_G))];
			imOutput->imageData[nY*nStep+nX*3+2] = (uchar)m_pucGammaLUT[(uchar)CLIP((((float)((uchar)imInput->imageData[nY*nStep+nX*3+2])-fA_R)/CLIP_Z(m_pfTransmissionR[nY*m_nWid+nX]) + fA_R))];

			// Transmission 값이 0.4이하의 안개가 짙은 영역에서만 후처리 보정 수행(안개 짙은 영역에서 보정이 필요하기 때문)
			if( nX > nDisPos+nNumStep && m_pfTransmissionR[nY*m_nWid+nX-nDisPos] < 0.4 )
			{
				// (2) 안개제거 영상 획득과 동시에 nDisPos만큼 떨어진 픽셀 값을 관찰
				nAD0 = (float)((int)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos)*3])	- (int)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1)*3]));
				nAD1 = (float)((int)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos)*3+1])	- (int)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1)*3+1]));
				nAD2 = (float)((int)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos)*3+2])	- (int)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1)*3+2]));

				// (3) Transmission이 0.3이고, 인접한 픽셀간의 R,G,B 채널값의 차이중 최대 차이가 20이하 이면서
				// nNumStep만큼 떨어진 위치의 픽셀값과 차이가 거의 없을 경우, blur를 수행
				if(__max(__max(abs(nAD0),abs(nAD1)),abs(nAD2)) < 20 
					// 차이 비교
					&&	 abs((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1)*3+0]-(uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1-nNumStep)*3+0])
					+abs((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1)*3+1]-(uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1-nNumStep)*3+1])
					+abs((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1)*3+2]-(uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1-nNumStep)*3+2])
					+abs((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos)*3+0]-(uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1+nNumStep)*3+0])
					+abs((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos)*3+1]-(uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1+nNumStep)*3+1])
					+abs((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos)*3+2]-(uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1+nNumStep)*3+2]) < 30 )
				{
					for( nS = 1 ; nS < nNumStep+1; nS++)
					{
						// 블러 수행
						imOutput->imageData[nY*nStep+(nX-nDisPos-1+nS-nNumStep)*3+0]=(uchar)CLIP((float)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1+nS-nNumStep)*3+0])+(float)nS*nAD0/(float)nNumStep);
						imOutput->imageData[nY*nStep+(nX-nDisPos-1+nS-nNumStep)*3+1]=(uchar)CLIP((float)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1+nS-nNumStep)*3+1])+(float)nS*nAD1/(float)nNumStep);
						imOutput->imageData[nY*nStep+(nX-nDisPos-1+nS-nNumStep)*3+2]=(uchar)CLIP((float)((uchar)imOutput->imageData[nY*nStep+(nX-nDisPos-1+nS-nNumStep)*3+2])+(float)nS*nAD2/(float)nNumStep);
					}
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// 기능 : 안개 제거 함수
//		(1) 영상이 첫 프레임인지 아닌지 확인하여 Airlight 검출 유무 및 이전 프레임 데이터 사용유무 결정
//		(2-1) 초기 프레임 일경우 LUT 생성
//		(2-2) 안개값 영역에 따른 안개 값 검출 수행
//		(3) IplImage를 int로 변환
//		(4) 영상 다운샘플링(320x240)
//		(5)	전달량 검출 함수 호출
//		(5-1) 이전프레임 활용을 위한 데이터 저장
//		(6) 블록단위 전달량에 가이드 필터 적용
//		(7) 픽셀단위 전달량의 업샘플링(원본 크기)
//		(8) 업샘플링 된 전달량에 필터 적용 
//		(9) 영상 복원 수행
// 입력
//		- imInput : 입력영상
//		- nFrame : 프레임넘버
// 출력
//		- imOutput : 복원된 출력 영상
//
//////////////////////////////////////////////////////////////////////////
void dehazing::HazeRemoval(IplImage* imInput, IplImage* imOutput, int nFrame)
{
	// (1) 영상이 첫 프레임인지 아닌지 확인하여 Airlight 검출 유무 및 이전 프레임 데이터 사용유무 결정
	if(nFrame == 0)
	{
		IplImage* imAir;
		// (2-1) 초기 프레임 일경우 LUT 생성
		MakeExpLUT(); // ExpLUP는 이전 프레임 데이터 사용시 쓰는 expnential function임
		GuideLUTMaker(); // Guided filter를 위한 LUT
		GammaLUTMaker(0.7f); // gamma correction을 위한 LUT

		// (2-2) 안개값 영역에 따른 안개 값 검출 수행
		// 안개값 영역 지정
		cvSetImageROI(imInput, cvRect(m_nTopLeftX, m_nTopLeftY, m_nBottomRightX-m_nTopLeftX, m_nBottomRightY-m_nTopLeftY));

		imAir = cvCreateImage(cvSize(m_nBottomRightX-m_nTopLeftX, m_nBottomRightY-m_nTopLeftY),IPL_DEPTH_8U, 3);
		cvCopyImage(imInput, imAir);
		// 안개값 검출 함수 호출
		AirlightEstimation(imAir);

		// grey 안개값 획득
		m_nAirlight = (m_anAirlight[0] + m_anAirlight[1] + m_anAirlight[2])/3;

		cvReleaseImage(&imAir);
		cvResetImageROI(imInput);
	}
	// (3) IplImage를 int로 변환
	IplImageToInt(imInput);
	// (4) 영상 다운샘플링(320x240)
	DownsampleImage();
	// (5)	전달량 검출 함수 호출
	TransmissionEstimation(nFrame);
	// (5-1) 이전프레임 활용을 위한 데이터 저장
	CopyTo(m_pfSmallTrans, m_pfSmallTransP);
	CopyTo(m_pnSmallYImg, m_pnSmallYImgP);	
	
	// (6) 블록단위 전달량에 가이드 필터 적용
	GuidedFilter(320, 240);

	// (7) 픽셀단위 전달량의 업샘플링(원본 크기)
	UpsampleImage();
	
	// (8) 업샘플링 된 전달량에 필터 적용 
	GuidedFilter();

	// (9) 영상 복원 수행
	RestoreImage(imInput, imOutput);
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 안개 제거 함수
//		(1) 초기 프레임 일경우 LUT 생성
//		(2) 안개값 영역에 따른 안개 값 검출 수행
//		(3) IplImage를 int로 변환
//		(4) 영상 다운샘플링(320x240)
//		(5)	전달량 검출 함수 호출
//		(6) 블록단위 전달량에 가이드 필터 적용
//		(7) 픽셀단위 전달량의 업샘플링(원본 크기)
//		(8) 업샘플링 된 전달량에 필터 적용 
//		(9) 영상 복원 수행
// 입력
//		- imInput : 입력영상
//		- nFrame : 프레임넘버
// 출력
//		- imOutput : 복원된 출력 영상
//
//////////////////////////////////////////////////////////////////////////
void dehazing::ImageHazeRemoval(IplImage* imInput, IplImage* imOutput)
{
	IplImage* imAir;
	IplImage* imSmallInput;
	// (1) 초기 프레임 일경우 LUT 생성
	MakeExpLUT(); // ExpLUP는 이전 프레임 데이터 사용시 쓰는 expnential function임
	GuideLUTMaker(); // Guided filter를 위한 LUT
	GammaLUTMaker(0.7f); // gamma correction을 위한 LUT

	// (2) 안개값 영역에 따른 안개 값 검출 수행
	// 안개값 영역 지정
	cvSetImageROI(imInput, cvRect(m_nTopLeftX, m_nTopLeftY, m_nBottomRightX-m_nTopLeftX, m_nBottomRightY-m_nTopLeftY));

	imAir = cvCreateImage(cvSize(m_nBottomRightX-m_nTopLeftX, m_nBottomRightY-m_nTopLeftY),IPL_DEPTH_8U, 3);
	imSmallInput = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);
	cvCopyImage(imInput, imAir);
	// 안개값 검출 함수 호출
	AirlightEstimation(imAir);

	// grey 안개값 획득
	m_nAirlight = (m_anAirlight[0] + m_anAirlight[1] + m_anAirlight[2])/3;

	cvReleaseImage(&imAir);
	cvResetImageROI(imInput);
	
	// (3) IplImage를 int로 변환
	IplImageToInt(imInput);
	// (4) 영상 다운샘플링(320x240)
	DownsampleImage();
	cvResize(imInput, imSmallInput, 1);
	// (5)	전달량 검출 함수 호출
	TransmissionEstimation(0);
	
	// (6) 블록단위 전달량에 가이드 필터 적용
	LowSpeedGuidedFilter(imSmallInput, m_pfSmallTrans, m_pfSmallTransR, 320, 240, 0.001);

	// (7) 픽셀단위 전달량의 업샘플링(원본 크기)
	UpsampleImage();

	// (8) 업샘플링 된 전달량에 필터 적용 
	LowSpeedGuidedFilter(imInput, m_pfTransmission, m_pfTransmissionR, m_nWid, m_nHei, 0.001);

	// (9) 영상 복원 수행
	RestoreImage(imInput, imOutput);
	cvReleaseImage(&imSmallInput);
}

//////////////////////////////////////////////////////////////////////////
// 기능 : airlight값 주소 리턴
// 동작
//		(1) airlight값 주소 리턴
// 출력
//		- m_anAirlight : 주소값 리턴
//////////////////////////////////////////////////////////////////////////
int* dehazing::GetAirlight()
{
	// (1) airlight값 주소 리턴
	return m_anAirlight;
}
//////////////////////////////////////////////////////////////////////////
// 기능 : Y 영상 주소 리턴
// 동작
//		(1) Y영상 주소 리턴
// 출력
//		- m_pnYImg : 주소값 리턴
//////////////////////////////////////////////////////////////////////////
int* dehazing::GetYImg()
{
	// (1) Y영상 주소 리턴
	return m_pnYImg;
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 전달량 주소 리턴
// 동작
//		(1) 전달량 주소 리턴
// 출력
//		- m_pfTransmission : 주소값 리턴
//////////////////////////////////////////////////////////////////////////
float* dehazing::GetTransmission()
{
	// (1) 전달량 주소 리턴
	return m_pfTransmission;
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 비용함수를 위한 람다값 셋팅
// 동작
//		(1) 람다값을 재정의 할 때 사용하는 함수
// 입력
//		- fLambdaLoss 손실 비용함수 람다값
//		- fLambdaTemp 시간 비용함수 람다값
//////////////////////////////////////////////////////////////////////////
void dehazing::LambdaSetting(float fLambdaLoss, float fLambdaTemp)
{
	// (1) 람다값을 재정의 할 때 사용하는 함수
	m_fLambda1 = fLambdaLoss;
	if(fLambdaTemp>0)
		m_fLambda2 = fLambdaTemp;
	else
		m_bPreviousFlag = false;
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 안개 판별 사용유무 결정
// 동작
//		(1) 안개 판별 사용유무 결정
// 입력
//		- bChoice 사용유무 결정
//////////////////////////////////////////////////////////////////////////
void dehazing::DecisionUse(bool bChoice)
{
	// (1) 안개 판별 사용유무 결정
	m_bDecision = bChoice;
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 이전 데이터 사용 유무 결정
// 동작
//		(1) 이전 데이터 사용 유무 결정
// 입력
//		- bPrevFlag 사용유무 결정
//////////////////////////////////////////////////////////////////////////
void dehazing::PreviousFlag(bool bPrevFlag)
{
	// (1) 이전 데이터 사용 유무 결정
	m_bPreviousFlag = bPrevFlag;
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 블록 크기 결정
// 동작
//		(1) 전달량 계산의 블록 크기 결정
// 입력
//		- nBlockSize 블록 크기 결정
//////////////////////////////////////////////////////////////////////////
void dehazing::TransBlockSize(int nBlockSize)
{
	// (1) 전달량 계산의 블록 크기 결정
	m_nTBlockSize = nBlockSize;
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 블록 크기 결정
// 동작
//		(1) 전달량 계산의 블록 크기 결정
// 입력
//		- nBlockSize 블록 크기 결정
//////////////////////////////////////////////////////////////////////////
void dehazing::FilterBlockSize(int nBlockSize)
{
	// (1) 전달량 계산의 블록 크기 결정
	m_nGBlockSize = nBlockSize;
}
//////////////////////////////////////////////////////////////////////////
// 기능 : Guided filter의 step size 수정
// 동작
//		(1) guided filter의 step size 수정
// 입력
//		- nStepSize : 수정할 크기 (예: 2 일경우 블록의  1/2 만큼 step size결정, 4일경우 블록의 1/4 만큼을 step size로 결정)
//////////////////////////////////////////////////////////////////////////
void dehazing::SetFilterStepSize(int nStepSize)
{
	// (1) guided filter의 step size 수정
	m_nStepSize = nStepSize;
}
//////////////////////////////////////////////////////////////////////////
// 기능 : 안개값 검출 범위 지정
// 동작
//		(1) 안개값을 검출하기위한 범위를 지정함
// 입력
//		- pointTopLeft :좌상단 값
//		- pointBottomRight : 우하단 값
//////////////////////////////////////////////////////////////////////////
void dehazing::AirlightSerachRange(POINT pointTopLeft, POINT pointBottomRight)
{
	// (1) 안개값을 검출하기위한 범위를 지정함
	m_nTopLeftX = pointTopLeft.x;
	m_nTopLeftY = pointTopLeft.y;
	m_nBottomRightX = pointBottomRight.x;
	m_nBottomRightY = pointBottomRight.y;
}

//////////////////////////////////////////////////////////////////////////
// 기능 : 안개 유무 체크
// 동작
//		(1) 안개 제거 처리 전, 후의 영상을 비교
//		(2) 문턱값 이상의 값을 가지면 안개로 판단
// 입력
//		- imInput : 입력 영상
//		- imOutput : 안개 제거 영상
//		- nThreshold : 문턱값
// 출력
//		- bool : 안개영상으로 판단되면 true, 아니면 false
//////////////////////////////////////////////////////////////////////////
bool dehazing::Decision(IplImage* imInput, IplImage* imOutput, int nThreshold)
{
	int nX, nY;
	int nStep;

	int nMAD;

	nMAD = 0;

	nStep = imInput->widthStep;
    // (1) 안개 제거 처리 전, 후의 영상을 비교
	for(nY=0; nY<m_nHei; nY++)
	{
		for(nX=0; nX<m_nWid; nX++)
		{
			nMAD += abs((int)imInput->imageData[nY*nStep+nX*3+2]-(int)imOutput->imageData[nY*nStep+nX*3+2]);
			nMAD += abs((int)imInput->imageData[nY*nStep+nX*3+1]-(int)imOutput->imageData[nY*nStep+nX*3+1]);
			nMAD += abs((int)imInput->imageData[nY*nStep+nX*3+0]-(int)imOutput->imageData[nY*nStep+nX*3+0]);
		}
	}

	nMAD /= (m_nWid*m_nHei);
	// (2) 문턱값 이상의 값을 가지면 안개로 판단
	if(nMAD > nThreshold)
		return true;
	else
		return false; 
}
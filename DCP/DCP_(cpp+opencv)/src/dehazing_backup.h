#include <cv.h>
#include <highgui.h>
#include <omp.h>
#include <emmintrin.h>

#define CLIP(x) ((x)<(0)?0:((x)>(255)?(255):(x)))
#define CLIP_Z(x) ((x)<(0)?0:((x)>(1.0f)?(1.0f):(x)))

using namespace std;

class dehazing
{
public:
	dehazing(int nW, int nH, bool bPrevFlag, bool bPosFlag);
	dehazing(int nW, int nH, bool bPrevFlag, bool bPosFlag, float fL1, float fL2);
	~dehazing(void);
	
	void	HazeRemoval(IplImage* imInput, IplImage* imOutput, int nFrame);
	void	ImageHazeRemoval(IplImage* imInput, IplImage* imOutput);
	void	LambdaSetting(float fLambdaLoss, float fLambdaTemp);
	void	DecisionUse(bool bChoice);
	void	TransBlockSize(int nBlockSize);
	void	FilterBlockSize(int nBlockSize);
	void	AirlightSerachRange(POINT pointTopLeft, POINT pointBottomRight);
	void	SetFilterStepSize(int nStepsize);
	void	PreviousFlag(bool bPrevFlag);

	int*	GetAirlight();
	int*	GetYImg();
	float*	GetTransmission();
	
private:

	//320*240 size
	float*	m_pfSmallY;			//Y image
	float*	m_pfSmallPk_p;		//(Y image) - (mean of Y image)
	float*	m_pfSmallNormPk;	//Normalize된 Y image
	float*	m_pfSmallInteg;		//Gaussian weight가 적용된 transmission 결과
	float*	m_pfSmallDenom;		//Gaussian weight가 저장된 행렬

	int*	m_pnSmallYImg;		//입력 영상의 Y채널
	float*	m_pfSmallTransP;	//이전 프레임의 transmission 영상
	float*	m_pfSmallTrans;		//초기 transmission 영상
	float*	m_pfSmallTransR;	//정련된 transmission 영상
	int*	m_pnSmallYImgP;		//이전 프레임의 Y채널

	//Original size
	float*	m_pfY;				//Y image
	float*	m_pfPk_p;			//(Y image) - (mean of Y image)
	float*	m_pfNormPk;			//Normalize된 Y image
	float*	m_pfInteg;			//Gaussian weight가 적용된 transmission 결과
	float*	m_pfDenom;			//Gaussian weight가 저장된 행렬
	
	int*	m_pnYImg;			//입력 영상의 Y채널
	float*	m_pfTransmission;	//초기 transmission
	float*	m_pfTransmissionR;	//정련된 transmission 영상
	
	//////////////////////////////////////////////////////////////////////////
	int		m_nStepSize;		//Guided filter의 step size;
	float*	m_pfGuidedLUT;		//Guided filter 내의 gaussian weight를 위한 LUT
	float	m_fGSigma;			//Guided filter 내의 gaussian weight에 대한 sigma

	int		m_anAirlight[3];	//안개값
	uchar	m_pucGammaLUT[256];	//감마 보정을 위한 LUT
	float	m_pfExpLUT[256];	//Transmission 계산시, 픽셀 차이에 대한 weight용 LUT

	int		m_nAirlight;		//안개값(grey)
	
	bool	m_bPreviousFlag;	//이전 프레임 이용 여부
	float	m_fLambda1;			//Loss cost
	float	m_fLambda2;			//Temporal cost

	bool	m_bDecision;		//안개 여부 체크

	int		m_nWid;				//너비
	int		m_nHei;				//높이

	int		m_nTBlockSize;		//Transmission 예측 블록 크기
	int		m_nGBlockSize;		//Guided filter의 블록 크기

	//Airlight search range
	int		m_nTopLeftX;				
	int		m_nTopLeftY;
	int		m_nBottomRightX;
	int		m_nBottomRightY;

	bool	m_bPostFlag;		//후처리 여부 flag

	void	CopyTo(int*, int*);
	void	CopyTo(float*, float*);
	void	GuidedFilter(int nW, int nH);
	void	DownsampleImage();
	void	UpsampleImage();
	void	MakeExpLUT();
	void	IplImageToInt(IplImage* imInput);
	void	AirlightEstimation(IplImage* imInput);
	void	TransmissionEstimation(int nFrame);
	float	NFTrsEstimation(int nStartX, int nStartY);
	float	NFTrsEstimationP(int nStartX, int nStartY);
	void	GuideLUTMaker();
	void	GammaLUTMaker(float fParameter);
	void	GuidedFilter();
	void	RestoreImage(IplImage* imInput, IplImage* imOutput);
	void	PostProcessing(IplImage* imInput, IplImage* imOutput);
	bool	Decision(IplImage* imInput, IplImage* imOutput, int nThreshold);

	//////////////////////////////////////////////////////////////////////////
	
	void	LowSpeedGuidedFilter(IplImage* imInput, float* pfTrans, float* pfTransR, int nW, int nH, float nEps);
	void	CalcAcoeff(float* pfSigma, float* pfCov, float* pfA1, float* pfA2, float* pfA3, int nIdx);
	void	BoxFilter(float* pfInImg, int nR, int nWid, int nHei, float*& fOutImg);
	void	BoxFilter(float* pfInImg1, float* pfInImg2, float* pfInImg3, int nR, int m_nWid, int m_nHei, float*& pfOutImg1, float*& pfOutImg2, float*& pfOutImg3);
};
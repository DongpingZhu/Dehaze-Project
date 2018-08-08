#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "guidedfilter.h"
#pragma comment( lib, "opencv_world310d.lib" ) 
using namespace std;
using namespace cv;


int _PriorSize = 15;		//���ڴ�С
double _topbright = 0.001;//������ߵ����ر���
double _w = 0.95;			//w
float t0 = 0.1;			//T(x)����Сֵ   ��Ϊ������txС��0 ����0 Ч������
int SizeH = 0;			//ͼƬ�߶�
int SizeW = 0;			//ͼƬ���
int SizeH_W = 0;			//ͼƬ�е������� �� H*W
Vec<float, 3> a;//ȫ������Ĺ���ֵ
Mat trans_refine;
Mat dark_out1;


char img_name[100] = "ny_night.jpg";//�ļ���


							   //����ͼƬ
Mat ReadImage()
{

	Mat img = imread(img_name);

	SizeH = img.rows;
	SizeW = img.cols;
	SizeH_W = img.rows*img.cols;

	Mat real_img(img.rows, img.cols, CV_32FC3);
	img.convertTo(real_img, CV_32FC3);

	real_img = real_img / 255;

	return real_img;

	//����ͼƬ ����ת��Ϊ3ͨ���ľ���� 
	//����255 ����RBGȷ����0-1֮��
}



//���㰵ͨ��
//J^{dark}(x)=min( min( J^c(y) ) )
Mat DarkChannelPrior(Mat img)
{
	Mat dark = Mat::zeros(img.rows, img.cols, CV_32FC1);//�½�һ������Ԫ��Ϊ0�ĵ�ͨ���ľ���

	for (int i = 0; i<img.rows; i++)
	{
		for (int j = 0; j<img.cols; j++)
		{

			dark.at<float>(i, j) = min(
				min(img.at<Vec<float, 3>>(i, j)[0], img.at<Vec<float, 3>>(i, j)[1]),
				min(img.at<Vec<float, 3>>(i, j)[0], img.at<Vec<float, 3>>(i, j)[2])
				);//����������Сֵ�Ĺ���
		}
	}
	erode(dark, dark_out1, Mat::ones(_PriorSize, _PriorSize, CV_32FC1));//��������и�ʴ �����Ǵ��ڴ�С��ģ������ ,��Ӧ������Сֵ�˲�,�� ��ɫͼ���е�һ���Ķ���

	return dark_out1;//����dark_out1�õ���ȫ�ֱ�������Ϊ�������ط�ҲҪ�õ�
}
Mat DarkChannelPrior_(Mat img)//��������ڼ���tx�õ�����Ϊ����㰵ͨ��һ�����õ�������Сֵ�Ĺ��̣��仯���࣬���Ը��˸ľ���������
{
	double A = (a[0] + a[1] + a[2]) / 3.0;//ȫ���������ֵ �˴���3ͨ����ƽ��ֵ

	Mat dark = Mat::zeros(img.rows, img.cols, CV_32FC1);
	Mat dark_out = Mat::zeros(img.rows, img.cols, CV_32FC1);
	for (int i = 0; i<img.rows; i++)
	{
		for (int j = 0; j<img.cols; j++)
		{
			dark.at<float>(i, j) = min(
				min(img.at<Vec<float, 3>>(i, j)[0] / A, img.at<Vec<float, 3>>(i, j)[1] / A),
				min(img.at<Vec<float, 3>>(i, j)[0] / A, img.at<Vec<float, 3>>(i, j)[2] / A)
				);//ͬ��


		}
	}
	erode(dark, dark_out, Mat::ones(_PriorSize, _PriorSize, CV_32FC1));//ͬ��

	return dark_out;

}


//����A��ֵ
Vec<float, 3> Airlight(Mat img, Mat dark)//vec<float ,3>��ʾ��3����С��vector ����Ϊfloat
{
	int n_bright = _topbright*SizeH_W;

	Mat dark_1 = dark.reshape(1, SizeH_W);//����dark_1��һ����ͼƬ������ô���еľ��� ��������ѭ������

	vector<int> max_idx;

	float max_num = 0;

	Vec<float, 3> A(0, 0, 0);
	Mat RGBPixcels = Mat::ones(n_bright, 1, CV_32FC3);

	for (int i = 0; i<n_bright; i++)
	{
		max_num = 0;
		max_idx.push_back(max_num);
		for (float * p = (float *)dark_1.datastart; p != (float *)dark_1.dataend; p++)
		{
			if (*p>max_num)
			{
				max_num = *p;//��¼���յ����ֵ

				max_idx[i] = (p - (float *)dark_1.datastart);//λ��

				RGBPixcels.at<Vec<float, 3>>(i, 0) = ((Vec<float, 3> *)img.data)[max_idx[i]];//��Ӧ ������ͨ����ֵ��RGBPixcels

			}
		}
		((float *)dark_1.data)[max_idx[i]] = 0;//���ʹ��ı��Ϊ0�������Ͳ����ظ�����
	}


	for (int j = 0; j<n_bright; j++)
	{

		A[0] += RGBPixcels.at<Vec<float, 3>>(j, 0)[0];
		A[1] += RGBPixcels.at<Vec<float, 3>>(j, 0)[1];
		A[2] += RGBPixcels.at<Vec<float, 3>>(j, 0)[2];

	}//������ֵ�ۼ�

	A[0] /= n_bright;
	A[1] /= n_bright;
	A[2] /= n_bright;//��������   ��ȡ���з��ϵĵ��ƽ��ֵ��

	return A;
}


//Calculate Transmission Matrix
Mat TransmissionMat(Mat dark)
{
	double A = (a[0] + a[1] + a[2]) / 3.0;
	for (int i = 0; i < dark.rows; i++)
	{
		for (int j = 0; j < dark.cols; j++)
		{
			double temp = (dark_out1.at<float>(i, j));
			double B = fabs(A - temp);
			//	conut++;
			//cout << conut << endl;
			//if (B==)
			if (B - 0.3137254901960784 < 0.0000000000001)//K=80    80/255=0.31   ���︡����Ҫ����������������ȷ�ıȽ�
			{
				dark.at<float>(i, j) = (1 - _w*dark.at<float>(i, j))*
					(0.3137254901960784 / (B));//�˴�Ϊ�Ĺ���ʽ�Ӳ���
			}
			else
			{
				dark.at<float>(i, j) = 1 - _w*dark.at<float>(i, j);
			}
			if (dark.at<float>(i, j) <= 0.2)//��֤Tx��ʧ�棬��Ϊ�����ϳ����Ľ�����в���
			{
				dark.at<float>(i, j) = 0.5;
			}
			if (dark.at<float>(i, j) >= 1)//ͬ��
			{
				dark.at<float>(i, j) = 1.0;
			}

		}
	}

	return dark;
}
Mat TransmissionMat1(Mat dark)
{
	double A = (a[0] + a[1] + a[2]) / 3.0;
	for (int i = 0; i < dark.rows; i++)
	{
		for (int j = 0; j < dark.cols; j++)
		{

			dark.at<float>(i, j) = (1 - _w*dark.at<float>(i, j));

		}
	}

	return dark;
}
//Calculate Haze Free Image
Mat hazefree(Mat img, Mat t, Vec<float, 3> a, float exposure = 0)//�˴���exposure��ֵ��ʾȥ���Ӧ�ü�����ֵ��
{
	double AAA = a[0];
	if (a[1] > AAA)
		AAA = a[1];
	if (a[2] > AAA)
		AAA = a[2];
	//ȡa�е�����ֵ


	//�¿�һ������
	Mat freeimg = Mat::zeros(SizeH, SizeW, CV_32FC3);
	img.copyTo(freeimg);

	//������������������д�����Բ�������ѭ�����ȽϿ��
	Vec<float, 3> * p = (Vec<float, 3> *)freeimg.datastart;
	float * q = (float *)t.datastart;

	for (; p<(Vec<float, 3> *)freeimg.dataend && q<(float *)t.dataend; p++, q++)
	{
		(*p)[0] = ((*p)[0] - AAA) / std::max(*q, t0) + AAA + exposure;
		(*p)[1] = ((*p)[1] - AAA) / std::max(*q, t0) + AAA + exposure;
		(*p)[2] = ((*p)[2] - AAA) / std::max(*q, t0) + AAA + exposure;
	}

	return freeimg;
}


void printMatInfo(char * name, Mat m)
{
	cout << name << ":" << endl;
	cout << "\t" << "cols=" << m.cols << endl;
	cout << "\t" << "rows=" << m.rows << endl;
	cout << "\t" << "channels=" << m.channels() << endl;
}


//Main Function
int main(int argc, char * argv[])
{
	Mat dark_channel;
	Mat trans;
	Mat img;
	Mat free_img;
	char filename[100];



	while (_access(img_name, 0) != 0)//���ͼƬ�Ƿ����
	{
		std::cout << "The image " << img_name << " don't exist." << endl << "Please enter another one:" << endl;
		cin >> filename;
	}

	clock_t start, finish;
	double duration1, duration3, duration4, duration7;

	//����ͼƬ
	cout << "����ͼƬ ..." << endl;

	start = clock();
	img = ReadImage();
	imshow("ԭͼ", img);
	//printMatInfo("img", img);
	finish = clock();
	duration1 = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Time Cost: " << duration1 << "s" << endl;//�����һ����ʱ��
	cout << endl;

	//���㰵ͨ��
	cout << "���㰵ͨ�� ..." << endl;

	start = clock();
	dark_channel = DarkChannelPrior(img);
	imshow("Dark Channel Prior", dark_channel);
	printMatInfo("dark_channel", dark_channel);
	finish = clock();
	duration3 = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Time Cost: " << duration3 << "s" << endl;
	cout << endl;

	//����ȫ�����ֵ
	cout << "����Aֵ ..." << endl;
	start = clock();
	a = Airlight(img, dark_channel);
	cout << "Airlight:\t" << " B:" << a[0] << " G:" << a[1] << " R:" << a[2] << endl;
	finish = clock();
	duration4 = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Time Cost: " << duration4 << "s" << endl;
	cout << endl;

	//����tx
	cout << "Reading Refine Transmission..." << endl;
	trans_refine = TransmissionMat(DarkChannelPrior_(ReadImage()));
	//printMatInfo("trans_refine", trans_refine);
	//imshow("Refined Transmission Mat",trans_refine);
	cout << endl;

	Mat tran = guidedFilter(img, trans_refine, 60, 0.0001);//�����˲� �õ���ϸ��͸����ͼ
														   //imshow("fitler", tran);

														   //ȥ��

	cout << "Calculating Haze Free Image ..." << endl;
	start = clock();
	free_img = hazefree(img, tran, a, 0);//�˴� �����tran�Ļ����ǵ����˲�����
										 //�����trans_refine ��û���õ����˲� Ч��������ô						�ĺ�
										 /*
										 ������ĸ������������������ȵģ�0.1�ȽϺ�
										 */
	imshow("ȥ���", free_img);


	finish = clock();
	duration7 = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Time Cost: " << duration7 << "s" << endl;
	cout << "Total Time Cost: " << duration1 + duration3 + duration4 + duration7 << "s" << endl;

	//����ͼƬ�Ĵ���

	//imwrite("output.jpg", free_img * 255);

	waitKey();
	cout << endl;

	return 0;
}
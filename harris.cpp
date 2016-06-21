

#include <opencv2\opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

/*
*description:
*	�Ƚ�s1��s2���ȽϽ��д��d
*/
void compareMat(Mat& s1, Mat& s2, Mat& d)
{
	if (d.empty())
	{
		d.create(s1.rows, s1.cols, s1.type());
	}

	for (int y = 0; y < s1.rows; y++)
	{
		for (int x = 0; x < s1.cols; x++)
		{
			if (s1.at<float>(y, x) == s2.at<float>(y, x))
			{
				d.at<float>(y, x) = s1.at<float>(y, x);
			}
			else
			{
				d.at<float>(y, x) = 0.0f;
			}
		}
	}
}


void Harris(vector<Point2i>& corners, Mat& I)
{

	if (!I.data)
	{
		return;
	}

	double GaussKernel[5][5] = {
		0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625,
		0.015625, 0.0625, 0.09375, 0.0625, 0.015625,
		0.0234375, 0.09375, 0.140625, 0.09375, 0.0234375,
		0.015625, 0.0625, 0.09375, 0.0625, 0.015625,
		0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625,
	};

	Mat gray;
	cvtColor(I, gray, CV_BGR2GRAY);

	Mat IxIx(gray.rows, gray.cols, CV_8UC1);
	Mat IyIy(gray.rows, gray.cols, CV_8UC1);
	Mat IxIy(gray.rows, gray.cols, CV_8UC1);
	for (int y = 1; y <= I.rows - 2; y++)
	{
		//const uchar* grayRow = gray.ptr<uchar>(y);
		for (int x = 1; x <= I.cols - 2; x++)
		{
			//���㵱ǰ���������������ƫ����(���ݶ�)
			/*��Ӧʹ��Harris����˽��о����Harris����ˣ�
			[ 0  -1  0 ]
			[ -1  0  1 ]
			[ 0   1  0 ]

			�˴��������Ƚϼ򵥣���˷������
			*/
			uchar ix = gray.at<uchar>(y + 1, x) - gray.at<uchar>(y - 1, x);
			uchar iy = gray.at<uchar>(y, x + 1) - gray.at<uchar>(y, x - 1);

			IxIx.at<uchar>(y, x) = ix * ix;
			IyIy.at<uchar>(y, x) = iy * iy;

			IxIy.at<uchar>(y, x) = ix * iy;
		}
	}

	//Harris��Ӧ����
	Mat R(gray.rows, gray.cols, CV_32FC1);
	//�����Ӧֵ
	float maxResponse = 0.0f;

	for (int y = 0; y <= I.rows - 5; y++)
	{
		for (int x = 0; x <= I.cols - 5; x++)
		{
			uchar a = 0;  /*  �����ƾ��� M��                       */
			uchar b = 0;  /*  [ ��g*a  ��g*c ]   ��  [ ��g*IxIx ��g*IxIy ]        */
			uchar c = 0;  /*  [ ��g*c  ��g*b ]       [ ��g*IxIy ��g*IyIy ]        */
			for (int m = 0; m < 5; m++)
			{
				for (int n = 0; n < 5; n++)
				{
					//�ֱ��IxIx��IyIy��IxIy��5*5��������˹��ֵ����������ƾ��� M
					a += IxIx.at<uchar>(y + m, x + n) * GaussKernel[m][n];
					b += IyIy.at<uchar>(y + m, x + n) * GaussKernel[m][n];
					c += IxIy.at<uchar>(y + m, x + n) * GaussKernel[m][n];
				}
			}

			//���㵱ǰ���ص�Harris��Ӧֵ
			float t = (a * c - b * b) - 0.05f * (a + c) * (a + c);

			R.at<float>(y, x) = t;

			if (t > maxResponse)
			{
				maxResponse = t;
			}
		}
	}

	/*
	Ϊ����þֲ�����ֵ���ȶ�R�������ͣ������ͽ����R�Ƚϣ���ͬ�ľ��Ǿֲ�����ֵ
	*/
	Mat dilated;
	dilate(R, dilated, Mat());
	Mat localMax;
	compareMat(R, dilated, localMax);
	
	//�޳����־ֲ�����ֵ��������Ϊ�����������ֲ�����ֵ�п��ܺܶ࣬������Ҫ�Ľ��
	Mat threshold2;
	threshold(localMax, threshold2, 0.5 * maxResponse, 255, THRESH_BINARY);
	//imshow("localMax", threshold2);

	for (int y = 0; y < threshold2.rows - 1; y++)
	{
		const float* row = threshold2.ptr<float>(y);
		for (int x = 0; x < threshold2.cols - 1; x++)
		{
			if (row[x])
			{
				corners.push_back(Point2i(x, y));
			}
		}
	}
	
}



int main()
{

	Mat m = imread("C:\\Users\\WZTCM\\Pictures\\IMG_44451.jpg");
	

	vector<Point2i> corners;
	Harris(corners, m);
	
	for (vector<Point2i>::iterator it = corners.begin(); it != corners.end(); it++)
	{
		circle(m, *it, 2, Scalar(255, 242, 0)); 
	}

	imshow("ok", m);

	waitKey();
	return 0;
}
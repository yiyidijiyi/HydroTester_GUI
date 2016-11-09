/*
* �������ڣ�2016-10-31
* ����޸ģ�2016-11-01
* ��      �ߣ�syf
* ��      ����
*/
#include "imageproc.h"

/*
* ������
* ���أ�
* ���ܣ����캯��
*/
ImageProc::ImageProc(QObject *parent)
	: QObject(parent)
	, m_pCamera(NULL)
	, m_bClacFlag(false)
{
	m_iRoiX = 622;			// �����������X����
	m_iRoiY = 522;			// �����������Y����
	m_iRoiRadius = 495;		// �������뾶
	m_dDropArea = 0.5;		// ˮ�������ֵ��ʵ���������λmm��
	m_iDropArea = m_dDropArea / 0.12;		// ˮ�������ֵ�����ص������	
	m_iAdapArea = 25;		// ����Ӧ��ֵ���
	m_iAdapParam = 5;		// ����Ӧ��ֵ������
	m_iDropNum = 0;			// ��⵽��ˮ�����

	m_iMinDis = 400;		// ˮ��֮�����С����ƽ��
	m_iReduceDis = 35;		// ���������������

	// ����ʱ�����м�����ͼ��
	m_mat1 = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1, Scalar::all(0));
	m_mat2 = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1, Scalar::all(0));

	// ������ģ��
	m_matRoi = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1, Scalar::all(0));
	circle(m_matRoi, Point(m_iRoiX, m_iRoiY), m_iRoiRadius - 5, Scalar::all(255), -1, 8);
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
ImageProc::~ImageProc()
{

}


/*
* ������
* ���أ�
* ���ܣ�ˮ��ʶ���㷨
*/
void ImageProc::Calc(Mat &image)
{
	// �ж�ͼ�������Ƿ�Ϊ��
	if (image.data == NULL)
	{
		return;
	}

	//qDebug() << "image process thread:" << QThread::currentThreadId() << endl;

	image.copyTo(m_matShow);

	if (m_bClacFlag)
	{
		int val = 0;
		int roi_num = 0;
		int64 roi_gray = 0;
		int roi_gray_ave = 0;
		int dropNum = 0;
		unsigned int n = 0;

		//Mat image1(m_matShow.rows, m_matShow.cols, CV_8UC1, Scalar::all(0));
		//Mat image2(m_matShow.rows, m_matShow.cols, CV_8UC1, Scalar::all(0));

		// ��������������������������ֵ��Ϊ0
		for (int i = 0; i < m_matShow.rows; i++)
		{
			for (int j = 0; j < m_matShow.cols; j++)
			{
				if (m_matRoi.at<uchar>(i, j) > 0)
				{
					val = m_matShow.at<uchar>(i, j);
					roi_num++;
					roi_gray += val;
					m_mat1.at<uchar>(i, j) = val;
				}
				else
				{
					m_matShow.at<uchar>(i, j) = 0;
					m_mat1.at<uchar>(i, j) = 0;
				}
			}
		}

		roi_gray_ave = roi_gray / roi_num;
		cv::blur(m_mat1, m_mat2, Size(3, 3));
		cv::medianBlur(m_mat2, m_mat1, 3);

		// ��ֵ������
		if (roi_gray_ave > 90)
		{
			cv::adaptiveThreshold(m_mat1, m_mat2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, m_iAdapArea, -0.045*roi_gray_ave*m_iAdapParam);
		}
		if (roi_gray_ave > 70)
		{
			cv::adaptiveThreshold(m_mat1, m_mat2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, m_iAdapArea, -0.055*roi_gray_ave*m_iAdapParam);
		}
		if (roi_gray_ave > 50)
		{
			cv::adaptiveThreshold(m_mat1, m_mat2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, m_iAdapArea, -0.07*roi_gray_ave*m_iAdapParam);
		}
		if (roi_gray_ave > 30)
		{
			cv::adaptiveThreshold(m_mat1, m_mat2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, m_iAdapArea, -0.1*roi_gray_ave*m_iAdapParam);
		}
		else
		{
			cv::adaptiveThreshold(m_mat1, m_mat2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, m_iAdapArea, -0.2*roi_gray_ave*m_iAdapParam);
		}

		// ��̬ѧ����
		Mat elem = cv::getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(m_mat2, m_mat1, elem, Point(-1, -1), 1);
		erode(m_mat1, m_mat2, elem, Point(-1, -1), 1);

		// ��ȡÿ������ÿ�����ص�  
		vector<vector<Point>> contours;
		cv::findContours(m_mat2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
		dropNum = m_vecDropPos.size();

		int dropArea = m_iDropArea;
		if (roi_gray_ave < 35)
		{
			dropArea *= 2;
		}

		n = contours.size();
		for (unsigned int i = 0; i < n; i++)
		{
			CvRect rect = boundingRect(contours[i]);
			int contArea = max(rect.width, rect.height);
			Point point;

			if (contArea > dropArea)
			{
				point.x = rect.x + rect.width / 2;
				point.y = rect.y + rect.height / 2;
				if (pow((point.x - m_iRoiX), 2) + pow((point.y - m_iRoiY), 2) < pow((m_iRoiRadius - m_iReduceDis), 2))
				{
					if (dropNum > 0)
					{
						// ���㵱ǰˮ��������ˮ��֮�����С����
						int distance = 500;
						int k = 0;
						unsigned int m = m_vecDropPos.size();
						for (unsigned int j = 0; j < m; j++)
						{
							if (distance > sqrt(pow((point.x - m_vecDropPos[j].x), 2) + pow((point.y - m_vecDropPos[j].y), 2)))
							{
								distance = sqrt(pow((point.x - m_vecDropPos[j].x), 2) + pow((point.y - m_vecDropPos[j].y), 2));
								k = j;
							}

						}

						// ��ǰˮ��������ˮ��ľ������20����Ϊ���µ�ˮ�飬
						// ������Ϊ��ԭ��ˮ��λ��ƫ�ƣ����µ����긲��ԭ������
						if (distance > 20)
						{
							dropNum++;
							m_vecDropPos.push_back(point);
						}
						else
						{
							m_vecDropPos[k] = point;
						}
					}
					else
					{
						dropNum++;
						m_vecDropPos.push_back(point);
					}
				}
			}
		}

		if (dropNum>0)
		{
			for (unsigned int j = 0; j < m_vecDropPos.size(); j++)
			{
				circle(m_matShow, m_vecDropPos[j], 20, Scalar::all(255), 2, 8, 0);
				char a[10];
				_itoa_s(j + 1, a, 10, 10);
				const char *dropn = a;
				putText(m_matShow, dropn, Point(m_vecDropPos[j].x, m_vecDropPos[j].y - 25), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0), 2, 8, 0);//��ͼƬ������ַ� 
			}
		}

		m_iDropNum = dropNum;
		if (m_iDropNum >= 3)
		{
			Size dsize = Size(m_matShow.cols / 2, m_matShow.rows / 2);
			Mat tmpImage;
			cv::resize(m_matShow, tmpImage, dsize);
			imwrite("report.bmp", tmpImage);
		}
	}
	else
	{
		circle(m_matShow, cvPoint(m_iRoiX, m_iRoiY), m_iRoiRadius, Scalar::all(255), 2, 8, 0);  //����Ȥ����ȷ��Բ�����ĸ���Ȥ����ʱ��
	}

	emit ImageProcessed();
}

/*
* ������
* ���أ�
* ���ܣ�ʶ���㷨��ʼ����
*/
void ImageProc::InitCalc()
{
	m_iDropNum = 0;
	m_vecDropPos.clear();
}


/*
* ������
* ���أ�
* ���ܣ�ʶ���㷨��ʼ����
*/
void ImageProc::StartCalc()
{
	m_bClacFlag = true;
}


/*
* ������
* ���أ�
* ���ܣ�ʶ���㷨��������
*/
void ImageProc::StopCalc()
{
	m_bClacFlag = false;
}


/*
* ������
* ���أ�
* ���ܣ�
*/
void ImageProc::SetCamera(MerCamera *pCamera)
{
	if (m_pCamera)
	{
		disconnect(m_pCamera, &MerCamera::ImageSnaped, this, &ImageProc::OnImagePrepared);
	}
	else
	{
		m_pCamera = pCamera;
		connect(m_pCamera, &MerCamera::ImageSnaped, this, &ImageProc::OnImagePrepared);
	}
}


/*
* ������
* ���أ�
* ���ܣ���ȡ�������ͼ������
*/
Mat& ImageProc::GetImage()
{
	return m_matShow;
}


/*
* ������
* ���أ�
* ���ܣ���ȡʶ��ˮ����Ŀ
*/
int ImageProc::GetDropNum()
{
	return m_iDropNum;
}


/*
* ������
* ���أ�
* ���ܣ��ۺ��������ͼ���Ѳɼ����ɿ�ʼʶ��ˮ��
*/
void ImageProc::OnImagePrepared()
{
	assert(m_pCamera != NULL);
	Calc(m_pCamera->GetMat());
}


/*
* ������
* ���أ�
* ���ܣ�
*/


/*
* ������
* ���أ�
* ���ܣ�
*/

/*
* ������
* ���أ�
* ���ܣ�
*/
/*
* 创建日期：2016-10-31
* 最后修改：2016-11-01
* 作      者：syf
* 描      述：
*/
#include "imageproc.h"

/*
* 参数：
* 返回：
* 功能：构造函数
*/
ImageProc::ImageProc(QObject *parent)
	: QObject(parent)
	, m_pCamera(NULL)
	, m_bClacFlag(false)
{
	m_iRoiX = 622;			// 检测区域中心X坐标
	m_iRoiY = 522;			// 检测区域中心Y坐标
	m_iRoiRadius = 495;		// 检测区域半径
	m_dDropArea = 0.5;		// 水珠面积阈值（实际面积，单位mm）
	m_iDropArea = m_dDropArea / 0.12;		// 水珠面积阈值（像素点个数）	
	m_iAdapArea = 25;		// 自适应阈值面积
	m_iAdapParam = 5;		// 自适应阈值灵敏度
	m_iDropNum = 0;			// 检测到的水珠个数

	m_iMinDis = 400;		// 水珠之间的最小距离平方
	m_iReduceDis = 35;		// 检测区域缩减距离

	// 运算时保存中间结果的图像
	m_mat1 = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1, Scalar::all(0));
	m_mat2 = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1, Scalar::all(0));

	// 检测局域模板
	m_matRoi = Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1, Scalar::all(0));
	circle(m_matRoi, Point(m_iRoiX, m_iRoiY), m_iRoiRadius - 5, Scalar::all(255), -1, 8);
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
ImageProc::~ImageProc()
{

}


/*
* 参数：
* 返回：
* 功能：水珠识别算法
*/
void ImageProc::Calc(Mat &image)
{
	// 判断图像数据是否为空
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

		// 将检测区域外的其他部分像素数值置为0
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

		// 阈值化处理
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

		// 形态学处理
		Mat elem = cv::getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(m_mat2, m_mat1, elem, Point(-1, -1), 1);
		erode(m_mat1, m_mat2, elem, Point(-1, -1), 1);

		// 获取每个轮廓每个像素点  
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
						// 计算当前水珠与已有水珠之间的最小距离
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

						// 当前水珠与已有水珠的距离大于20，认为是新的水珠，
						// 否则认为是原有水珠位置偏移，用新的坐标覆盖原有坐标
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
				putText(m_matShow, dropn, Point(m_vecDropPos[j].x, m_vecDropPos[j].y - 25), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0), 2, 8, 0);//在图片中输出字符 
			}
		}

		m_iDropNum = dropNum;
		if (m_iDropNum >= 3)
		{
			Size dsize = Size(m_matShow.cols / 2, m_matShow.rows / 2);
			Mat tmpImage;
			cv::resize(m_matShow, tmpImage, dsize);
			imwrite("./report/report.bmp", tmpImage);
		}
	}
	else
	{
		circle(m_matShow, cvPoint(m_iRoiX, m_iRoiY), m_iRoiRadius, Scalar::all(255), 2, 8, 0);  //感兴趣区域确认圆，更改感兴趣区域时用
	}

	emit ImageProcessed();
}

/*
* 参数：
* 返回：
* 功能：识别算法开始运行
*/
void ImageProc::InitCalc()
{
	m_iDropNum = 0;
	m_vecDropPos.clear();
}


/*
* 参数：
* 返回：
* 功能：识别算法开始运行
*/
void ImageProc::StartCalc()
{
	m_bClacFlag = true;
}


/*
* 参数：
* 返回：
* 功能：识别算法结束运行
*/
void ImageProc::StopCalc()
{
	m_bClacFlag = false;
}


/*
* 参数：
* 返回：
* 功能：
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
* 参数：
* 返回：
* 功能：获取处理过的图像数据
*/
Mat& ImageProc::GetImage()
{
	return m_matShow;
}


/*
* 参数：
* 返回：
* 功能：获取识别水珠数目
*/
int ImageProc::GetDropNum()
{
	return m_iDropNum;
}


/*
* 参数：
* 返回：
* 功能：槽函数，相机图像已采集，可开始识别水珠
*/
void ImageProc::OnImagePrepared()
{
	assert(m_pCamera != NULL);
	Calc(m_pCamera->GetMat());
}


/*
* 参数：
* 返回：
* 功能：
*/


/*
* 参数：
* 返回：
* 功能：
*/

/*
* 参数：
* 返回：
* 功能：
*/
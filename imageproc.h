/*
* 创建日期：2016-10-31
* 最后修改：2016-11-01
* 作      者：syf
* 描      述：
*/
#ifndef IMAGEPROC_H
#define IMAGEPROC_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <cassert>
#include "common.h"
#include "mercamera.h"

using namespace cv;
using namespace std;

class ImageProc : public QObject
{
	Q_OBJECT

public:
	ImageProc(QObject *parent = 0);
	~ImageProc();

private:
	void Calc(Mat &image);
	

public:
	void InitCalc();
	void StartCalc();
	void StopCalc();
	void SetCamera(MerCamera *pCamera);
	Mat& GetImage();
	int GetDropNum();

public slots:
	void OnImagePrepared();
	

signals:
	void ImageProcessed();
private:
	bool  m_bClacFlag;

	MerCamera *m_pCamera;

	int m_iRoiX;					// 检测区域中心X坐标
	int m_iRoiY;					// 检测区域中心Y坐标
	int m_iRoiRadius;			// 检测区域半径
	int m_iDropArea;			// 水珠面积阈值（像素点个数）
	double m_dDropArea;	// 水珠面积阈值（实际面积，单位mm）
	int m_iAdapArea;			// 自适应阈值面积
	int m_iAdapParam;			// 自适应阈值灵敏度
	int m_iDropNum;			// 检测到的水珠个数
	int m_iMinDis;				// 水珠之间的最小距离平方
	int m_iReduceDis;			// 检测区域缩减距离
	vector<Point> m_vecDropPos;		// 水珠中心坐标

	Mat m_matShow;
	Mat m_matRoi;
	Mat m_mat1;
	Mat m_mat2;
};

#endif // IMAGEPROC_H

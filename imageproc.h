/*
* �������ڣ�2016-10-31
* ����޸ģ�2016-11-01
* ��      �ߣ�syf
* ��      ����
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
	void UpdateParams(STRUCT_AdvanceParams &params);
	Mat& GetImage();
	int GetDropNum();

public slots:
	void OnImagePrepared();
	

signals:
	void ImageProcessed();
private:
	bool  m_bClacFlag;

	MerCamera *m_pCamera;

	int m_iRoiX;					// �����������X����
	int m_iRoiY;					// �����������Y����
	int m_iRoiRadius;			// �������뾶
	int m_iDropArea;			// ˮ�������ֵ�����ص������
	double m_dDropArea;	// ˮ�������ֵ��ʵ���������λmm��
	int m_iAdapArea;			// ����Ӧ��ֵ���
	int m_iDensity;
	int m_iAdapParam;		// ����Ӧ��ֵ������
	int m_iDropNum;			// ��⵽��ˮ�����
	int m_iMinDis;				// ˮ��֮�����С����ƽ��
	int m_iReduceDis;			// ���������������
	vector<Point> m_vecDropPos;		// ˮ����������

	Mat m_matShow;
	Mat m_matRoi;
	Mat m_mat1;
	Mat m_mat2;
};

#endif // IMAGEPROC_H

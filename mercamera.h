/*
* 创建日期：2016-09-21
* 最后修改：2016-09-22
* 作      者：syf
* 描      述：
*/
#ifndef MERCAMERAL_H
#define MERCAMERAL_H

#include <QObject>
#include <QTextCodec>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "GxIAPI.h"
#include "DxImageProc.h"

using namespace cv;

///< 错误提示函数宏定义
#define  GX_VERIFY(emStatus) \
if (emStatus != GX_STATUS_SUCCESS)\
{\
	DecodeErrorStatus(emStatus); \
	return FALSE; \
}

///< 判断返回值宏定义
#define VERIFY_STATUS_RET(emStatus) \
if (emStatus != GX_STATUS_SUCCESS) \
{\
	return emStatus; \
}

class CVxCriticalSection
{
public:
	/// 构造
	CVxCriticalSection() { InitializeCriticalSection(&m_cs); }
	/// 析构
	virtual ~CVxCriticalSection() { DeleteCriticalSection(&m_cs); }
public:
	/// 进入临界区
	void Enter(){ EnterCriticalSection(&m_cs); }
	/// 离开临界区
	void Leave(){ LeaveCriticalSection(&m_cs); }
private:
	CRITICAL_SECTION m_cs; ///< 临界段变量	
};



class MerCamera : public QObject
{
	Q_OBJECT

public:
	enum ENUM_SnapMode{
		ClosedMode	= 0,
		ContinuousMode = 1,
		SoftwareTriggerMode = 2
	};

	MerCamera(QObject *parent = 0);
	~MerCamera();

	// 回调函数
	static void __stdcall OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame);

	// 获取设备的宽高等属性信息
	GX_STATUS GetDeviceParam();

	// 设置相机的数据格式,使其输出图像位深为8-bit
	GX_STATUS SetPixelFormat8bit();

	// 设备初始化
	GX_STATUS InitDevice(void);

	//  为彩色图像显示准备资源
	bool PrepareForShowColorImg();

	// 为黑白图像显示准备资源
	bool PrepareForShowMonoImg();

	// 为图像显示准备资源,分配Buffer
	bool PrepareForShowImg();

	// 释放为图像显示准备的资源
	void UnPrepareForShowImg();

	// 打开相机
	BOOL OpenCamera(void);

	// 关闭相机，并释放占用资源
	BOOL CloseCamera(void);

	// 开始采集
	BOOL StartSnap(ENUM_SnapMode mode);

	// 停止采集
	BOOL StopSnap(ENUM_SnapMode mode);

	// 软件触发图像采集
	BOOL SoftwareTrigger(void);

	// 设置自动白平衡
	BOOL SetAutoWhiteBalance(GX_BALANCE_WHITE_AUTO_ENTRY mode);

	// 设置白平衡系数
	BOOL SetBalanceRatio(GX_BALANCE_RATIO_SELECTOR_ENTRY channel, double val);

	// 获取白平衡系数
	BOOL GetBalanceRatio(GX_BALANCE_RATIO_SELECTOR_ENTRY channel, double *val);

	// 设置黑电平通道通道
	BOOL SetBlackLevelSelector(GX_BLACKLEVEL_SELECTOR_ENTRY selector);

	// 设置自动黑电平
	BOOL SetAutoBlackLevel(GX_BLACKLEVEL_AUTO_ENTRY mode);

	// 设置增益通道
	BOOL SetGainSelector(GX_GAIN_SELECTOR_ENTRY selector);

	// 设置自动增益
	BOOL SetAutoGain(GX_GAIN_AUTO_ENTRY mode);

	// 设定增益值
	BOOL SetGain(double gian);

	// 获取增益值
	BOOL GetGain(double *gain);

	// 设置自动曝光
	BOOL SetAutoExposure(GX_EXPOSURE_AUTO_ENTRY mode);

	// 设置曝光时间
	BOOL SetExposure(double t);

	// 获取曝光时间
	BOOL GetExposure(double *t);

	// 设置自动曝光的期望灰度值
	BOOL SetExpectedGray(int64_t gray);

	// 相机是否已打开
	BOOL IsCameraOpened();

	// 连接mat的图像数据
	void BitMap2Mat(BYTE* pBuffer);

	// 获取Mat类型的图像
	Mat& GetMat();

	// 复位图像数据采集标志
	void ResetImgSnapedFlag();

	// 创建avi视频
	BOOL CreateAviVideo(const QString &path);

	// 释放打开的avi视频
	void ReleaseAviVideo();

	// 向avi文件中写入一帧视频
	void WriteFrame2Video(const Mat &frame);
signals:
	void ImageSnaped();

private:
	void DecodeErrorStatus(GX_STATUS emErrorStatus);

private:
	QStringList m_messageList;
	//----------------大恒相机图像采集相关成员--------------------------
	GX_DEV_HANDLE     m_hDevice;                ///< 设备句柄
	int64_t                 m_nImageWidth;            ///< 相机输出图像宽度
	int64_t                 m_nImageHeight;           ///< 相机输出图像高度
	int64_t                 m_nPayLoadSize;           ///< 设备输出原始图像大小
	int64_t                 m_nPixelColorFilter;       ///< 彩色相机的Bayer格式

	GX_FLOAT_RANGE          m_stShutterFloatRange;    ///< 保存曝光时间范围
	GX_FLOAT_RANGE          m_stGainFloatRange;       ///< 保存增益值范围           

	BITMAPINFO             *m_pBmpInfo;	              ///< BITMAPINFO 结构指针，显示图像时使用
	GX_FRAME_DATA		m_stFrameData;			  ///< GetImage获取的图像地址

	BOOL               m_bDevOpened;             ///< 标识设备是否已打开
	BOOL               m_bDevSnaped;                ///< 标识设备是否已开采
	bool					m_bIsImgSnaped;		///< 图像数据采集标志
	bool                 m_bIsColorFilter;         ///< 判断相机是否支持Bayer格式
	bool                 m_bImplementAutoGain;     ///< 是否支持自动增益
	bool                 m_bImplementAutoShutter;  ///< 是否支持自动曝光
	bool                 m_bImplementLight;        ///< 是否支持2A光照环境功能
	bool					m_bIsSingleSnap;		  ///< 标识设备是否单词采集	
	UINT					m_nEditTimeOut;		      ///< 单次触发采集超时时间	
	ENUM_SnapMode	m_snapMode;
	Mat 					m_image;				  ///< Mat类型图像矩阵

	BYTE                   *m_pRawBuffer;             ///< 存放原始RAW图的Buffer
	BYTE                   *m_pImgBuffer;             ///< 指向经过处理后的图像数据缓冲区
	char                    m_chBmpBuf[2048];         ///< BIMTAPINFO存储缓冲区，m_pBmpInfo即指向此缓冲区
	GX_EXPOSURE_AUTO_ENTRY  m_emAutoShutterMode;      ///< 自动曝光模式
	GX_GAIN_AUTO_ENTRY      m_emAutoGainMode;         ///< 自动增益模式

	//----------------AVI 视频保存相关成员--------------------------
	VideoWriter m_videoWriter;

	int		m_nGray;               ///< 期望灰度值
	int		m_nRoiX;               ///< 2A感兴趣区域X坐标
	int		m_nRoiY;               ///< 2A感兴趣区域Y坐标
	int		m_nRoiH;               ///< 2A感兴趣区域高
	int		m_nRoiW;               ///< 2A感兴趣区域宽
	double	m_dAutoGainMin;        ///< 自动增益最小值
	double  m_dAutoGainMax;        ///< 自动增益最大值
	double	m_dAutoShutterMin;     ///< 自动曝光最小值
	double	m_dAutoShutterMax;     ///< 自动曝光最大值
	double	m_dShutterValue;       ///< 曝光值
	double	m_dGainValue;          ///< 增益值  
};

#endif // MERCAMERAL_H

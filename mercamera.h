/*
* �������ڣ�2016-09-21
* ����޸ģ�2016-09-22
* ��      �ߣ�syf
* ��      ����
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

///< ������ʾ�����궨��
#define  GX_VERIFY(emStatus) \
if (emStatus != GX_STATUS_SUCCESS)\
{\
	DecodeErrorStatus(emStatus); \
	return FALSE; \
}

///< �жϷ���ֵ�궨��
#define VERIFY_STATUS_RET(emStatus) \
if (emStatus != GX_STATUS_SUCCESS) \
{\
	return emStatus; \
}

class CVxCriticalSection
{
public:
	/// ����
	CVxCriticalSection() { InitializeCriticalSection(&m_cs); }
	/// ����
	virtual ~CVxCriticalSection() { DeleteCriticalSection(&m_cs); }
public:
	/// �����ٽ���
	void Enter(){ EnterCriticalSection(&m_cs); }
	/// �뿪�ٽ���
	void Leave(){ LeaveCriticalSection(&m_cs); }
private:
	CRITICAL_SECTION m_cs; ///< �ٽ�α���	
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

	// �ص�����
	static void __stdcall OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame);

	// ��ȡ�豸�Ŀ�ߵ�������Ϣ
	GX_STATUS GetDeviceParam();

	// ������������ݸ�ʽ,ʹ�����ͼ��λ��Ϊ8-bit
	GX_STATUS SetPixelFormat8bit();

	// �豸��ʼ��
	GX_STATUS InitDevice(void);

	//  Ϊ��ɫͼ����ʾ׼����Դ
	bool PrepareForShowColorImg();

	// Ϊ�ڰ�ͼ����ʾ׼����Դ
	bool PrepareForShowMonoImg();

	// Ϊͼ����ʾ׼����Դ,����Buffer
	bool PrepareForShowImg();

	// �ͷ�Ϊͼ����ʾ׼������Դ
	void UnPrepareForShowImg();

	// �����
	BOOL OpenCamera(void);

	// �ر���������ͷ�ռ����Դ
	BOOL CloseCamera(void);

	// ��ʼ�ɼ�
	BOOL StartSnap(ENUM_SnapMode mode);

	// ֹͣ�ɼ�
	BOOL StopSnap(ENUM_SnapMode mode);

	// �������ͼ��ɼ�
	BOOL SoftwareTrigger(void);

	// �����Զ���ƽ��
	BOOL SetAutoWhiteBalance(GX_BALANCE_WHITE_AUTO_ENTRY mode);

	// ���ð�ƽ��ϵ��
	BOOL SetBalanceRatio(GX_BALANCE_RATIO_SELECTOR_ENTRY channel, double val);

	// ��ȡ��ƽ��ϵ��
	BOOL GetBalanceRatio(GX_BALANCE_RATIO_SELECTOR_ENTRY channel, double *val);

	// ���úڵ�ƽͨ��ͨ��
	BOOL SetBlackLevelSelector(GX_BLACKLEVEL_SELECTOR_ENTRY selector);

	// �����Զ��ڵ�ƽ
	BOOL SetAutoBlackLevel(GX_BLACKLEVEL_AUTO_ENTRY mode);

	// ��������ͨ��
	BOOL SetGainSelector(GX_GAIN_SELECTOR_ENTRY selector);

	// �����Զ�����
	BOOL SetAutoGain(GX_GAIN_AUTO_ENTRY mode);

	// �趨����ֵ
	BOOL SetGain(double gian);

	// ��ȡ����ֵ
	BOOL GetGain(double *gain);

	// �����Զ��ع�
	BOOL SetAutoExposure(GX_EXPOSURE_AUTO_ENTRY mode);

	// �����ع�ʱ��
	BOOL SetExposure(double t);

	// ��ȡ�ع�ʱ��
	BOOL GetExposure(double *t);

	// �����Զ��ع�������Ҷ�ֵ
	BOOL SetExpectedGray(int64_t gray);

	// ����Ƿ��Ѵ�
	BOOL IsCameraOpened();

	// ����mat��ͼ������
	void BitMap2Mat(BYTE* pBuffer);

	// ��ȡMat���͵�ͼ��
	Mat& GetMat();

	// ��λͼ�����ݲɼ���־
	void ResetImgSnapedFlag();

	// ����avi��Ƶ
	BOOL CreateAviVideo(const QString &path);

	// �ͷŴ򿪵�avi��Ƶ
	void ReleaseAviVideo();

	// ��avi�ļ���д��һ֡��Ƶ
	void WriteFrame2Video(const Mat &frame);
signals:
	void ImageSnaped();

private:
	void DecodeErrorStatus(GX_STATUS emErrorStatus);

private:
	QStringList m_messageList;
	//----------------������ͼ��ɼ���س�Ա--------------------------
	GX_DEV_HANDLE     m_hDevice;                ///< �豸���
	int64_t                 m_nImageWidth;            ///< ������ͼ����
	int64_t                 m_nImageHeight;           ///< ������ͼ��߶�
	int64_t                 m_nPayLoadSize;           ///< �豸���ԭʼͼ���С
	int64_t                 m_nPixelColorFilter;       ///< ��ɫ�����Bayer��ʽ

	GX_FLOAT_RANGE          m_stShutterFloatRange;    ///< �����ع�ʱ�䷶Χ
	GX_FLOAT_RANGE          m_stGainFloatRange;       ///< ��������ֵ��Χ           

	BITMAPINFO             *m_pBmpInfo;	              ///< BITMAPINFO �ṹָ�룬��ʾͼ��ʱʹ��
	GX_FRAME_DATA		m_stFrameData;			  ///< GetImage��ȡ��ͼ���ַ

	BOOL               m_bDevOpened;             ///< ��ʶ�豸�Ƿ��Ѵ�
	BOOL               m_bDevSnaped;                ///< ��ʶ�豸�Ƿ��ѿ���
	bool					m_bIsImgSnaped;		///< ͼ�����ݲɼ���־
	bool                 m_bIsColorFilter;         ///< �ж�����Ƿ�֧��Bayer��ʽ
	bool                 m_bImplementAutoGain;     ///< �Ƿ�֧���Զ�����
	bool                 m_bImplementAutoShutter;  ///< �Ƿ�֧���Զ��ع�
	bool                 m_bImplementLight;        ///< �Ƿ�֧��2A���ջ�������
	bool					m_bIsSingleSnap;		  ///< ��ʶ�豸�Ƿ񵥴ʲɼ�	
	UINT					m_nEditTimeOut;		      ///< ���δ����ɼ���ʱʱ��	
	ENUM_SnapMode	m_snapMode;
	Mat 					m_image;				  ///< Mat����ͼ�����

	BYTE                   *m_pRawBuffer;             ///< ���ԭʼRAWͼ��Buffer
	BYTE                   *m_pImgBuffer;             ///< ָ�򾭹�������ͼ�����ݻ�����
	char                    m_chBmpBuf[2048];         ///< BIMTAPINFO�洢��������m_pBmpInfo��ָ��˻�����
	GX_EXPOSURE_AUTO_ENTRY  m_emAutoShutterMode;      ///< �Զ��ع�ģʽ
	GX_GAIN_AUTO_ENTRY      m_emAutoGainMode;         ///< �Զ�����ģʽ

	//----------------AVI ��Ƶ������س�Ա--------------------------
	VideoWriter m_videoWriter;

	int		m_nGray;               ///< �����Ҷ�ֵ
	int		m_nRoiX;               ///< 2A����Ȥ����X����
	int		m_nRoiY;               ///< 2A����Ȥ����Y����
	int		m_nRoiH;               ///< 2A����Ȥ�����
	int		m_nRoiW;               ///< 2A����Ȥ�����
	double	m_dAutoGainMin;        ///< �Զ�������Сֵ
	double  m_dAutoGainMax;        ///< �Զ��������ֵ
	double	m_dAutoShutterMin;     ///< �Զ��ع���Сֵ
	double	m_dAutoShutterMax;     ///< �Զ��ع����ֵ
	double	m_dShutterValue;       ///< �ع�ֵ
	double	m_dGainValue;          ///< ����ֵ  
};

#endif // MERCAMERAL_H

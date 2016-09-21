/*
* �������ڣ�2016-09-21
* ����޸ģ�2016-09-21
* ��      �ߣ�syf
* ��      ����
*/
#include "mercamera.h"


/*
* ������
* ���أ�
* ���ܣ����캯��
*/
MerCamera::MerCamera(QObject *parent)
	: QObject(parent)
	, m_bDevOpened(FALSE)
	, m_bIsSnap(FALSE)
	, m_bIsImgSnaped(false)
	, m_bIsSingleSnap(false)
	, m_snapMode(ClosedMode)
	, m_bIsColorFilter(false)
	, m_bImplementAutoGain(false)
	, m_bImplementAutoShutter(false)
	, m_bImplementLight(false)
	, m_nGray(0)
	, m_nRoiX(0)
	, m_nRoiW(0)
	, m_nRoiH(0)
	, m_nRoiY(0)
	, m_dAutoGainMin(0)
	, m_dAutoShutterMax(0)
	, m_dAutoGainMax(0)
	, m_dAutoShutterMin(0)
	, m_nImageWidth(0)
	, m_nImageHeight(0)
	, m_nPayLoadSize(0)
	, m_nPixelColorFilter(0)
	, m_pRawBuffer(NULL)
	, m_pImgBuffer(NULL)
	, m_pBmpInfo(NULL)
	, m_hDevice(NULL)
	, m_emAutoShutterMode(GX_EXPOSURE_AUTO_OFF)
	, m_emAutoGainMode(GX_GAIN_AUTO_OFF)
{
	m_dShutterValue = 0.0;

	// ��ʼ������
	memset(&m_stShutterFloatRange, 0, sizeof(m_stShutterFloatRange));
	memset(&m_stFrameData, 0, sizeof(GX_FRAME_DATA));
	memset(m_chBmpBuf, 0, sizeof(m_chBmpBuf));


	GX_STATUS emStatus = GX_STATUS_ERROR;
	emStatus = GXInitLib();

	if (emStatus != GX_STATUS_SUCCESS)
	{
		DecodeErrorStatus(emStatus);
		m_messageList.push_front(QStringLiteral("��ʼ�������ʧ�ܣ�"));
	}
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
MerCamera::~MerCamera()
{

}


/*
* ������emErrorStatus:������ⷵ�صĲ�����Ϣ
* ���أ�
* ���ܣ�������ⷵ�صĴ�����Ϣ������Ϊ�ַ�����ʽ
*/
void MerCamera::DecodeErrorStatus(GX_STATUS emErrorStatus)
{
	char*     pchErrorInfo = NULL;
	size_t    nSize = 0;
	GX_STATUS emStatus = GX_STATUS_ERROR;
	m_messageList.clear();

	// ��ȡ������Ϣ���ȣ��������ڴ���Դ
	emStatus = GXGetLastError(&emErrorStatus, NULL, &nSize);
	pchErrorInfo = new char[nSize];
	if (NULL == pchErrorInfo)
	{
		return;
	}

	// ��ȡ������Ϣ������ʾ
	emStatus = GXGetLastError(&emErrorStatus, pchErrorInfo, &nSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		m_messageList.append(QStringLiteral("GXGetLastError�ӿڵ���ʧ�ܣ�"));
	}
	else
	{
		QTextCodec *pCodec = QTextCodec::codecForName("GB18030");
		m_messageList.append(pCodec->toUnicode(pchErrorInfo));
	}

	// �ͷ�������ڴ���Դ
	if (NULL != pchErrorInfo)
	{
		delete[] pchErrorInfo;
		pchErrorInfo = NULL;
	}
}


/*
* ������pFrame������ɼ��ص�����
* ���أ�
* ���ܣ�����ɼ��ص�����
*/
void __stdcall MerCamera::OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame)
{
	MerCamera *pCamera = (MerCamera*)(pFrame->pUserParam);
	VxUint32 nImageHeight = (VxUint32)pCamera->m_nImageHeight;
	VxUint32 nImageWidth = (VxUint32)pCamera->m_nImageWidth;

	if (0 == pFrame->status)
	{
		if (!pCamera->m_bIsImgSnaped)	// ���ϴβɼ���ͼ������֮ǰ�����ڲɼ��µ�ͼ��
		{
			

			if (pCamera->m_bIsColorFilter)
			{
				memcpy(pCamera->m_pRawBuffer, pFrame->pImgBuf, pFrame->nImgSize);

				// ��ɫͼ����Ҫ��Raw8ͼ�񾭹�RGBת������ʾ
				DxRaw8toRGB24(pCamera->m_pRawBuffer, pCamera->m_pImgBuffer, nImageWidth, nImageHeight, RAW2RGB_NEIGHBOUR,
					DX_PIXEL_COLOR_FILTER(pCamera->m_nPixelColorFilter), true);
			}
			else
			{
				// �ڰ������Ҫ��ת���ݺ���ʾ
				//memcpy(pCamera->m_pRawBuffer, pFrame->pImgBuf, pFrame->nImgSize);
				//for (VxUint32 i = 0; i < nImageHeight; i++)
				//{
				//	memcpy(pCamera->m_pImgBuffer + i * nImageWidth, pCamera->m_pRawBuffer + (nImageHeight - i - 1) * nImageWidth, (size_t)nImageWidth);
				//}

				// �ڰ������ֱ�ӻ�ȡͼ������
				memcpy(pCamera->m_pImgBuffer, pFrame->pImgBuf, pFrame->nImgSize);
			}

			pCamera->BitMap2Mat(pCamera->m_pImgBuffer);
			pCamera->m_bIsImgSnaped = true;
			emit pCamera->ImageSnaped();
		}
	}
}

/*
* ������
* ���أ�
* ���ܣ���ȡ�豸��ߵ�������Ϣ
*/
GX_STATUS MerCamera::GetDeviceParam()
{
	GX_STATUS emStatus = GX_STATUS_ERROR;

	// ��ȡͼ���С
	emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &m_nPayLoadSize);
	VERIFY_STATUS_RET(emStatus);

	// ��ȡ���
	emStatus = GXGetInt(m_hDevice, GX_INT_WIDTH, &m_nImageWidth);
	VERIFY_STATUS_RET(emStatus);

	// ��ȡ�߶�
	emStatus = GXGetInt(m_hDevice, GX_INT_HEIGHT, &m_nImageHeight);
	VERIFY_STATUS_RET(emStatus);

	// �ж�����Ƿ�֧��bayer��ʽ
	emStatus = GXIsImplemented(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &m_bIsColorFilter);
	VERIFY_STATUS_RET(emStatus);

	// ֧�ֱ�ʾ�����ɫͼ��
	if (m_bIsColorFilter)
	{
		emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &m_nPixelColorFilter);
	}

	return emStatus;
}


/*
* ������
* ���أ�
* ���ܣ�������������ݸ�ʽΪ8Bit
*/
GX_STATUS MerCamera::SetPixelFormat8bit()
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	int64_t   nPixelSize = 0;
	uint32_t  nEnmuEntry = 0;
	size_t    nBufferSize = 0;
	//	BOOL      bIs8bit = TRUE;

	GX_ENUM_DESCRIPTION  *pEnumDescription = NULL;
	//	GX_ENUM_DESCRIPTION  *pEnumTemp = NULL;

	// ��ȡ���ص��С
	emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_SIZE, &nPixelSize);
	VERIFY_STATUS_RET(emStatus);

	// �ж�Ϊ8bitʱֱ�ӷ���,��������Ϊ8bit
	if (nPixelSize == GX_PIXEL_SIZE_BPP8)
	{
		return GX_STATUS_SUCCESS;
	}
	else
	{
		// ��ȡ�豸֧�ֵ����ظ�ʽö����
		emStatus = GXGetEnumEntryNums(m_hDevice, GX_ENUM_PIXEL_FORMAT, &nEnmuEntry);
		VERIFY_STATUS_RET(emStatus);

		// Ϊ��ȡ�豸֧�ֵ����ظ�ʽö��ֵ׼����Դ
		nBufferSize = nEnmuEntry * sizeof(GX_ENUM_DESCRIPTION);
		pEnumDescription = new GX_ENUM_DESCRIPTION[nEnmuEntry];

		// ��ȡ֧�ֵ�ö��ֵ
		emStatus = GXGetEnumDescription(m_hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription, &nBufferSize);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			if (pEnumDescription != NULL)
			{
				delete[]pEnumDescription;
				pEnumDescription = NULL;
			}

			return emStatus;
		}

		// �����豸֧�ֵ����ظ�ʽ,�������ظ�ʽΪ8bit,
		// ���豸֧�ֵ����ظ�ʽΪMono10��Mono8��������ΪMono8
		for (uint32_t i = 0; i<nEnmuEntry; i++)
		{
			if ((pEnumDescription[i].nValue & GX_PIXEL_8BIT) == GX_PIXEL_8BIT)
			{
				emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription[i].nValue);
				break;
			}
		}

		// �ͷ���Դ
		if (pEnumDescription != NULL)
		{
			delete[]pEnumDescription;
			pEnumDescription = NULL;
		}
	}

	return emStatus;
}


/*
* ������
* ���أ�
* ���ܣ���ʼ�����
*/
GX_STATUS MerCamera::InitDevice(void)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	//���òɼ�ģʽ�����ɼ�
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
	VERIFY_STATUS_RET(emStatus);

	// ���ô�������Ϊ��
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
	VERIFY_STATUS_RET(emStatus);

	// ��֪��ǰ���֧���ĸ�8λͼ�����ݸ�ʽ����ֱ������
	// �����豸֧�����ݸ�ʽGX_PIXEL_FORMAT_BAYER_GR8,��ɰ������´���ʵ��
	// emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, GX_PIXEL_FORMAT_BAYER_GR8);
	// VERIFY_STATUS_RET(emStatus);

	// �������ǰ��������ݸ�ʽʱ�����Ե������º�����ͼ�����ݸ�ʽ����Ϊ8Bit
	emStatus = SetPixelFormat8bit();

	return emStatus;
}


/*
* ������
* ���أ�true:Ϊͼ����ʾ׼����Դ�ɹ�  false:׼����Դʧ��
* ���ܣ�Ϊ��ɫͼ����ʾ׼����Դ,����Buffer
*/
bool  MerCamera::PrepareForShowColorImg()
{
	//--------------------------------------------------------------------
	//---------------------------��ʼ��bitmapͷ---------------------------
	m_pBmpInfo = (BITMAPINFO *)m_chBmpBuf;
	m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biWidth = (LONG)m_nImageWidth;
	m_pBmpInfo->bmiHeader.biHeight = (LONG)m_nImageHeight;

	m_pBmpInfo->bmiHeader.biPlanes = 1;
	m_pBmpInfo->bmiHeader.biBitCount = 24; // ��ɫͼ��Ϊ24,�ڰ�ͼ��Ϊ8
	m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage = 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biClrUsed = 0;
	m_pBmpInfo->bmiHeader.biClrImportant = 0;


	//----------------------------Ϊͼ�����ݷ���Buffer---------------------

	// Ϊ����RGBת�����ͼ�����Buffer
	m_pImgBuffer = new BYTE[(size_t)(m_nImageWidth * m_nImageHeight * 3)];
	if (m_pImgBuffer == NULL)
	{
		return false;
	}

	// ΪMat�������ռ�
	m_image.create(m_nImageHeight, m_nImageWidth, CV_8UC3);

	return true;
}


/*
* ������
* ���أ�true:Ϊͼ����ʾ׼����Դ�ɹ�  false:׼����Դʧ��
* ���ܣ�Ϊ�ڰ�ͼ����ʾ׼����Դ,����Buffer
*/
bool  MerCamera::PrepareForShowMonoImg()
{
	//--------------------------------------------------------------------
	//---------------------------��ʼ��bitmapͷ---------------------------
	m_pBmpInfo = (BITMAPINFO *)m_chBmpBuf;
	m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biWidth = (LONG)m_nImageWidth;
	m_pBmpInfo->bmiHeader.biHeight = (LONG)m_nImageHeight;

	m_pBmpInfo->bmiHeader.biPlanes = 1;
	m_pBmpInfo->bmiHeader.biBitCount = 8;  // ��ɫͼ��Ϊ24,�ڰ�ͼ��Ϊ8
	m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage = 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biClrUsed = 0;
	m_pBmpInfo->bmiHeader.biClrImportant = 0;

	if (!m_bIsColorFilter)
	{
		// �ڰ������Ҫ���г�ʼ����ɫ�����
		for (int i = 0; i < 256; i++)
		{
			m_pBmpInfo->bmiColors[i].rgbBlue = i;
			m_pBmpInfo->bmiColors[i].rgbGreen = i;
			m_pBmpInfo->bmiColors[i].rgbRed = i;
			m_pBmpInfo->bmiColors[i].rgbReserved = i;
		}
	}

	//---------------------------------------------------------------------
	//----------------------------Ϊͼ�����ݷ���Buffer---------------------
	// �ڰ�ͼ��Buffer����
	m_pImgBuffer = new BYTE[(size_t)(m_nImageWidth * m_nImageHeight)];
	if (m_pImgBuffer == NULL)
	{
		return false;
	}

	m_image.create(m_nImageHeight, m_nImageWidth, CV_8UC1);

	return true;
}


/*
* ������
* ���أ�true:Ϊͼ����ʾ׼����Դ�ɹ�  false:׼����Դʧ��
* ���ܣ�Ϊ����ͼ�����Buffer,Ϊͼ����ʾ׼����Դ
*/
bool  MerCamera::PrepareForShowImg()
{
	bool bRet = true;

	// Ϊ�洢ԭʼͼ�����Buffer
	m_pRawBuffer = new BYTE[(size_t)m_nPayLoadSize];
	m_stFrameData.pImgBuf = m_pRawBuffer;
	if (m_pRawBuffer == NULL)
	{
		bRet = false;
	}

	if (m_bIsColorFilter)
	{
		// Ϊ��ɫͼ����ʾ׼����Դ,����Buffer
		bRet = PrepareForShowColorImg();
	}
	else
	{
		// Ϊ�ڰ�ͼ����ʾ׼����Դ,����Buffer
		bRet = PrepareForShowMonoImg();
	}

	// ������ʧ�����ͷ��ѷ������Դ
	if (!bRet)
	{
		UnPrepareForShowImg();
	}

	return bRet;
}


/*
* ������
* ���أ�
* ���ܣ��ͷ�Ϊͼ����ʾ׼������Դ
*/
void MerCamera::UnPrepareForShowImg()
{
	if (m_pRawBuffer != NULL)
	{
		delete[]m_pRawBuffer;
		m_pRawBuffer = NULL;
		m_stFrameData.pImgBuf = NULL;
	}

	if (m_pImgBuffer != NULL)
	{
		delete[]m_pImgBuffer;
		m_pImgBuffer = NULL;
	}

	if (m_image.data)
	{
		m_image.release();
	}
}


/*
* ������
* ���أ�1:�ɹ���0��ʧ��
* ���ܣ������
*/
BOOL MerCamera::OpenCamera(void)
{
	GX_STATUS     emStatus = GX_STATUS_SUCCESS;
	uint32_t      nDevNum = 0;
	GX_OPEN_PARAM stOpenParam;

	// ���ԭ����Ϣ
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	// ��ʼ���豸�򿪲���
	stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
	stOpenParam.openMode = GX_OPEN_INDEX;
	stOpenParam.pszContent = (char *)("1");

	// ö���豸
	emStatus = GXUpdateDeviceList(&nDevNum, 1000);
	GX_VERIFY(emStatus);

	// �жϵ�ǰ�����豸����
	if (nDevNum <= 0)
	{
		m_messageList.append(QStringLiteral("δ��������豸��"));

		return FALSE;
	}

	// ����豸�Ѿ�����ر�,��֤����ڳ�ʼ������������ٴδ�
	if (m_hDevice != NULL)
	{
		emStatus = GXCloseDevice(m_hDevice);
		GX_VERIFY(emStatus);
		m_hDevice = NULL;
	}

	// ���豸
	emStatus = GXOpenDevice(&stOpenParam, &m_hDevice);
	GX_VERIFY(emStatus);
	m_bDevOpened = TRUE;

	// ���������Ĭ�ϲ���:�ɼ�ģʽ:�����ɼ�,�������ģʽ������أ����ݸ�ʽ:8-bit
	emStatus = InitDevice();
	GX_VERIFY(emStatus);

	// ��ȡ�豸�Ŀ��ߵ�������Ϣ
	emStatus = GetDeviceParam();
	GX_VERIFY(emStatus);

	// ���ð�ƽ��ϵ��
	//SetBalanceRatio(0, 1.18);
	//SetBalanceRatio(1, 1.0);
	//SetBalanceRatio(2, 1.36);

	return TRUE;
}


/*
* ������
* ���أ�
* ���ܣ��ر�������ͷ��ڴ�ռ�
*/
BOOL MerCamera::CloseCamera(void)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	// ������ڲɼ�ʱ����ֹͣ�ɼ�
	if (m_bIsSnap)
	{
		//����ֹͣ�ɼ�����
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			// ������Ϣ
			m_messageList.append(QStringLiteral("ֹͣ�ɼ�ʧ�ܣ��޷������ر������"));
			return FALSE;
		}

		// �����ɼ�����ע���ص��������ͷ�Ϊ�ɼ�ͼ�񿪱ٵ�Buffer
		if (ContinuousMode == m_snapMode)
		{
			//ע���ص�
			emStatus = GXUnregisterCaptureCallback(m_hDevice);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// ������Ϣ
				m_messageList.append(QStringLiteral("ע���ص�����ʧ�ܣ��޷������ر������"));
				return FALSE;
			}

			m_snapMode = ClosedMode;
		}

		// �ͷ�Ϊ�ɼ�ͼ�񿪱ٵ�ͼ��Buffer
		UnPrepareForShowImg();

		m_bIsSnap = FALSE;
		//m_bIsSingleSnap = FALSE;
	}

	emStatus = GXCloseDevice(m_hDevice);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		// ������Ϣ
		m_messageList.append(QStringLiteral("�޷������ر������"));
		return FALSE;
	}

	m_bDevOpened = FALSE;
	m_hDevice = NULL;

	return TRUE;
}


/*
* ������mode���ɼ�ģʽ
* ���أ�
* ���ܣ���ʼ�ɼ�
*/
BOOL MerCamera::StartSnap(ENUM_SnapMode mode)
{
	GX_STATUS emStatus = GX_STATUS_ERROR;
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	// ��ʼ��BMPͷ��Ϣ������BufferΪͼ��ɼ���׼��
	if (!PrepareForShowImg())
	{
		m_messageList.append(QStringLiteral("Ϊͼ����ʾ�����ڴ�ռ�ʧ��!"));
		return FALSE;
	}

	if (ContinuousMode == mode)
	{
		//ע��ص�
		emStatus = GXRegisterCaptureCallback(m_hDevice, this, OnFrameCallbackFun);
		GX_VERIFY(emStatus);
	}

	//����ʼ�ɼ�����
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_START);
	GX_VERIFY(emStatus);

	m_bIsSnap = TRUE;
	m_snapMode = mode;

	return TRUE;
}


/*
* ������mode���ɼ�ģʽ
* ���أ�
* ���ܣ�ֹͣ�ɼ�
*/
BOOL MerCamera::StopSnap(ENUM_SnapMode mode)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	//����ֹͣ�ɼ�����
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
	GX_VERIFY(emStatus);

	if (ContinuousMode == mode)
	{
		//ע���ص�
		emStatus = GXUnregisterCaptureCallback(m_hDevice);
		GX_VERIFY(emStatus);
	}

	// �ͷ�Ϊ�ɼ�ͼ�񿪱ٵ�ͼ��Buffer
	UnPrepareForShowImg();

	m_bIsSnap = FALSE;
	m_snapMode = ClosedMode;
	return TRUE;
}


/*
* ������
* ���أ�
* ���ܣ��������ͼ��ɼ�
*/
BOOL MerCamera::SoftwareTrigger(void)
{
	GX_STATUS emStatus = GX_STATUS_ERROR;
	int timeout = 1000;
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}


	//ÿ�η��ʹ�������֮ǰ��ղɼ��������
	//��ֹ���ڲ�����֡����ɱ���GXGetImage�õ���ͼ�����ϴη��ʹ����õ���ͼ
	emStatus = GXFlushQueue(m_hDevice);
	GX_VERIFY(emStatus);

	//������������
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_TRIGGER_SOFTWARE);
	GX_VERIFY(emStatus);


	//��ȡͼ��
	emStatus = GXGetImage(m_hDevice, &m_stFrameData, timeout);

	//�ж�GXGetImage����ֵ
	GX_VERIFY(emStatus);

	BitMap2Mat((BYTE *)m_stFrameData.pImgBuf);
	m_bIsImgSnaped = true;
	emit ImageSnaped();

	return TRUE;
}


/*
* ������mode:��ƽ�⹤��ģʽ
* ���أ�
* ���ܣ������Զ���ƽ��
*/
BOOL MerCamera::SetAutoWhiteBalance(ENUM_WorkingMode mode)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	// 0-off, 1-continuous, 2-once
	int64_t nAwbMode = static_cast<int64_t>(mode);

	// 0-adaptive, 1-D65, 2-Fluoresence, Incandescent
	int64_t nAwbLightMode = 0;

	// ���ð�ƽ��
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_WHITE_AUTO, nAwbMode);
	GX_VERIFY(emStatus);

	if (nAwbMode > 0)
	{
		// ���ð�ƽ���Դ
		emStatus = GXSetEnum(m_hDevice, GX_ENUM_AWB_LAMP_HOUSE, nAwbLightMode);
		GX_VERIFY(emStatus);
	}

	return TRUE;
}


/*
* ������channel:��ƽ��ͨ����
*			val��ͨ����ƽ��ֵ
* ���أ�
* ���ܣ�
*/
BOOL MerCamera::SetBalanceRatio(ENUM_WhiteBalanceChannel channel, double val)
{
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// �ر��Զ���ƽ��
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_WHITE_AUTO, 0);
	GX_VERIFY(emStatus);


	// ѡ���ƽ��ͨ��
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR,static_cast<int64_t>(channel));
	GX_VERIFY(emStatus);

	emStatus = GXSetFloat(m_hDevice, GX_FLOAT_BALANCE_RATIO, val);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* ������channel:��ƽ��ͨ��
*			val�������ͨ����ƽ��ֵ
* ���أ�
* ���ܣ�
*/
BOOL MerCamera::GetBalanceRatio(ENUM_WhiteBalanceChannel channel, double *val)
{
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// ѡ���ƽ��ͨ��
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, static_cast<int64_t>(channel));
	GX_VERIFY(emStatus);

	emStatus = GXGetFloat(m_hDevice, GX_FLOAT_BALANCE_RATIO, val);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* ������mode���ع⹤��ģʽ
* ���أ�
* ���ܣ������ع⹤��ģʽ
*/
BOOL MerCamera::SetAutoExposure(ENUM_WorkingMode mode)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	// 0--off��1--continuous, 2-once
	int64_t  nExposureMode = static_cast<int64_t>(mode);

	emStatus = GXSetEnum(m_hDevice, GX_ENUM_EXPOSURE_AUTO, nExposureMode);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* ������t:�ع�ʱ��
* ���أ�
* ���ܣ������ع�ʱ��
*/
BOOL MerCamera::SetExposure(double t)
{
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	GX_STATUS status = GX_STATUS_SUCCESS;

	status = GXSetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, t);
	GX_VERIFY(status);

	return TRUE;
}


/*
* ������t:������ع�ʱ��
* ���أ�
* ���ܣ���ȡ������ع�ʱ��
*/
BOOL MerCamera::GetExposure(double *t)
{
	m_messageList.clear();

	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("�豸�����Ч��"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXGetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, t);;

	return TRUE;
}


/*
* ������pBuffer��ͼ�������ڴ�ռ�
* ���أ�
* ���ܣ���ͼ�����ݣ����ӵ�Mat��data����
*/
void MerCamera::BitMap2Mat(BYTE* pBuffer)
{
	m_image.data = pBuffer;
}


/*
* ������
* ���أ�Mat��ʽ��ͼ��
* ���ܣ�
*/
Mat& MerCamera::GetMat()
{
	return m_image;
}


/*
* ������
* ���أ�
* ���ܣ���λͼ�����ݲɼ���־
*/
void MerCamera::ResetImgSnapedFlag()
{
	m_bIsImgSnaped = false;
}


/*
* ������path:avi�ļ�·��
* ���أ�
* ���ܣ�����avi��Ƶ
*/
BOOL MerCamera::CreateAviVideo(const QString &path)
{
	string stdPath = path.toStdString();

	// �ȹر��Ѵ򿪵���Ƶ
	m_videoWriter.release();

	if (m_videoWriter.open(stdPath, CV_FOURCC('M', 'J', 'P', 'G'), 1.0, Size(m_nImageWidth, m_nImageHeight), false))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*
* ������
* ���أ�
* ���ܣ��ͷŴ򿪵�avi�ļ�
*/
void MerCamera::ReleaseAviVideo()
{
	if (m_videoWriter.isOpened())
	{
		m_videoWriter.release();
	}
}


/*
* ������frame:һ֡ͼ��
* ���أ�
* ���ܣ���avi�ļ���д��һ֡��Ƶ
*/
void MerCamera::WriteFrame2Video(const Mat &frame)
{
	if (m_videoWriter.isOpened())
	{
		m_videoWriter.write(frame);
	}
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

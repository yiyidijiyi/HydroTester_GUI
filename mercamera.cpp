/*
* 创建日期：2016-09-21
* 最后修改：2016-09-22
* 作      者：syf
* 描      述：
*/
#include "mercamera.h"


/*
* 参数：
* 返回：
* 功能：构造函数
*/
MerCamera::MerCamera(QObject *parent)
	: QObject(parent)
	, m_bDevOpened(FALSE)
	, m_bDevSnaped(FALSE)
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

	// 初始化参数
	memset(&m_stShutterFloatRange, 0, sizeof(m_stShutterFloatRange));
	memset(&m_stFrameData, 0, sizeof(GX_FRAME_DATA));
	memset(m_chBmpBuf, 0, sizeof(m_chBmpBuf));


	GX_STATUS emStatus = GX_STATUS_ERROR;
	emStatus = GXInitLib();

	if (emStatus != GX_STATUS_SUCCESS)
	{
		DecodeErrorStatus(emStatus);
		m_messageList.push_front(QStringLiteral("初始化相机库失败！"));
	}
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
MerCamera::~MerCamera()
{
	if (m_bDevOpened)
	{
		CloseCamera();
	}
}


/*
* 参数：emErrorStatus:有相机库返回的操作信息
* 返回：
* 功能：将相机库返回的错误信息，解析为字符串形式
*/
void MerCamera::DecodeErrorStatus(GX_STATUS emErrorStatus)
{
	char*     pchErrorInfo = NULL;
	size_t    nSize = 0;
	GX_STATUS emStatus = GX_STATUS_ERROR;
	m_messageList.clear();

	// 获取错误信息长度，并申请内存资源
	emStatus = GXGetLastError(&emErrorStatus, NULL, &nSize);
	pchErrorInfo = new char[nSize];
	if (NULL == pchErrorInfo)
	{
		return;
	}

	// 获取错误信息，并显示
	emStatus = GXGetLastError(&emErrorStatus, pchErrorInfo, &nSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		m_messageList.append(QStringLiteral("GXGetLastError接口调用失败！"));
	}
	else
	{
		QTextCodec *pCodec = QTextCodec::codecForName("GB18030");
		m_messageList.append(pCodec->toUnicode(pchErrorInfo));
	}

	// 释放申请的内存资源
	if (NULL != pchErrorInfo)
	{
		delete[] pchErrorInfo;
		pchErrorInfo = NULL;
	}
}


/*
* 参数：pFrame：相机采集回调参数
* 返回：
* 功能：相机采集回调函数
*/
void __stdcall MerCamera::OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame)
{
	MerCamera *pCamera = (MerCamera*)(pFrame->pUserParam);
	VxUint32 nImageHeight = (VxUint32)pCamera->m_nImageHeight;
	VxUint32 nImageWidth = (VxUint32)pCamera->m_nImageWidth;

	if (0 == pFrame->status)
	{
		if (!pCamera->m_bIsImgSnaped)	// 正上次采集的图像处理完之前，不在采集新的图像
		{
			

			if (pCamera->m_bIsColorFilter)
			{
				memcpy(pCamera->m_pRawBuffer, pFrame->pImgBuf, pFrame->nImgSize);

				// 彩色图像需要将Raw8图像经过RGB转换后显示
				DxRaw8toRGB24(pCamera->m_pRawBuffer, pCamera->m_pImgBuffer, nImageWidth, nImageHeight, RAW2RGB_NEIGHBOUR,
					DX_PIXEL_COLOR_FILTER(pCamera->m_nPixelColorFilter), true);
			}
			else
			{
				// 黑白相机需要翻转数据后显示
				//memcpy(pCamera->m_pRawBuffer, pFrame->pImgBuf, pFrame->nImgSize);
				//for (VxUint32 i = 0; i < nImageHeight; i++)
				//{
				//	memcpy(pCamera->m_pImgBuffer + i * nImageWidth, pCamera->m_pRawBuffer + (nImageHeight - i - 1) * nImageWidth, (size_t)nImageWidth);
				//}

				// 黑白相机，直接获取图像数据
				memcpy(pCamera->m_pImgBuffer, pFrame->pImgBuf, pFrame->nImgSize);
			}

			pCamera->BitMap2Mat(pCamera->m_pImgBuffer);
			pCamera->m_bIsImgSnaped = true;
			emit pCamera->ImageSnaped();
		}
	}
}

/*
* 参数：
* 返回：
* 功能：获取设备宽高等属性信息
*/
GX_STATUS MerCamera::GetDeviceParam()
{
	GX_STATUS emStatus = GX_STATUS_ERROR;

	// 获取图像大小
	emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &m_nPayLoadSize);
	VERIFY_STATUS_RET(emStatus);

	// 获取宽度
	emStatus = GXGetInt(m_hDevice, GX_INT_WIDTH, &m_nImageWidth);
	VERIFY_STATUS_RET(emStatus);

	// 获取高度
	emStatus = GXGetInt(m_hDevice, GX_INT_HEIGHT, &m_nImageHeight);
	VERIFY_STATUS_RET(emStatus);

	// 判断相机是否支持bayer格式
	emStatus = GXIsImplemented(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &m_bIsColorFilter);
	VERIFY_STATUS_RET(emStatus);

	// 支持表示输出彩色图像
	if (m_bIsColorFilter)
	{
		emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &m_nPixelColorFilter);
	}

	return emStatus;
}


/*
* 参数：
* 返回：
* 功能：设置相机的数据格式为8Bit
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

	// 获取像素点大小
	emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_SIZE, &nPixelSize);
	VERIFY_STATUS_RET(emStatus);

	// 判断为8bit时直接返回,否则设置为8bit
	if (nPixelSize == GX_PIXEL_SIZE_BPP8)
	{
		return GX_STATUS_SUCCESS;
	}
	else
	{
		// 获取设备支持的像素格式枚举数
		emStatus = GXGetEnumEntryNums(m_hDevice, GX_ENUM_PIXEL_FORMAT, &nEnmuEntry);
		VERIFY_STATUS_RET(emStatus);

		// 为获取设备支持的像素格式枚举值准备资源
		nBufferSize = nEnmuEntry * sizeof(GX_ENUM_DESCRIPTION);
		pEnumDescription = new GX_ENUM_DESCRIPTION[nEnmuEntry];

		// 获取支持的枚举值
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

		// 遍历设备支持的像素格式,设置像素格式为8bit,
		// 如设备支持的像素格式为Mono10和Mono8则设置其为Mono8
		for (uint32_t i = 0; i<nEnmuEntry; i++)
		{
			if ((pEnumDescription[i].nValue & GX_PIXEL_8BIT) == GX_PIXEL_8BIT)
			{
				emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription[i].nValue);
				break;
			}
		}

		// 释放资源
		if (pEnumDescription != NULL)
		{
			delete[]pEnumDescription;
			pEnumDescription = NULL;
		}
	}

	return emStatus;
}


/*
* 参数：
* 返回：
* 功能：初始化相机
*/
GX_STATUS MerCamera::InitDevice(void)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	//设置采集模式连续采集
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
	VERIFY_STATUS_RET(emStatus);

	// 设置触发开关为关
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
	VERIFY_STATUS_RET(emStatus);

	// 已知当前相机支持哪个8位图像数据格式可以直接设置
	// 例如设备支持数据格式GX_PIXEL_FORMAT_BAYER_GR8,则可按照以下代码实现
	// emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, GX_PIXEL_FORMAT_BAYER_GR8);
	// VERIFY_STATUS_RET(emStatus);

	// 不清楚当前相机的数据格式时，可以调用以下函数将图像数据格式设置为8Bit
	emStatus = SetPixelFormat8bit();

	return emStatus;
}


/*
* 参数：
* 返回：true:为图像显示准备资源成功  false:准备资源失败
* 功能：为彩色图像显示准备资源,分配Buffer
*/
bool  MerCamera::PrepareForShowColorImg()
{
	//--------------------------------------------------------------------
	//---------------------------初始化bitmap头---------------------------
	m_pBmpInfo = (BITMAPINFO *)m_chBmpBuf;
	m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biWidth = (LONG)m_nImageWidth;
	m_pBmpInfo->bmiHeader.biHeight = (LONG)m_nImageHeight;

	m_pBmpInfo->bmiHeader.biPlanes = 1;
	m_pBmpInfo->bmiHeader.biBitCount = 24; // 彩色图像为24,黑白图像为8
	m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage = 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biClrUsed = 0;
	m_pBmpInfo->bmiHeader.biClrImportant = 0;


	//----------------------------为图像数据分配Buffer---------------------

	// 为经过RGB转换后的图像分配Buffer
	m_pImgBuffer = new BYTE[(size_t)(m_nImageWidth * m_nImageHeight * 3)];
	if (m_pImgBuffer == NULL)
	{
		return false;
	}

	// 为Mat对象分配空间
	m_image.create(m_nImageHeight, m_nImageWidth, CV_8UC3);

	return true;
}


/*
* 参数：
* 返回：true:为图像显示准备资源成功  false:准备资源失败
* 功能：为黑白图像显示准备资源,分配Buffer
*/
bool  MerCamera::PrepareForShowMonoImg()
{
	//--------------------------------------------------------------------
	//---------------------------初始化bitmap头---------------------------
	m_pBmpInfo = (BITMAPINFO *)m_chBmpBuf;
	m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biWidth = (LONG)m_nImageWidth;
	m_pBmpInfo->bmiHeader.biHeight = (LONG)m_nImageHeight;

	m_pBmpInfo->bmiHeader.biPlanes = 1;
	m_pBmpInfo->bmiHeader.biBitCount = 8;  // 彩色图像为24,黑白图像为8
	m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage = 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biClrUsed = 0;
	m_pBmpInfo->bmiHeader.biClrImportant = 0;

	if (!m_bIsColorFilter)
	{
		// 黑白相机需要进行初始化调色板操作
		for (int i = 0; i < 256; i++)
		{
			m_pBmpInfo->bmiColors[i].rgbBlue = i;
			m_pBmpInfo->bmiColors[i].rgbGreen = i;
			m_pBmpInfo->bmiColors[i].rgbRed = i;
			m_pBmpInfo->bmiColors[i].rgbReserved = i;
		}
	}

	//---------------------------------------------------------------------
	//----------------------------为图像数据分配Buffer---------------------
	// 黑白图像Buffer分配
	m_pImgBuffer = new BYTE[(size_t)(m_nImageWidth * m_nImageHeight)];
	if (m_pImgBuffer == NULL)
	{
		return false;
	}

	m_image.create(m_nImageHeight, m_nImageWidth, CV_8UC1);

	return true;
}


/*
* 参数：
* 返回：true:为图像显示准备资源成功  false:准备资源失败
* 功能：为保存图像分配Buffer,为图像显示准备资源
*/
bool  MerCamera::PrepareForShowImg()
{
	bool bRet = true;

	// 为存储原始图像分配Buffer
	m_pRawBuffer = new BYTE[(size_t)m_nPayLoadSize];
	m_stFrameData.pImgBuf = m_pRawBuffer;
	if (m_pRawBuffer == NULL)
	{
		bRet = false;
	}

	if (m_bIsColorFilter)
	{
		// 为彩色图像显示准备资源,分配Buffer
		bRet = PrepareForShowColorImg();
	}
	else
	{
		// 为黑白图像显示准备资源,分配Buffer
		bRet = PrepareForShowMonoImg();
	}

	// 若分配失败则释放已分配的资源
	if (!bRet)
	{
		UnPrepareForShowImg();
	}

	return bRet;
}


/*
* 参数：
* 返回：
* 功能：释放为图像显示准备的资源
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
* 参数：
* 返回：1:成功，0：失败
* 功能：打开相机
*/
BOOL MerCamera::OpenCamera(void)
{
	GX_STATUS     emStatus = GX_STATUS_SUCCESS;
	uint32_t      nDevNum = 0;
	GX_OPEN_PARAM stOpenParam;

	// 清除原有信息
	m_messageList.clear();

	// 初始化设备打开参数
	stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
	stOpenParam.openMode = GX_OPEN_INDEX;
	stOpenParam.pszContent = (char *)("1");

	// 枚举设备
	emStatus = GXUpdateDeviceList(&nDevNum, 1000);
	GX_VERIFY(emStatus);

	// 判断当前连接设备个数
	if (nDevNum <= 0)
	{
		m_messageList.append(QStringLiteral("未发现相机设备！"));

		return FALSE;
	}

	// 如果设备已经打开则关闭,保证相机在初始化出错情况能再次打开
	if (m_hDevice != NULL)
	{
		emStatus = GXCloseDevice(m_hDevice);
		GX_VERIFY(emStatus);
		m_hDevice = NULL;
	}

	// 打开设备
	emStatus = GXOpenDevice(&stOpenParam, &m_hDevice);
	GX_VERIFY(emStatus);
	m_bDevOpened = TRUE;

	// 设置相机的默认参数:采集模式:连续采集,软件触发模式：开或关，数据格式:8-bit
	emStatus = InitDevice();
	GX_VERIFY(emStatus);

	// 获取设备的宽、高等属性信息
	emStatus = GetDeviceParam();
	GX_VERIFY(emStatus);

	// 设置白平衡系数
	//SetBalanceRatio(0, 1.18);
	//SetBalanceRatio(1, 1.0);
	//SetBalanceRatio(2, 1.36);

	return TRUE;
}


/*
* 参数：
* 返回：
* 功能：关闭相机，释放内存空间
*/
BOOL MerCamera::CloseCamera(void)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	// 相机正在采集时则先停止采集
	if (m_bDevSnaped)
	{
		//发送停止采集命令
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			// 错误信息
			m_messageList.append(QStringLiteral("停止采集失败，无法正常关闭相机！"));
			return FALSE;
		}

		// 连续采集，需注销回调函数，释放为采集图像开辟的Buffer
		if (ContinuousMode == m_snapMode)
		{
			//注销回调
			emStatus = GXUnregisterCaptureCallback(m_hDevice);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// 错误信息
				m_messageList.append(QStringLiteral("注销回调函数失败，无法正常关闭相机！"));
				return FALSE;
			}

			m_snapMode = ClosedMode;
		}

		// 释放为采集图像开辟的图像Buffer
		UnPrepareForShowImg();

		m_bDevSnaped = FALSE;
		//m_bIsSingleSnap = FALSE;
	}

	emStatus = GXCloseDevice(m_hDevice);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		// 错误信息
		m_messageList.append(QStringLiteral("无法正常关闭相机！"));
		return FALSE;
	}

	m_bDevOpened = FALSE;
	m_hDevice = NULL;

	return TRUE;
}


/*
* 参数：mode：采集模式
* 返回：
* 功能：开始采集
*/
BOOL MerCamera::StartSnap(ENUM_SnapMode mode)
{
	GX_STATUS emStatus = GX_STATUS_ERROR;
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	// 初始化BMP头信息、分配Buffer为图像采集做准备
	if (!PrepareForShowImg())
	{
		m_messageList.append(QStringLiteral("为图像显示分配内存空间失败!"));
		return FALSE;
	}

	if (ContinuousMode == mode)
	{
		//注册回调
		emStatus = GXRegisterCaptureCallback(m_hDevice, this, OnFrameCallbackFun);
		GX_VERIFY(emStatus);
	}

	//发开始采集命令
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_START);
	GX_VERIFY(emStatus);

	m_bDevSnaped = TRUE;
	m_snapMode = mode;

	return TRUE;
}


/*
* 参数：mode：采集模式
* 返回：
* 功能：停止采集
*/
BOOL MerCamera::StopSnap(ENUM_SnapMode mode)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	//发送停止采集命令
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
	GX_VERIFY(emStatus);

	if (ContinuousMode == mode)
	{
		//注销回调
		emStatus = GXUnregisterCaptureCallback(m_hDevice);
		GX_VERIFY(emStatus);
	}

	// 释放为采集图像开辟的图像Buffer
	UnPrepareForShowImg();

	m_bDevSnaped = FALSE;
	m_snapMode = ClosedMode;
	return TRUE;
}


/*
* 参数：
* 返回：
* 功能：软件触发图像采集
*/
BOOL MerCamera::SoftwareTrigger(void)
{
	GX_STATUS emStatus = GX_STATUS_ERROR;
	int timeout = 1000;
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}


	//每次发送触发命令之前清空采集输出队列
	//防止库内部缓存帧，造成本次GXGetImage得到的图像是上次发送触发得到的图
	emStatus = GXFlushQueue(m_hDevice);
	GX_VERIFY(emStatus);

	//发送软触发命令
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_TRIGGER_SOFTWARE);
	GX_VERIFY(emStatus);


	//获取图像
	emStatus = GXGetImage(m_hDevice, &m_stFrameData, timeout);

	//判断GXGetImage返回值
	GX_VERIFY(emStatus);

	BitMap2Mat((BYTE *)m_stFrameData.pImgBuf);
	m_bIsImgSnaped = true;
	emit ImageSnaped();

	return TRUE;
}


/*
* 参数：mode:白平衡工作模式
* 返回：
* 功能：设置自动白平衡
*/
BOOL MerCamera::SetAutoWhiteBalance(GX_BALANCE_WHITE_AUTO_ENTRY mode)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	// 0-off, 1-continuous, 2-once
	int64_t nAwbMode = static_cast<int64_t>(mode);

	// 0-adaptive, 1-D65, 2-Fluoresence, Incandescent
	int64_t nAwbLightMode = 0;

	// 设置白平衡
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_WHITE_AUTO, nAwbMode);
	GX_VERIFY(emStatus);

	if (nAwbMode > 0)
	{
		// 设置白平衡光源
		emStatus = GXSetEnum(m_hDevice, GX_ENUM_AWB_LAMP_HOUSE, nAwbLightMode);
		GX_VERIFY(emStatus);
	}

	return TRUE;
}


/*
* 参数：channel:白平衡通道，
*			val：通道白平衡值
* 返回：
* 功能：
*/
BOOL MerCamera::SetBalanceRatio(GX_BALANCE_RATIO_SELECTOR_ENTRY channel, double val)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 关闭自动白平衡
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_WHITE_AUTO, 0);
	GX_VERIFY(emStatus);


	// 选择白平衡通道
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR,static_cast<int64_t>(channel));
	GX_VERIFY(emStatus);

	emStatus = GXSetFloat(m_hDevice, GX_FLOAT_BALANCE_RATIO, val);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：channel:白平衡通道
*			val：输出，通道白平衡值
* 返回：
* 功能：
*/
BOOL MerCamera::GetBalanceRatio(GX_BALANCE_RATIO_SELECTOR_ENTRY channel, double *val)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 选择白平衡通道
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, static_cast<int64_t>(channel));
	GX_VERIFY(emStatus);

	emStatus = GXGetFloat(m_hDevice, GX_FLOAT_BALANCE_RATIO, val);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：selector：选择增益通道
* 返回：
* 功能：设置相机增益通道
*/
BOOL MerCamera::SetBlackLevelSelector(GX_BLACKLEVEL_SELECTOR_ENTRY selector)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BLACKLEVEL_SELECTOR, static_cast<int64_t>(selector));
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：mode：自动增益工作模式
* 返回：
* 功能：设置相机增益工作模式
*/
BOOL MerCamera::SetAutoBlackLevel(GX_BLACKLEVEL_AUTO_ENTRY mode)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXSetEnum(m_hDevice, GX_ENUM_BLACKLEVEL_AUTO, static_cast<int64_t>(mode));
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：selector：选择增益通道
* 返回：
* 功能：设置相机增益通道
*/
BOOL MerCamera::SetGainSelector(GX_GAIN_SELECTOR_ENTRY selector)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXSetEnum(m_hDevice, GX_ENUM_GAIN_SELECTOR, static_cast<int64_t>(selector));
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：mode：自动增益工作模式
* 返回：
* 功能：设置相机增益工作模式
*/
BOOL MerCamera::SetAutoGain(GX_GAIN_AUTO_ENTRY mode)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXSetEnum(m_hDevice, GX_ENUM_GAIN_AUTO, static_cast<int64_t>(mode));
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：val：增益值
* 返回：
* 功能：设置相机增益值
*/
BOOL MerCamera::SetGain(double gain)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, gain);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：gain:输出，增益值
* 返回：
* 功能：获取相机的增益值
*/
BOOL MerCamera::GetGain(double *gain)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXGetFloat(m_hDevice, GX_FLOAT_GAIN, gain);

	return TRUE;
}


/*
* 参数：mode：曝光工作模式
* 返回：
* 功能：设置曝光工作模式
*/
BOOL MerCamera::SetAutoExposure(GX_EXPOSURE_AUTO_ENTRY mode)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	// 0--off，1--continuous, 2-once
	int64_t  nExposureMode = static_cast<int64_t>(mode);

	emStatus = GXSetEnum(m_hDevice, GX_ENUM_EXPOSURE_AUTO, nExposureMode);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：t:曝光时间
* 返回：
* 功能：设置曝光时间
*/
BOOL MerCamera::SetExposure(double t)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS status = GX_STATUS_SUCCESS;

	status = GXSetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, t);
	GX_VERIFY(status);

	return TRUE;
}


/*
* 参数：t:输出，曝光时间
* 返回：
* 功能：获取相机的曝光时间
*/
BOOL MerCamera::GetExposure(double *t)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	emStatus = GXGetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, t);

	return TRUE;
}


/*
* 参数：gray:期望平均灰度值
* 返回：
* 功能：设置自动曝光时，图像期望平均灰度值
*/
BOOL MerCamera::SetExpectedGray(int64_t gray)
{
	m_messageList.clear();

	//判断句柄是否有效，避免设备掉线后关闭程序出现的异常
	if (m_hDevice == NULL)
	{
		m_messageList.append(QStringLiteral("设备句柄无效！"));
		return FALSE;
	}

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 光照类型0：自然光，1：50Hz日光灯，2：60Hz日光灯
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_AA_LIGHT_ENVIRONMENT, 0);
	GX_VERIFY(emStatus);

	// 期望灰度值
	emStatus = GXSetInt(m_hDevice, GX_INT_GRAY_VALUE, gray);
	GX_VERIFY(emStatus);

	return TRUE;
}


/*
* 参数：
* 返回：
* 功能：返回相机状态
*/
BOOL MerCamera::IsCameraOpened()
{
	// 开采标记置位，才表示相机已正常工作
	return m_bDevSnaped;
}


/*
* 参数：pBuffer：图像数据内存空间
* 返回：
* 功能：将图像数据，连接到Mat的data部分
*/
void MerCamera::BitMap2Mat(BYTE* pBuffer)
{
	m_image.data = pBuffer;
}


/*
* 参数：
* 返回：Mat格式的图像
* 功能：
*/
Mat& MerCamera::GetMat()
{
	return m_image;
}


/*
* 参数：
* 返回：
* 功能：复位图像数据采集标志
*/
void MerCamera::ResetImgSnapedFlag()
{
	m_bIsImgSnaped = false;
}


/*
* 参数：path:avi文件路径
* 返回：
* 功能：创建avi视频
*/
BOOL MerCamera::CreateAviVideo(const QString &path)
{
	string stdPath = path.toStdString();

	// 先关闭已打开的视频
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
* 参数：
* 返回：
* 功能：释放打开的avi文件
*/
void MerCamera::ReleaseAviVideo()
{
	if (m_videoWriter.isOpened())
	{
		m_videoWriter.release();
	}
}


/*
* 参数：frame:一帧图像
* 返回：
* 功能：向avi文件中写入一帧视频
*/
void MerCamera::WriteFrame2Video(const Mat &frame)
{
	if (m_videoWriter.isOpened())
	{
		m_videoWriter.write(frame);
	}
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

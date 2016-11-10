/*
* 创建日期：2016-09-19
* 最后修改：2016-11-10
* 作      者：syf
* 描      述：
*/
#include "serialport.h"


/*
* 参数：
* 返回：
* 功能：构造函数
*/
SerialPort::SerialPort(QObject *parent)
	: QSerialPort(parent)
	, m_pTimer(NULL)
	, m_handshakeState(Idle)
	, m_currentPressure(0)
{
	m_handshakeState = Idle;
	m_pTimer = new QTimer;

	connect(this, &QSerialPort::readyRead, this, &SerialPort::OnReadyRead);
	//connect(this, &SerialPort::StartTimer, this, &SerialPort::OnStartTimer);
	connect(m_pTimer, &QTimer::timeout, this, &SerialPort::OnTimer);
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
SerialPort::~SerialPort()
{
	if (m_pTimer)
	{
		m_pTimer->stop();
		delete m_pTimer;
	}
}


/*
* 参数：
* 返回：
* 功能：返回系统可用com口列表
*/
QStringList SerialPort::GetComList()
{
	QStringList comList;

	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		comList.append(info.portName());
	}

	return comList;
}


/*
* 参数：comName:串口名称
* 返回：
* 功能：根据串口名称，打开相应的窗口
*/
bool SerialPort::Open(const QString &comName)
{
	bool state = false;

	this->setPortName(comName);
	m_messageList.clear();
	state = this->open(QIODevice::ReadWrite);

	if (state)
	{
		this->setBaudRate(19200);
		this->setDataBits(QSerialPort::Data8);
		this->setStopBits(QSerialPort::OneStop);
		this->setParity(QSerialPort::NoParity);
		this->setFlowControl(QSerialPort::NoFlowControl);
		m_messageList.append("open serial port successed!");

		m_pTimer->start(200);
		//emit StartTimer();
	}
	else
	{
		m_messageList.append("open serial port failed!");
	}
	
	return state;
}


/*
* 参数：
* 返回：
* 功能：关闭串口
*/
void SerialPort::Close()
{
	if ((NULL != m_pTimer) && (m_pTimer->isActive()))
	{
		m_pTimer->stop();
	}

	this->close();
	m_handshakeState = Idle;
}

/*
* 参数：
* 返回：
* 功能：接收数据解码
*/
void SerialPort::RxDataDecode()
{
	int index = 0;
	int len = m_rxBuf.size();


	// 从接收缓存数据中，寻找帧头
	for (; index < len; index++)
	{
		if (static_cast<char>(0x68) == m_rxBuf[index])
		{
			break;
		}
	}

	if (index < len)
	{
		// 帧结构 start(1) + dir(1) + function code(1) + (data len)(1) + checkSum(1) + end(1) = 6
		int dataLen = m_rxBuf[3];
		int frameLen = dataLen + 6;
		quint8 checkSum = 0;

		// 帧结尾判断
		if ((static_cast<char>(0x16) == m_rxBuf[index + frameLen - 1]) && ((index + frameLen) <= len))
		{
			// 计算校验和
			for (int n = index + 1; n < frameLen - 2; n++)
			{
				checkSum += static_cast<quint8>(m_rxBuf[n]);
			}

			if (checkSum == static_cast<quint8>(m_rxBuf[index + frameLen - 2]))
			{
				ProtocolDecode(m_rxBuf, index);
			}

			// 清除处理完的数据
			//m_rxBuf.clear();
			m_rxBuf.remove(0, index + len);
		}
	}
	
	// 防止发生错误时，数据堆积导致解析错误
	if (len >= 128)
	{
		m_rxBuf.clear();
	}
}


/*
* 参数：data：需要解析的数据
			index：帧头索引
* 返回：
* 功能：数据协议解析
*/
void SerialPort::ProtocolDecode(const QByteArray &data, int index)
{
	// 帧结构 start(1) + dir(1) + function code(1) + (data len)(1) + checkSum(1) + end(1) = 6
	int funcCode = static_cast<int>(data[index + 2]);
	

	switch (funcCode)
	{
	case 0:	// 参数设置读取	
		m_handshake.cmd = 0x0;
		if (WaitForSetParamAck == m_handshakeState)
		{
			if (data[index + 4])
			{
				// 应答，参数设置不成功
				m_handshake.state = SetParamError;
			}
			else
			{
				// 应答，参数设置成功
				m_handshake.state = SetParamOk;
			}
			m_handshakeState = Idle;
			emit HandshakeState(&m_handshake);
		}
		else if (WaitForReadParamAck == m_handshakeState)
		{

		}
		
		m_handshakeState = Idle;
		break;
	case 1:	// 实时数据及状态读取
		if (WaitForReadStateAck == m_handshakeState)
		{
			// 当前压力值
			m_currentPressure = static_cast<quint8>(data[index + 7]);
			m_currentPressure <<= 8;
			m_currentPressure += static_cast<quint8>(data[index + 6]);
			m_currentPressure <<= 8;
			m_currentPressure += static_cast<quint8>(data[index + 5]);
			m_currentPressure <<= 8;
			m_currentPressure += static_cast<quint8>(data[index + 4]);
			m_deviceState.pressure = static_cast<double>(m_currentPressure);

#ifdef DATA_DEBUG
			emit CurrentPressure(m_currentPressure);
#endif

			// 流进工作台谁的体积
			m_deviceState.waterInVolum = static_cast<int>(data[index + 11]);
			m_deviceState.waterInVolum <<= 8;
			m_deviceState.waterInVolum += static_cast<int>(data[index + 10]);
			m_deviceState.waterInVolum <<= 8;
			m_deviceState.waterInVolum += static_cast<int>(data[index + 9]);
			m_deviceState.waterInVolum <<= 8;
			m_deviceState.waterInVolum += static_cast<int>(data[index + 8]);
			
			// 水箱位置
			m_deviceState.waterLevelState = static_cast<quint8>(data[index + 12]);

			// 压头状态
			m_deviceState.pressureHeadState = static_cast<quint8>(data[index + 13]);

			// 照明灯状态
			m_deviceState.lightingState = static_cast<quint8>(data[index + 14]);

			// 保护罩状态
			m_deviceState.shieldState = static_cast<quint8>(data[index + 15]);

			// 工作状态
			m_deviceState.workingState = static_cast<quint8>(data[index + 16]);

			// 当前测试项目
			m_deviceState.methoPlan = static_cast<quint8>(data[index + 17]);

			// 温度
			m_deviceState.temperature = static_cast<quint16>(data[index + 19]);
			m_deviceState.temperature <<= 8;
			m_deviceState.temperature += static_cast<quint8>(data[index + 18]);

			// 剩余时间
			m_deviceState.restTime = static_cast<quint8>(data[index + 20]);

			// 剩余周期
			m_deviceState.restCycle = static_cast<quint8>(data[index + 21]); 
			
			//emit DeviceStateReceived(m_deviceState);

			m_handshake.state = ReadStateOk;
			m_handshake.cmd = 0x0;
			m_handshakeState = Idle;
			emit HandshakeState(&m_handshake);
		}
		
		m_handshakeState = Idle;
		break;
	case 2:	// 对设备的操作命令	
		if (WaitForCmdAck == m_handshakeState)
		{
			if (data[index + 4])
			{
				// 命令不成功
				m_handshake.state = CmdError;
			}
			else
			{
				// 命令成功
				m_handshake.state = CmdOk;
			}

			emit HandshakeState(&m_handshake);
		}
		m_handshakeState = Idle;
		
		break;
	case 3:	// 设备主动上报
		m_handshake.state = DeviceReport;
		m_handshake.cmd = data[index + 4];
		emit HandshakeState(&m_handshake);
		//m_handshakeState = Idle;
		break;
	default:
		break;
	}
}


/*
* 参数：method--包含方法参数的结构体
* 返回：
* 功能：向设备设置参数
*/
void SerialPort::TxSetParam(STRUCT_MethodParam &method)
{
	// 串口非空闲状态，不发送新数据
	if (Idle != m_handshakeState)
	{
		m_handshake.state = Busy;
		m_handshake.cmd = 0x0;
		emit HandshakeState(&m_handshake);
		return;
	}

	QByteArray txData;
	quint8 checkSum = 0;
	char ch = 0;
	int value = 0;

	// 帧头
	txData.append(static_cast<char>(0x68));

	// 方向
	txData.append(static_cast<char>(0x0));

	// 功能码
	txData.append(static_cast<char>(0x0));

	switch (method.plan)
	{
	case 0:	// 设置持续增压参数
		// dataLen = 持续增压（长度 = 1 + 4）
		txData.append(static_cast<char>(0x05));
		checkSum += static_cast<quint8>(0x05);

		// 持续增压的速率（数据项 = 0x01； 数据 = method.rate）
		txData.append(static_cast<char>(0x01));
		checkSum += static_cast<quint8>(0x01);

		value = static_cast<int>(method.rate);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value>> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);
		break;
	case 1:	// 设置定时计压参数
		// dataLen = 定时计压（长度 = 2 + 8）
		txData.append(static_cast<char>(0x0a));
		checkSum += static_cast<quint8>(0x0a);

		// 定时计压的速率（数据项 = 0x02； 参数 = method.rate）
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		value = static_cast<int>(method.rate);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 定时计压的定时时间（数据项 = 0x03； 参数 = method.timing）
		txData.append(static_cast<char>(0x03));
		checkSum += static_cast<quint8>(0x03);

		value = static_cast<int>(method.timing);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);
		break;
	case 2:	// 设置定时定压参数
		// dataLen = 定时定压（长度 = 3 + 12）
		txData.append(static_cast<char>(0x0f));
		checkSum += static_cast<quint8>(0x0f);

		// 定时定压的速率（数据项 = 0x04； 参数 = method.rate）
		txData.append(static_cast<char>(0x04));
		checkSum += static_cast<quint8>(0x04);

		value = static_cast<int>(method.rate);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 定时定压的目标压强（数据项 = 0x05； 参数 = method.pressure）
		txData.append(static_cast<char>(0x05));
		checkSum += static_cast<quint8>(0x05);

		value = static_cast<int>(method.pressure);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 定时定压的定时时间（数据项 = 0x06； 参数 = method.timing）
		txData.append(static_cast<char>(0x06));
		checkSum += static_cast<quint8>(0x06);

		value = static_cast<int>(method.timing);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);
		break;
	case 3: // 设置绕曲松弛参数
		// dataLen = 绕曲松弛（长度 = 4 + 16）
		txData.append(static_cast<char>(0x14));
		checkSum += static_cast<quint8>(0x014);

		// 绕曲松弛的速率（数据项 = 0x07；参数 = method.rate ）
		txData.append(static_cast<char>(0x07));
		checkSum += static_cast<quint8>(0x07);

		value = static_cast<int>(method.rate);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 绕曲松弛的目标压强（数据项 = 0x08； 参数 = method.pressure）
		txData.append(static_cast<char>(0x08));
		checkSum += static_cast<quint8>(0x08);

		value = static_cast<int>(method.pressure);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 绕曲松弛的松绕周期（数据项 = 0x09； 参数 = method.cycel）
		txData.append(static_cast<char>(0x09));
		checkSum += static_cast<quint8>(0x09);

		value = method.cycle;
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 绕曲松弛的水压保持时间（数据项 = 0x0a； 参数 = method.holdingTime）
		txData.append(static_cast<char>(0x0a));
		checkSum += static_cast<quint8>(0x0a);

		value = static_cast<int>(method.holdingTime);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);
		break;
	case 4:	// 设置渗水漏水参数
		// dataLen = 渗水漏水（长度 = 3 + 12）
		txData.append(static_cast<char>(0x0f));
		checkSum += static_cast<quint8>(0x0f);

		// 渗水漏水的速率（数据项 = 0x0b； 参数 = method.rate）
		txData.append(static_cast<char>(0x0b));
		checkSum += static_cast<quint8>(0x0b);

		value = static_cast<int>(method.rate);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 渗水漏水的目标压强（数据项 = 0x0c； 参数 = method.pressure）
		txData.append(static_cast<char>(0x05));
		checkSum += static_cast<quint8>(0x05);

		value = static_cast<int>(method.pressure);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		// 渗水漏水的定时时间（数据项 = 0x0d； 参数 = method.timing）
		txData.append(static_cast<char>(0x0d));
		checkSum += static_cast<quint8>(0x0d);

		value = static_cast<int>(method.timing);
		ch = static_cast<char>(value);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 8);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 16);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);

		ch = static_cast<char>(value >> 24);
		txData.append(ch);
		checkSum += static_cast<quint8>(ch);
		break;
	case 5:	// 设置压力量程参数
		// dataLen = 量程设置（长度 = 1 + 1）
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 量程设置（数据项 = 0x0E； 数据 = method.range）
		txData.append(static_cast<char>(0x0e));
		checkSum += static_cast<quint8>(0x0e);
		
		switch (method.range)
		{
		case 0:
			txData.append(static_cast<char>(0x01));
			checkSum += static_cast<quint8>(0x01);
			break;
		case 1:
			txData.append(static_cast<char>(0x02));
			checkSum += static_cast<quint8>(0x02);
			break;
		case 2:
			txData.append(static_cast<char>(0x03));
			checkSum += static_cast<quint8>(0x03);
			break;
		case 3:
			txData.append(static_cast<char>(0x04));
			checkSum += static_cast<quint8>(0x04);
			break;
		default:
			txData.append(static_cast<char>(0x03));
			checkSum += static_cast<quint8>(0x03);
			break;
		}

		break;
	default:
		break;
	}

	// 校验
	txData.append(static_cast<char>(checkSum));

	// 帧尾
	txData.append(static_cast<char>(0x16));

	// 通过串口发送数据
	this->write(txData);

	// 方案参数设置，等待返回
	m_handshakeState = WaitForSetParamAck;
	m_ackTime = QTime::currentTime().addSecs(2);
}


/*
* 参数：
* 返回：
* 功能：想设备发送实时数据及状态读取命令
*/
void SerialPort::TxReadState()
{
	// 串口非空闲状态，不发送新数据
	if (Idle != m_handshakeState)
	{
		//m_handshake.state = Busy;
		//m_handshake.cmd = 0x0;
		//emit HandshakeState(m_handshake);

		return;
	}

	QByteArray txData;
	quint8 checkSum = 0;

	// 帧头
	txData.append(static_cast<char>(0x68));

	// 方向
	txData.append(static_cast<char>(0x01));
	checkSum += static_cast<quint8>(0x01);

	// 功能码
	txData.append(static_cast<char>(0x01));
	checkSum += static_cast<quint8>(0x01);

	// 长度
	txData.append(static_cast<char>(0x0));

	// 校验
	txData.append(static_cast<char>(checkSum));

	// 帧尾
	txData.append(static_cast<char>(0x16));

	// 通过串口发送数据
	this->write(txData);

	// 方案参数设置，等待返回
	m_handshakeState = WaitForReadStateAck;
	m_ackTime = QTime::currentTime().addSecs(2);
}


/*
* 参数：cmd:命令类型
			val1:命令参数1
			val2:命令参数2
* 返回：
* 功能：向下位机发送操作命令
*/
void SerialPort::TxCmd(quint8 cmd, quint8 val1, int val2)
{
	// 串口非空闲状态，不发送新数据
	//if (Idle != m_handshakeState)
	//{
	//	m_handshake.state = Busy;
	//	m_handshake.cmd = 0x0;

	//	emit HandshakeState(m_handshake);
	//	
	//	return;
	//}

	QByteArray txData;
	quint8 checkSum = 0;
	m_handshake.cmd = cmd;
	m_handshake.data = val1;

	// 帧头
	txData.append(static_cast<char>(0x68));

	// 方向
	txData.append(static_cast<char>(0x0));

	// 功能码
	txData.append(static_cast<char>(0x02));
	checkSum += static_cast<quint8>(0x02);

	// 根据命令项，设置长度及命令
	switch (cmd)
	{
	case 0x01: // 测试开始与结束命令
		// 长度 = 1 + 1
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 命令项 = 0x01
		txData.append(static_cast<char>(0x01));
		checkSum += static_cast<quint8>(0x01);

		// 命令值 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;

		break;
	case 0x02:	// 压头控制命令
		// 长度（1 + 1）
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 命令项 = 0x02
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 命令值 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x03:	// 灯光控制命令
		// 长度（1 + 1）
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 命令项 = 0x03
		txData.append(static_cast<char>(0x03));
		checkSum += static_cast<quint8>(0x03);

		// 命令值 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x05:	// 去皮
		// 长度（1）
		txData.append(static_cast<char>(0x01));
		checkSum += static_cast<quint8>(0x01);

		// 命令项 = 0x05
		txData.append(static_cast<char>(0x05));
		checkSum += static_cast<quint8>(0x05);
		break;
	case 0x06:	// 恢复出厂设置
		// 长度（1）
		txData.append(static_cast<char>(0x01));
		checkSum += static_cast<quint8>(0x01);

		// 命令项 = 0x06
		txData.append(static_cast<char>(0x06));
		checkSum += static_cast<quint8>(0x06);
		break;
	case 0x07:	// 进水控制
		// 长度（1 + 1）
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 命令项 = 0x07
		txData.append(static_cast<char>(0x07));
		checkSum += static_cast<quint8>(0x07);

		// 命令值 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x08:	// 排水
		// 长度（1 + 1）
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 命令项 = 0x08
		txData.append(static_cast<char>(0x08));
		checkSum += static_cast<quint8>(0x08);

		// 命令值 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x09:
		// 长度（1 + 5）
		txData.append(static_cast<char>(0x06));
		checkSum += static_cast<quint8>(0x06);

		// 命令项 = 0x09
		txData.append(static_cast<char>(0x09));
		checkSum += static_cast<quint8>(0x09);

		// 命令值1 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;

		// 命令值2 = val2
		txData.append(static_cast<char>(val2));
		checkSum += static_cast<quint8>(val2);

		txData.append(static_cast<char>(val2 >> 8));
		checkSum += static_cast<quint8>(val2 >> 8);

		txData.append(static_cast<char>(val2 >> 16));
		checkSum += static_cast<quint8>(val2 >> 16);

		txData.append(static_cast<char>(val2 >> 24));
		checkSum += static_cast<quint8>(val2 >> 24);
		break;
	case 0x0b:	// 通知设备当前选择测试项
		// 长度（1 + 1）
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// 命令项 = 0x0b
		txData.append(static_cast<char>(0x0b));
		checkSum += static_cast<quint8>(0x0b);

		// 命令值 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	default:
		break;
	}

	// 校验
	txData.append(static_cast<char>(checkSum));

	// 帧尾
	txData.append(static_cast<char>(0x16));

	// 通过串口发送数据
	this->write(txData);

	// 方案参数设置，等待返回
	m_handshakeState = WaitForCmdAck;
	m_ackTime = QTime::currentTime().addSecs(2);
}


/*
* 参数：dir:数据方向，code：功能码
* 返回：
* 功能：向下位机发送应答
*/
void SerialPort::TxAck(quint8 dir, quint8 code)
{
	QByteArray txData;
	txData.clear();
	quint8 checkSum = 0;

	// 帧头
	txData.append(static_cast<char>(0x68));

	// 方向
	txData.append(static_cast<char>(dir));
	checkSum += dir;

	// 功能码
	txData.append(static_cast<char>(code));
	checkSum += code;

	// 长度
	txData.append(static_cast<char>(0x0));
	
	// 校验
	txData.append(static_cast<char>(checkSum));

	// 帧尾
	txData.append(static_cast<char>(0x16));

	// 通过串口发送数据
	this->write(txData);
}


/*
* 参数：
* 返回：
* 功能：读取串口接收数据
*/
void SerialPort::ResetHandshakeState()
{
	m_handshakeState = Idle;

	ResetCurrentPressure();
}


/*
* 参数：
* 返回：
* 功能：获取实时压力值
*/
int SerialPort::GetCurrentPressure()
{
	return m_currentPressure;
}

/*
* 参数：
* 返回：
* 功能：复位当前压力值
*/
void SerialPort::ResetCurrentPressure()
{
	m_currentPressure = 0;
}

/*
* 参数：
* 返回：
* 功能：返回通信状态，空闲返回1
*/
bool SerialPort::IsIdle()
{
	if (m_handshakeState == Idle)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*
* 参数：
* 返回：
* 功能：读取串口接收数据
*/
void SerialPort::OnReadyRead()
{
	//m_rxBuf.clear();
	//qDebug() << "serailport thread:" << QThread::currentThreadId() << endl;

	m_rxBuf.append(this->readAll());


	// 确保读取到一整帧数据，5ms内无新数据到来认为一帧数据传输完成
	//while (this->waitForReadyRead(20))
	//{
	//	m_rxBuf.append(this->readAll());
	//}

	if (0x16 == m_rxBuf[m_rxBuf.size() - 1])
	{
#ifdef DATA_DEBUG
		emit DataReceived(m_rxBuf);
#endif
		RxDataDecode();
	}
	
	
}

/*
* 参数：
* 返回：
* 功能：启动定时器槽函数
*/
void SerialPort::OnStartTimer()
{
	m_pTimer->start(500);
}


/*
* 参数：
* 返回：
* 功能：定时器处理槽函数
*/
void SerialPort::OnTimer()
{
	switch (m_handshakeState)
	{
	case WaitForSetParamAck:
		if (QTime::currentTime() > m_ackTime)
		{
			m_handshake.state = SetParamAckTimeOut;
			m_handshake.cmd = 0x0;
			m_handshakeState = Idle;
			emit HandshakeState(&m_handshake);		
		}
		break;
	case WaitForReadParamAck:
		if (QTime::currentTime() > m_ackTime)
		{
			m_handshake.state = ReadParamAckTimeOut;
			m_handshake.cmd = 0x0;
			m_handshakeState = Idle;
			emit HandshakeState(&m_handshake);	
		}
		break;
	case WaitForReadStateAck:
		if (QTime::currentTime() > m_ackTime)
		{
			m_handshake.state = ReadStateAckTimeOut;
			m_handshake.cmd = 0x0;
			m_handshakeState = Idle;
			emit HandshakeState(&m_handshake);		
		}
		break;
	case WaitForCmdAck:
		if (QTime::currentTime() > m_ackTime)
		{
			m_handshake.state = CmdAckTimeOut;
			m_handshakeState = Idle;
			emit HandshakeState(&m_handshake);	
		}
		break;
	default:
		break;
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
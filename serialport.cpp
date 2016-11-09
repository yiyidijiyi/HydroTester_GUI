/*
* �������ڣ�2016-09-19
* ����޸ģ�2016-11-09
* ��      �ߣ�syf
* ��      ����
*/
#include "serialport.h"


/*
* ������
* ���أ�
* ���ܣ����캯��
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
* ������
* ���أ�
* ���ܣ���������
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
* ������
* ���أ�
* ���ܣ�����ϵͳ����com���б�
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
* ������comName:��������
* ���أ�
* ���ܣ����ݴ������ƣ�����Ӧ�Ĵ���
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
* ������
* ���أ�
* ���ܣ��رմ���
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
* ������
* ���أ�
* ���ܣ��������ݽ���
*/
void SerialPort::RxDataDecode()
{
	int index = 0;
	int len = m_rxBuf.size();


	// �ӽ��ջ��������У�Ѱ��֡ͷ
	for (; index < len; index++)
	{
		if (static_cast<char>(0x68) == m_rxBuf[index])
		{
			break;
		}
	}

	if (index < len)
	{
		// ֡�ṹ start(1) + dir(1) + function code(1) + (data len)(1) + checkSum(1) + end(1) = 6
		int dataLen = m_rxBuf[3];
		int frameLen = dataLen + 6;
		quint8 checkSum = 0;

		// ֡��β�ж�
		if ((static_cast<char>(0x16) == m_rxBuf[index + frameLen - 1]) && ((index + frameLen) <= len))
		{
			// ����У���
			for (int n = index + 1; n < frameLen - 2; n++)
			{
				checkSum += static_cast<quint8>(m_rxBuf[n]);
			}

			if (checkSum == static_cast<quint8>(m_rxBuf[index + frameLen - 2]))
			{
				ProtocolDecode(m_rxBuf, index);
			}

			// ��������������
			//m_rxBuf.clear();
			m_rxBuf.remove(0, index + len);
		}
	}
	
	// ��ֹ��������ʱ�����ݶѻ����½�������
	if (len >= 128)
	{
		m_rxBuf.clear();
	}
}


/*
* ������data����Ҫ����������
			index��֡ͷ����
* ���أ�
* ���ܣ�����Э�����
*/
void SerialPort::ProtocolDecode(const QByteArray &data, int index)
{
	// ֡�ṹ start(1) + dir(1) + function code(1) + (data len)(1) + checkSum(1) + end(1) = 6
	int funcCode = static_cast<int>(data[index + 2]);
	

	switch (funcCode)
	{
	case 0:	// �������ö�ȡ	
		m_handshake.cmd = 0x0;
		if (WaitForSetParamAck == m_handshakeState)
		{
			if (data[index + 4])
			{
				// Ӧ�𣬲������ò��ɹ�
				m_handshake.state = SetParamError;
			}
			else
			{
				// Ӧ�𣬲������óɹ�
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
	case 1:	// ʵʱ���ݼ�״̬��ȡ
		if (WaitForReadStateAck == m_handshakeState)
		{
			// ��ǰѹ��ֵ
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

			// ��������̨˭�����
			m_deviceState.waterInVolum = static_cast<int>(data[index + 11]);
			m_deviceState.waterInVolum <<= 8;
			m_deviceState.waterInVolum += static_cast<int>(data[index + 10]);
			m_deviceState.waterInVolum <<= 8;
			m_deviceState.waterInVolum += static_cast<int>(data[index + 9]);
			m_deviceState.waterInVolum <<= 8;
			m_deviceState.waterInVolum += static_cast<int>(data[index + 8]);
			
			// ˮ��λ��
			m_deviceState.waterLevelState = static_cast<quint8>(data[index + 12]);

			// ѹͷ״̬
			m_deviceState.pressureHeadState = static_cast<quint8>(data[index + 13]);

			// ������״̬
			m_deviceState.lightingState = static_cast<quint8>(data[index + 14]);

			// ������״̬
			m_deviceState.shieldState = static_cast<quint8>(data[index + 15]);

			// ����״̬
			m_deviceState.workingState = static_cast<quint8>(data[index + 16]);

			// ��ǰ������Ŀ
			m_deviceState.methoPlan = static_cast<quint8>(data[index + 17]);

			// �¶�
			m_deviceState.temperature = static_cast<quint16>(data[index + 19]);
			m_deviceState.temperature <<= 8;
			m_deviceState.temperature += static_cast<quint8>(data[index + 18]);

			// ʣ��ʱ��
			m_deviceState.restTime = static_cast<quint8>(data[index + 20]);

			// ʣ������
			m_deviceState.restCycle = static_cast<quint8>(data[index + 21]); 
			
			//emit DeviceStateReceived(m_deviceState);

			m_handshake.state = ReadStateOk;
			m_handshake.cmd = 0x0;
			m_handshakeState = Idle;
			emit HandshakeState(&m_handshake);
		}
		
		m_handshakeState = Idle;
		break;
	case 2:	// ���豸�Ĳ�������	
		if (WaitForCmdAck == m_handshakeState)
		{
			if (data[index + 4])
			{
				// ����ɹ�
				m_handshake.state = CmdError;
			}
			else
			{
				// ����ɹ�
				m_handshake.state = CmdOk;
			}

			emit HandshakeState(&m_handshake);
		}
		m_handshakeState = Idle;
		
		break;
	case 3:	// �豸�����ϱ�
		
		m_handshakeState = Idle;
		break;
	default:
		break;
	}
}


/*
* ������method--�������������Ľṹ��
* ���أ�
* ���ܣ����豸���ò���
*/
void SerialPort::TxSetParam(STRUCT_MethodParam &method)
{
	// ���ڷǿ���״̬��������������
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

	// ֡ͷ
	txData.append(static_cast<char>(0x68));

	// ����
	txData.append(static_cast<char>(0x0));

	// ������
	txData.append(static_cast<char>(0x0));

	switch (method.plan)
	{
	case 0:	// ���ó�����ѹ����
		// dataLen = ������ѹ������ = 1 + 4��
		txData.append(static_cast<char>(0x05));
		checkSum += static_cast<quint8>(0x05);

		// ������ѹ�����ʣ������� = 0x01�� ���� = method.rate��
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
	case 1:	// ���ö�ʱ��ѹ����
		// dataLen = ��ʱ��ѹ������ = 2 + 8��
		txData.append(static_cast<char>(0x0a));
		checkSum += static_cast<quint8>(0x0a);

		// ��ʱ��ѹ�����ʣ������� = 0x02�� ���� = method.rate��
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

		// ��ʱ��ѹ�Ķ�ʱʱ�䣨������ = 0x03�� ���� = method.timing��
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
	case 2:	// ���ö�ʱ��ѹ����
		// dataLen = ��ʱ��ѹ������ = 3 + 12��
		txData.append(static_cast<char>(0x0f));
		checkSum += static_cast<quint8>(0x0f);

		// ��ʱ��ѹ�����ʣ������� = 0x04�� ���� = method.rate��
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

		// ��ʱ��ѹ��Ŀ��ѹǿ�������� = 0x05�� ���� = method.pressure��
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

		// ��ʱ��ѹ�Ķ�ʱʱ�䣨������ = 0x06�� ���� = method.timing��
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
	case 3: // ���������ɳڲ���
		// dataLen = �����ɳڣ����� = 4 + 16��
		txData.append(static_cast<char>(0x14));
		checkSum += static_cast<quint8>(0x014);

		// �����ɳڵ����ʣ������� = 0x07������ = method.rate ��
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

		// �����ɳڵ�Ŀ��ѹǿ�������� = 0x08�� ���� = method.pressure��
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

		// �����ɳڵ��������ڣ������� = 0x09�� ���� = method.cycel��
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

		// �����ɳڵ�ˮѹ����ʱ�䣨������ = 0x0a�� ���� = method.holdingTime��
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
	case 4:	// ������ˮ©ˮ����
		// dataLen = ��ˮ©ˮ������ = 3 + 12��
		txData.append(static_cast<char>(0x0f));
		checkSum += static_cast<quint8>(0x0f);

		// ��ˮ©ˮ�����ʣ������� = 0x0b�� ���� = method.rate��
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

		// ��ˮ©ˮ��Ŀ��ѹǿ�������� = 0x0c�� ���� = method.pressure��
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

		// ��ˮ©ˮ�Ķ�ʱʱ�䣨������ = 0x0d�� ���� = method.timing��
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
	case 5:	// ����ѹ�����̲���
		// dataLen = �������ã����� = 1 + 1��
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// �������ã������� = 0x0E�� ���� = method.range��
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

	// У��
	txData.append(static_cast<char>(checkSum));

	// ֡β
	txData.append(static_cast<char>(0x16));

	// ͨ�����ڷ�������
	this->write(txData);

	// �����������ã��ȴ�����
	m_handshakeState = WaitForSetParamAck;
	m_ackTime = QTime::currentTime().addSecs(2);
}


/*
* ������
* ���أ�
* ���ܣ����豸����ʵʱ���ݼ�״̬��ȡ����
*/
void SerialPort::TxReadState()
{
	// ���ڷǿ���״̬��������������
	if (Idle != m_handshakeState)
	{
		//m_handshake.state = Busy;
		//m_handshake.cmd = 0x0;
		//emit HandshakeState(m_handshake);

		return;
	}

	QByteArray txData;
	quint8 checkSum = 0;

	// ֡ͷ
	txData.append(static_cast<char>(0x68));

	// ����
	txData.append(static_cast<char>(0x01));
	checkSum += static_cast<quint8>(0x01);

	// ������
	txData.append(static_cast<char>(0x01));
	checkSum += static_cast<quint8>(0x01);

	// ����
	txData.append(static_cast<char>(0x0));

	// У��
	txData.append(static_cast<char>(checkSum));

	// ֡β
	txData.append(static_cast<char>(0x16));

	// ͨ�����ڷ�������
	this->write(txData);

	// �����������ã��ȴ�����
	m_handshakeState = WaitForReadStateAck;
	m_ackTime = QTime::currentTime().addSecs(2);
}


/*
* ������cmd:��������
			val1:�������1
			val2:�������2
* ���أ�
* ���ܣ�����λ�����Ͳ�������
*/
void SerialPort::TxCmd(quint8 cmd, quint8 val1, int val2)
{
	// ���ڷǿ���״̬��������������
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

	// ֡ͷ
	txData.append(static_cast<char>(0x68));

	// ����
	txData.append(static_cast<char>(0x0));

	// ������
	txData.append(static_cast<char>(0x02));
	checkSum += static_cast<quint8>(0x02);

	// ������������ó��ȼ�����
	switch (cmd)
	{
	case 0x01: // ���Կ�ʼ���������
		// ���� = 1 + 1
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// ������ = 0x01
		txData.append(static_cast<char>(0x01));
		checkSum += static_cast<quint8>(0x01);

		// ����ֵ = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;

		break;
	case 0x02:	// ѹͷ��������
		// ���ȣ�1 + 1��
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// ������ = 0x02
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// ����ֵ = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x03:	// �ƹ��������
		// ���ȣ�1 + 1��
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// ������ = 0x03
		txData.append(static_cast<char>(0x03));
		checkSum += static_cast<quint8>(0x03);

		// ����ֵ = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x05:	// ȥƤ
		// ���ȣ�1��
		txData.append(static_cast<char>(0x01));
		checkSum += static_cast<quint8>(0x01);

		// ������ = 0x05
		txData.append(static_cast<char>(0x05));
		checkSum += static_cast<quint8>(0x05);
		break;
	case 0x06:	// �ָ���������
		// ���ȣ�1��
		txData.append(static_cast<char>(0x01));
		checkSum += static_cast<quint8>(0x01);

		// ������ = 0x06
		txData.append(static_cast<char>(0x06));
		checkSum += static_cast<quint8>(0x06);
		break;
	case 0x07:	// ��ˮ����
		// ���ȣ�1 + 1��
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// ������ = 0x07
		txData.append(static_cast<char>(0x07));
		checkSum += static_cast<quint8>(0x07);

		// ����ֵ = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x08:	// ��ˮ
		// ���ȣ�1 + 1��
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// ������ = 0x08
		txData.append(static_cast<char>(0x08));
		checkSum += static_cast<quint8>(0x08);

		// ����ֵ = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	case 0x09:
		// ���ȣ�1 + 5��
		txData.append(static_cast<char>(0x06));
		checkSum += static_cast<quint8>(0x06);

		// ������ = 0x09
		txData.append(static_cast<char>(0x09));
		checkSum += static_cast<quint8>(0x09);

		// ����ֵ1 = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;

		// ����ֵ2 = val2
		txData.append(static_cast<char>(val2));
		checkSum += static_cast<quint8>(val2);

		txData.append(static_cast<char>(val2 >> 8));
		checkSum += static_cast<quint8>(val2 >> 8);

		txData.append(static_cast<char>(val2 >> 16));
		checkSum += static_cast<quint8>(val2 >> 16);

		txData.append(static_cast<char>(val2 >> 24));
		checkSum += static_cast<quint8>(val2 >> 24);
		break;
	case 0x0b:	// ֪ͨ�豸��ǰѡ�������
		// ���ȣ�1 + 1��
		txData.append(static_cast<char>(0x02));
		checkSum += static_cast<quint8>(0x02);

		// ������ = 0x0b
		txData.append(static_cast<char>(0x0b));
		checkSum += static_cast<quint8>(0x0b);

		// ����ֵ = val1
		txData.append(static_cast<char>(val1));
		checkSum += val1;
		break;
	default:
		break;
	}

	// У��
	txData.append(static_cast<char>(checkSum));

	// ֡β
	txData.append(static_cast<char>(0x16));

	// ͨ�����ڷ�������
	this->write(txData);

	// �����������ã��ȴ�����
	m_handshakeState = WaitForCmdAck;
	m_ackTime = QTime::currentTime().addSecs(2);
}


/*
* ������
* ���أ�
* ���ܣ���ȡ���ڽ�������
*/
void SerialPort::ResetHandshakeState()
{
	m_handshakeState = Idle;

	ResetCurrentPressure();
}


/*
* ������
* ���أ�
* ���ܣ���ȡʵʱѹ��ֵ
*/
int SerialPort::GetCurrentPressure()
{
	return m_currentPressure;
}

/*
* ������
* ���أ�
* ���ܣ���λ��ǰѹ��ֵ
*/
void SerialPort::ResetCurrentPressure()
{
	m_currentPressure = 0;
}

/*
* ������
* ���أ�
* ���ܣ�����ͨ��״̬�����з���1
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
* ������
* ���أ�
* ���ܣ���ȡ���ڽ�������
*/
void SerialPort::OnReadyRead()
{
	//m_rxBuf.clear();
	//qDebug() << "serailport thread:" << QThread::currentThreadId() << endl;

	m_rxBuf.append(this->readAll());


	// ȷ����ȡ��һ��֡���ݣ�5ms���������ݵ�����Ϊһ֡���ݴ������
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
* ������
* ���أ�
* ���ܣ�������ʱ���ۺ���
*/
void SerialPort::OnStartTimer()
{
	m_pTimer->start(500);
}


/*
* ������
* ���أ�
* ���ܣ���ʱ������ۺ���
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
* ������
* ���أ�
* ���ܣ�
*/




/*
* ������
* ���أ�
* ���ܣ�
*/
/*
* 创建日期：2016-09-19
* 最后修改：2016-09-19
* 作      者：syf
* 描      述：
*/
#include "serialport.h"


/*
* 参数：
* 返回：
* 功能：够着函数
*/
SerialPort::SerialPort(QObject *parent)
: QSerialPort(parent)
{
	connect(this, &QSerialPort::readyRead, this, &SerialPort::OnReadyRead);
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
SerialPort::~SerialPort()
{

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
		this->setBaudRate(9600);
		this->setDataBits(QSerialPort::Data8);
		this->setStopBits(QSerialPort::OneStop);
		this->setParity(QSerialPort::NoParity);
		this->setFlowControl(QSerialPort::NoFlowControl);
		m_messageList.append("open serial port successed!");
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
* 功能：读取串口接收数据
*/
void SerialPort::OnReadyRead()
{
	m_rxBuf.clear();

	m_rxBuf.append(this->readAll());

	while (this->waitForReadyRead(5))
	{
		m_rxBuf.append(this->readAll());
	}

	emit DataReceived(m_rxBuf);
}


/*
* 参数：
* 返回：
* 功能：
*/
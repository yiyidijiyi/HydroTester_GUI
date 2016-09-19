/*
* �������ڣ�2016-09-19
* ����޸ģ�2016-09-19
* ��      �ߣ�syf
* ��      ����
*/
#include "serialport.h"


/*
* ������
* ���أ�
* ���ܣ����ź���
*/
SerialPort::SerialPort(QObject *parent)
: QSerialPort(parent)
{
	connect(this, &QSerialPort::readyRead, this, &SerialPort::OnReadyRead);
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
SerialPort::~SerialPort()
{

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
* ������
* ���أ�
* ���ܣ���ȡ���ڽ�������
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
* ������
* ���أ�
* ���ܣ�
*/
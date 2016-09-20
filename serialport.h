/*
* 创建日期：2016-09-19
* 最后修改：2016-09-20
* 作      者：syf
* 描      述：
*/
#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QTimer>
#include <QDateTime>

#include "common.h"

class SerialPort : public QSerialPort
{
	Q_OBJECT

public:

	SerialPort(QObject *parent = 0);
	~SerialPort();


	QStringList GetComList();
	bool Open(const QString &comName);
	void Close();

	void RxDataDecode();
	void ProtocolDecode(const QByteArray &data, int index);

	void TxSetParam(STRUCT_MethodParam &mthod);
	void TxReadState();
	void TxCmd(quint8 cmd, quint8 val1, int val2);
public slots:
	void OnReadyRead();
	void OnTimer();
signals:
	void DataReceived(const QByteArray &rxBuf);
	void HandshakeState(STRUCT_HandShake &handshake);
	void DeviceStateReceived(STRUCT_DeviceState &deviceState);

private:
	QStringList m_messageList;
	QByteArray m_rxBuf;
	ENUM_HandShakeState m_handshakeState;
	STRUCT_DeviceState m_deviceState;
	STRUCT_HandShake m_handshake;
	QTime m_ackTime;
	QTimer *m_pTimer;
};

#endif // SERIALPORT_H

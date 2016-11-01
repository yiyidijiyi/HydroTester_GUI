/*
* �������ڣ�2016-09-19
* ����޸ģ�2016-11-01
* ��      �ߣ�syf
* ��      ����
*/
#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QTimer>
#include <QDateTime>
#include <QThread>
#include <QDebug>
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

	void ResetHandshakeState();
	int GetCurrentPressure();
	void ResetCurrentPressure();
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
	int m_currentPressure;
};

#endif // SERIALPORT_H

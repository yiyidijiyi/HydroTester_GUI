/*
* 创建日期：2016-09-19
* 最后修改：2016-11-09
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
#include <QThread>
#include <QDebug>
#include "common.h"

// 同设备通信结果
struct STRUCT_HandShake{
	ENUM_HandShakeState state;
	quint8  cmd;
	quint8 data;
};

Q_DECLARE_METATYPE(STRUCT_HandShake*)


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
	void TxAck(quint8 dir, quint8 code);

	void ResetHandshakeState();
	int GetCurrentPressure();
	void ResetCurrentPressure();
	bool IsIdle();
	//STRUCT_HandShake GetHandshake();

public slots:
	void OnReadyRead();
	void OnStartTimer();
	void OnTimer();

signals:
	void StartTimer();
	void CurrentPressure(int pressure);
	void DataReceived(const QByteArray &rxBuf);
	void HandshakeState(STRUCT_HandShake *handshake);
	//void HandshakeState();
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

/*
* �������ڣ�2016-09-19
* ����޸ģ�2016-09-19
* ��      �ߣ�syf
* ��      ����
*/
#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>

class SerialPort : public QSerialPort
{
	Q_OBJECT

public:
	SerialPort(QObject *parent = 0);
	~SerialPort();

	QStringList GetComList();
	bool Open(const QString &comName);

public slots:
	void OnReadyRead();

signals:
	void DataReceived(const QByteArray &rxBuf);
private:
	QStringList m_messageList;
	QByteArray m_rxBuf;
};

#endif // SERIALPORT_H

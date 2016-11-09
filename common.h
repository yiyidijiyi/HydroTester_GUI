/*
* �������ڣ�2016-09-12
* ����޸ģ�2016-11-09
* ��      �ߣ�syf
* ��      ����
*/
#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QByteArray>

// �������ͼ��ķֱ���
#define IMAGE_WIDTH	1280
#define IMAGE_HEIGHT	1024

// �˻�����
enum ENUM_AccountType{
	Tester		= 1,
	Admin		= 2,
	Developer	= 3
};

// ѹǿ��λ
enum ENUM_PressureUnit{
	Pa				= 0,
	kPa			= 1,
	mBar			= 2,
	mmH2O	= 3
};

// ���Է���
enum ENUM_TestPlan{

};

// �û��˺�
struct STRUCT_Account{
	int id;
	QString userName;
	QString passward;
	int userType;
};

// ���Է�������
struct STRUCT_MethodParam{
	int id;
	QString name;
	int plan;
	QString standard;
	double rate;
	double timing;
	double pressure;
	int cycle;
	double holdingTime;
	int uint;
	QString discription;
	QString lastTime;
	int range;
};

// ���Խ������
struct STRUCT_Reprot{
	int id;
	QString methodName;
	int methodPlan;
	QString testDate;
	int endMode;
	QString userName;
	QString fileName;
};

// ͬ�豸ͨ�ŵ�����״̬
enum ENUM_HandShakeState{
	Idle = 0,
	Busy = 255,

	// ��������
	WaitForSetParamAck = 1,
	SetParamOk = 2,
	SetParamError = 3,
	SetParamAckTimeOut = 4,

	// ������ȡ
	WaitForReadParamAck = 11,
	ReadParamOk = 12,
	ReadParamError = 13,
	ReadParamAckTimeOut = 14,

	// ʵʱ���ݼ�״̬��ȡ
	WaitForReadStateAck = 21,
	ReadStateOk = 22,
	ReadStateError = 23,
	ReadStateAckTimeOut = 24,

	// ��������
	WaitForCmdAck = 31,
	CmdOk = 32,
	CmdError = 33,
	CmdAckTimeOut = 34,

	// �豸�����ϱ�
	DeviceReport = 100
};



struct STRUCT_DeviceState{
	int pressure;
	int waterInVolum;				
	quint8 waterLevelState;
	quint8 pressureHeadState;
	quint8 lightingState;
	quint8 shieldState;
	quint8 workingState;
	quint8 methoPlan;
	quint16	temperature;
	quint8 restTime;
	quint8 restCycle;
};

enum ENUM_TestState{
	Init = 0,
	Connected,
	WaterInState,
	WaterOffState,
	SetParams,
	Start,
	Pause,
	End
};

char ConvertCharHex(char ch);
void HexToString(const QByteArray &hexData, QString &str);

#endif
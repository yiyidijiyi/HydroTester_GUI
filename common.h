/*
* �������ڣ�2016-09-12
* ����޸ģ�2016-11-30
* ��      �ߣ�syf
* ��      ����
*/
#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QByteArray>

// �������ͼ��ķֱ���
#define IMAGE_WIDTH		1280
#define IMAGE_HEIGHT	1024

// �˻�����
enum ENUM_AccountType{
	Tester		= 1,
	Admin		= 2,
	Developer	= 3
};

// ѹǿ��λ
enum ENUM_PressureUnit{
	Pa		= 0,
	kPa,
	mBar,
	mmH2O
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
	ENUM_PressureUnit unit;
	QString description;
	QString lastTime;
	int range;
};

// ���Խ������
struct STRUCT_Report{
	int id;
	QString methodName;
	int methodPlan;
	QString testDate;
	int endMode;
	QString userName;
	QString fileName;
	double rate;
	double timing;
	double pressure;
	int cycle;
	double holdingTime;
	ENUM_PressureUnit unit;
	QString description;
	QString standard;
	QString decTime;
	QString decPressure;
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
	quint16 temperature;
	quint8 restTime;
	quint8 restCycle;
};

struct STRUCT_AdvanceParams{
	int id;
	int density;
	int waterInTime;
	double dropletArea;
	int sensitivity;
	int ignorePixes;
	int x;
	int y;
	int r; 
	int pressureRange;
};

enum ENUM_TestState{
	Init = 0,
	Connected,
	WaterInStart,
    WaterInStop,
	WaterOffStart,
    WaterOffStop,
	SetParams,
	Start,
	StartByDevice,
	StartBySoftware,
	Pause,
	PauseBySoftware,
	ContinueBySoftware,
	EndAuto,
	EndByDevice,
	EndBySoftware,
	EndTimeOut,
	EndPressureOverRange,
	EndBurst,
	End
};

enum ENUM_TxData{
	TxNoData = 0,
	TxConnect,
	TxWaterInStart,
	TxWaterInStop,
	TxWaterOffStart,
	TxWaterOffStop,
	TxSetParams,
	TxReadDeviceState,
	TxStartTest,
	TxStopTest,
	TxPauseTest,
	TxAck
};

//void InitStructReport(STRUCT_Report &report);
//void IntStructMethodParam(STRUCT_MethodParam &method);
char ConvertCharHex(char ch);
void HexToString(const QByteArray &hexData, QString &str);

#endif
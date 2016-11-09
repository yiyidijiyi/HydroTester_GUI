/*
* 创建日期：2016-09-12
* 最后修改：2016-11-09
* 作      者：syf
* 描      述：
*/
#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QByteArray>

// 相机采样图像的分辨率
#define IMAGE_WIDTH	1280
#define IMAGE_HEIGHT	1024

// 账户类型
enum ENUM_AccountType{
	Tester		= 1,
	Admin		= 2,
	Developer	= 3
};

// 压强单位
enum ENUM_PressureUnit{
	Pa				= 0,
	kPa			= 1,
	mBar			= 2,
	mmH2O	= 3
};

// 测试方法
enum ENUM_TestPlan{

};

// 用户账号
struct STRUCT_Account{
	int id;
	QString userName;
	QString passward;
	int userType;
};

// 测试方案参数
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

// 测试结果报告
struct STRUCT_Reprot{
	int id;
	QString methodName;
	int methodPlan;
	QString testDate;
	int endMode;
	QString userName;
	QString fileName;
};

// 同设备通信的握手状态
enum ENUM_HandShakeState{
	Idle = 0,
	Busy = 255,

	// 参数设置
	WaitForSetParamAck = 1,
	SetParamOk = 2,
	SetParamError = 3,
	SetParamAckTimeOut = 4,

	// 参数读取
	WaitForReadParamAck = 11,
	ReadParamOk = 12,
	ReadParamError = 13,
	ReadParamAckTimeOut = 14,

	// 实时数据及状态读取
	WaitForReadStateAck = 21,
	ReadStateOk = 22,
	ReadStateError = 23,
	ReadStateAckTimeOut = 24,

	// 操作命令
	WaitForCmdAck = 31,
	CmdOk = 32,
	CmdError = 33,
	CmdAckTimeOut = 34,

	// 设备主动上报
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
	TxPauseTest
};

char ConvertCharHex(char ch);
void HexToString(const QByteArray &hexData, QString &str);

#endif
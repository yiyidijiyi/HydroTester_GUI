/*
* 创建日期：2016-09-12
* 最后修改：2016-09-14
* 作      者：syf
* 描      述：
*/
#ifndef COMMON_H
#define COMMON_H

class QString;

// 账户类型
typedef enum {
	Tester		= 1,
	Admin		= 2,
	Developer	= 3
}ENUM_AccountType;

// 压强单位
typedef enum{
	Pa			= 0,
	kPa			= 1,
	mBar		= 2,
	mmH2O	= 3
}ENUM_PressureUnit;

// 测试方法
typedef enum{

}ENUM_TestPlan;

// 用户账号
typedef struct{
	int id;
	QString userName;
	QString passward;
	int userType;
}STRUCT_Account;

// 测试方案参数
typedef struct{
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
}STRUCT_MethodParam;

#endif
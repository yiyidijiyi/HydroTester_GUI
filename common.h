/*
* �������ڣ�2016-09-12
* ����޸ģ�2016-09-14
* ��      �ߣ�syf
* ��      ����
*/
#ifndef COMMON_H
#define COMMON_H

class QString;

// �˻�����
typedef enum {
	Tester		= 1,
	Admin		= 2,
	Developer	= 3
}ENUM_AccountType;

// ѹǿ��λ
typedef enum{
	Pa			= 0,
	kPa			= 1,
	mBar		= 2,
	mmH2O	= 3
}ENUM_PressureUnit;

// ���Է���
typedef enum{

}ENUM_TestPlan;

// �û��˺�
typedef struct{
	int id;
	QString userName;
	QString passward;
	int userType;
}STRUCT_Account;

// ���Է�������
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
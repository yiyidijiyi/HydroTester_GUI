/*
* �������ڣ�2016-09-12
* ����޸ģ�2016-09-12
* ��      �ߣ�syf
* ��      ����
*/
#ifndef COMMON_H
#define COMMON_H

class QString;

typedef enum {
	Tester		= 1,
	Admin		= 2,
	Developer	= 3
}ENUM_AccountType;

typedef struct{
	int id;
	QString userName;
	QString passward;
	int userType;
}STRUCT_Account;


#endif
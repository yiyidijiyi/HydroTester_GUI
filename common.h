/*
* 创建日期：2016-09-12
* 最后修改：2016-09-12
* 作      者：syf
* 描      述：
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
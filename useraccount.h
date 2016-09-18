/*
* 创建日期：2016-09-12
* 最后修改：2016-09-18
* 作      者：syf
* 描      述：
*/
#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QObject>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>

#include "common.h"


class UserAccount : public QObject
{
	Q_OBJECT

public:
	UserAccount(QObject *parent = 0);
	~UserAccount();

	QStringList& GetMessageList();
	QStringList& GetUserNameList();
	bool AuthenticateAccount(int &id, const QString &userName, const QString &passward, int userType);
	bool GetAccountList(QStringList &accountList,  ENUM_AccountType userType);
	bool GetAccountInfo(const QString &userName, STRUCT_Account &account);
	bool GetAccountInfo(int id, STRUCT_Account &account);
	bool AddAccount(STRUCT_Account &account);
	bool DeleteAccount(int id);
	bool DeleteAccount(const QString &userName);
	bool UpdateAccount(const STRUCT_Account &ccount);
private:
	QStringList m_messageList;
	QStringList m_userNameList;
};

#endif // USERACCOUNT_H

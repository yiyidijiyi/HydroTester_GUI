/*
* 创建日期：2016-09-12
* 最后修改：2016-09-12
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

#include "common.h"


class UserAccount : public QObject
{
	Q_OBJECT

public:
	UserAccount(QObject *parent = 0);
	~UserAccount();

	QStringList& GetAccontList(ENUM_AccountType userType);
private:
	QStringList m_accountList;
	QStringList m_messageList;
};

#endif // USERACCOUNT_H

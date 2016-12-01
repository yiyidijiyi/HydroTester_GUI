/*
* 创建日期：2016-09-12
* 最后修改：2016-11-30
* 作      者：syf
* 描      述：
*/
#include "useraccount.h"


/*
* 参数：
* 返回：
* 功能：构造函数
*/
UserAccount::UserAccount(QObject *parent)
	: QObject(parent)
{
	m_messageList.clear();
}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
UserAccount::~UserAccount()
{

}


/*
* 参数：
* 返回：
* 功能：数据库操作信息
*/
QStringList& UserAccount::GetMessageList()
{
	return m_messageList;
}


/*
* 参数：
* 返回：
* 功能：返回用户名列表
*/
QStringList& UserAccount::GetUserNameList()
{
	return m_userNameList;
}


/*
* 参数：userType--用户类型
* 返回：
* 功能：根据用户类型获取数据库中的账户列表
*/
bool UserAccount::GetAccountList(QStringList &accountList, ENUM_AccountType userType)
{
	m_messageList.clear();
	m_userNameList.clear();
	accountList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		QString strQuery = "SELECT userName FROM account WHERE userType = ?";
		QSqlQuery query(db);
		query.prepare(strQuery);
		query.addBindValue(static_cast<int>(userType));

		if (query.exec())
		{
			while (query.next())
			{
				QString userName = query.value(0).toString();
				accountList.append(userName);
				m_userNameList.append(userName);
			}

			state = true;
		}
		else
		{
			m_messageList.append(QStringLiteral("在数据库中查询数据，失败！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("打开数据库，失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：userName--用户账号名称
* 返回：
* 功能：根据用户账号名称查询，用户账号信息
*/
bool UserAccount::GetAccountInfo(const QString &userName, STRUCT_Account &account)
{
	bool state = false;
	m_messageList.clear();

	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM account WHERE userName = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(userName);

		if (query.exec())
		{
			int count = 0;
			while (query.next())
			{
				count++;
				account.id = query.value(0).toInt();
				account.userName = query.value(1).toString();
				account.passward = query.value(2).toString();
				account.userType = query.value(3).toInt();
			}

			if (0 == count)
			{
				m_messageList.append(QStringLiteral("未查找到账号信息！"));
			}
			else if (count > 1)
			{
				m_messageList.append(QStringLiteral("存在同名账号，请联系管理员！"));
			}
			else
			{
				state = true;
				m_messageList.append(QStringLiteral("查询账号信息成功！"));
			}
		}
		else
		{
			m_messageList.append(QStringLiteral("查询账号信息出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("连接账号数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：id--用户账号id
* 返回：
* 功能：根据用户账号名称查询，用户账号信息
*/
bool UserAccount::GetAccountInfo(int id, STRUCT_Account &account)
{
	bool state = false;
	m_messageList.clear();

	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM account WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			int count = 0;
			while (query.next())
			{
				count++;
				account.id = query.value(0).toInt();
				account.userName = query.value(1).toString();
				account.passward = query.value(2).toString();
				account.userType = query.value(3).toInt();
			}

			if (0 == count)
			{
				m_messageList.append(QStringLiteral("未查找到账号信息！"));
			}
			else if (count > 1)
			{
				m_messageList.append(QStringLiteral("存在同名账号，请联系管理员！"));
			}
			else
			{
				state = true;
				m_messageList.append(QStringLiteral("查询账号信息成功！"));
			}
		}
		else
		{
			m_messageList.append(QStringLiteral("查询账号信息出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("连接账号数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}



/*
* 参数：id--账户id
* 返回：
* 功能：校验登陆密码
*/
bool UserAccount::AuthenticateAccount(int &id, const QString &userName, const QString &passward, int userType)
{
	bool state = false;
	m_messageList.clear();
	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		QString strQuery = "SELECT id FROM account WHERE userName = ? AND userPassward = ? AND userType = ?";
		QSqlQuery query(strQuery, db);
		query.bindValue(0, userName);
		query.bindValue(1, passward);
		query.bindValue(2, userType);

		if (query.exec())
		{
			int count = 0;

			//QString errStr = query.lastError().text();

			while (query.next())
			{
				count++;
				id = query.value(0).toInt();
			}

			if (1 == count)
			{
				state = true;
			}
		}
		else
		{
			m_messageList.append(QStringLiteral("查询账号数据库出错！"));
			m_messageList.append(query.lastError().text());
		}	
	}
	else
	{
		m_messageList.append(QStringLiteral("连接账号数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();

	return state;
}

/*
* 参数：account--账户信息
* 返回：
* 功能：增加账户
*/
bool UserAccount::AddAccount(STRUCT_Account &account)
{
	m_messageList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		// 先查询账户名是否重复
		QString strQuery = "SELECT id FROM account WHERE userName = ?";
		QSqlQuery query(db);
		query.prepare(strQuery);
		query.bindValue(0, account.userName);

		if (query.exec())
		{
			int count = 0;

			while (query.next())
			{
				count++;
			}

			if (count > 0)
			{
				m_messageList.append(QStringLiteral("账号名重复！"));
				return state;
			}
		}
		else
		{
			m_messageList.append(QStringLiteral("在账号数据库中查询重复账号名时出错！"));
			m_messageList.append(query.lastError().text());
			return state;
		}

		// 插入新的账户
		strQuery = "INSERT INTO account (userName, userPassward, userType) VALUES(?, ?, ?)";
		query.prepare(strQuery);
		query.bindValue(0, account.userName);
		query.bindValue(1, account.passward);
		query.bindValue(2, account.userType);

		if (query.exec())
		{
			state = true;
		}
		else
		{
			m_messageList.append(QStringLiteral("在账号数据库中添加账号出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("链接账号数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：id--账户id
* 返回：
* 功能：根据账户id删除账户
*/
bool UserAccount::DeleteAccount(int id)
{
	m_messageList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		QString strQuery = "DELETE FROM account WHERE id = ?";
		QSqlQuery query(db);
		query.prepare(strQuery);
		query.bindValue(0, id);

		if (query.exec())
		{
			state = true;
		}
		else
		{
			m_messageList.append(QStringLiteral("在账号数据库中删除账号出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("链接账号数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：id--账户id
* 返回：
* 功能：根据账户id删除账户
*/
bool UserAccount::DeleteAccount(const QString &userName)
{
	m_messageList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		QString strQuery = "DELETE FROM account WHERE userName = ?";
		QSqlQuery query(db);
		query.prepare(strQuery);
		query.bindValue(0, userName);

		if (query.exec())
		{
			state = true;
		}
		else
		{
			m_messageList.append(QStringLiteral("在账号数据库中删除账号出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("链接账号数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：account--账户信息
* 返回：
* 功能：修改账户信息
*/
bool UserAccount::UpdateAccount(const STRUCT_Account &account)
{
	m_messageList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection");
	}
	else
	{
		db = QSqlDatabase::addDatabase("SQLITECIPHER", "qt_sql_default_connection");
	}
	db.setDatabaseName("./data/account.db");
	db.setPassword("caep17305");

	if (db.open())
	{
		// 修改账户密码
		QString strQuery = "UPDATE account SET userPassward = ? WHERE userName = ?";
		QSqlQuery query(db);
		query.prepare(strQuery);
		query.bindValue(0, account.passward);
		query.bindValue(1, account.userName);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("修改用户密码成功！"));
		}
		else
		{
			m_messageList.append(QStringLiteral("修改用户密码时出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("链接账号数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}



/*
* 参数：
* 返回：
* 功能：
*/

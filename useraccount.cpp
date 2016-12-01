/*
* �������ڣ�2016-09-12
* ����޸ģ�2016-11-30
* ��      �ߣ�syf
* ��      ����
*/
#include "useraccount.h"


/*
* ������
* ���أ�
* ���ܣ����캯��
*/
UserAccount::UserAccount(QObject *parent)
	: QObject(parent)
{
	m_messageList.clear();
}


/*
* ������
* ���أ�
* ���ܣ���������
*/
UserAccount::~UserAccount()
{

}


/*
* ������
* ���أ�
* ���ܣ����ݿ������Ϣ
*/
QStringList& UserAccount::GetMessageList()
{
	return m_messageList;
}


/*
* ������
* ���أ�
* ���ܣ������û����б�
*/
QStringList& UserAccount::GetUserNameList()
{
	return m_userNameList;
}


/*
* ������userType--�û�����
* ���أ�
* ���ܣ������û����ͻ�ȡ���ݿ��е��˻��б�
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
			m_messageList.append(QStringLiteral("�����ݿ��в�ѯ���ݣ�ʧ�ܣ�"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("�����ݿ⣬ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������userName--�û��˺�����
* ���أ�
* ���ܣ������û��˺����Ʋ�ѯ���û��˺���Ϣ
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
				m_messageList.append(QStringLiteral("δ���ҵ��˺���Ϣ��"));
			}
			else if (count > 1)
			{
				m_messageList.append(QStringLiteral("����ͬ���˺ţ�����ϵ����Ա��"));
			}
			else
			{
				state = true;
				m_messageList.append(QStringLiteral("��ѯ�˺���Ϣ�ɹ���"));
			}
		}
		else
		{
			m_messageList.append(QStringLiteral("��ѯ�˺���Ϣ����"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("�����˺����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������id--�û��˺�id
* ���أ�
* ���ܣ������û��˺����Ʋ�ѯ���û��˺���Ϣ
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
				m_messageList.append(QStringLiteral("δ���ҵ��˺���Ϣ��"));
			}
			else if (count > 1)
			{
				m_messageList.append(QStringLiteral("����ͬ���˺ţ�����ϵ����Ա��"));
			}
			else
			{
				state = true;
				m_messageList.append(QStringLiteral("��ѯ�˺���Ϣ�ɹ���"));
			}
		}
		else
		{
			m_messageList.append(QStringLiteral("��ѯ�˺���Ϣ����"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("�����˺����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}



/*
* ������id--�˻�id
* ���أ�
* ���ܣ�У���½����
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
			m_messageList.append(QStringLiteral("��ѯ�˺����ݿ����"));
			m_messageList.append(query.lastError().text());
		}	
	}
	else
	{
		m_messageList.append(QStringLiteral("�����˺����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();

	return state;
}

/*
* ������account--�˻���Ϣ
* ���أ�
* ���ܣ������˻�
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
		// �Ȳ�ѯ�˻����Ƿ��ظ�
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
				m_messageList.append(QStringLiteral("�˺����ظ���"));
				return state;
			}
		}
		else
		{
			m_messageList.append(QStringLiteral("���˺����ݿ��в�ѯ�ظ��˺���ʱ����"));
			m_messageList.append(query.lastError().text());
			return state;
		}

		// �����µ��˻�
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
			m_messageList.append(QStringLiteral("���˺����ݿ�������˺ų���"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("�����˺����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������id--�˻�id
* ���أ�
* ���ܣ������˻�idɾ���˻�
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
			m_messageList.append(QStringLiteral("���˺����ݿ���ɾ���˺ų���"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("�����˺����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������id--�˻�id
* ���أ�
* ���ܣ������˻�idɾ���˻�
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
			m_messageList.append(QStringLiteral("���˺����ݿ���ɾ���˺ų���"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("�����˺����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������account--�˻���Ϣ
* ���أ�
* ���ܣ��޸��˻���Ϣ
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
		// �޸��˻�����
		QString strQuery = "UPDATE account SET userPassward = ? WHERE userName = ?";
		QSqlQuery query(db);
		query.prepare(strQuery);
		query.bindValue(0, account.passward);
		query.bindValue(1, account.userName);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("�޸��û�����ɹ���"));
		}
		else
		{
			m_messageList.append(QStringLiteral("�޸��û�����ʱ����"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("�����˺����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}



/*
* ������
* ���أ�
* ���ܣ�
*/

/*
* �������ڣ�2016-09-14
* ����޸ģ�2016-11-17
* ��      �ߣ�syf
* ��      ����
*/
#include "methodparam.h"



/*
* ������
* ���أ�
* ���ܣ����캯��
*/
MethodParam::MethodParam(QObject *parent)
	: QObject(parent)
{

}


/*
* ������
* ���أ�
* ���ܣ���������
*/
MethodParam::~MethodParam()
{

}


/*
* ������
* ���أ��������ý����Ϣ�б�
* ���ܣ����س�Ա�������ú�Ľ����Ϣ�б�
*/
QStringList& MethodParam::GetMessageList()
{
	return m_messageList;
}


/*
* ������
* ���أ�
* ���ܣ������û����б�
*/
QStringList& MethodParam::GetMethodNameList()
{
	return m_methodNameList;
}


/*
* ������methodList--���أ����Է����б�
* ���أ�
* ���ܣ���ѯ���Է����������ݿ������еĲ��Է���
*/
bool MethodParam::GetMethodList(QStringList &methodList)
{
	bool state = false;
	m_methodNameList.clear();
	m_messageList.clear();

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM parameters";
		QSqlQuery query(strQuery, db);
		m_idMap.clear();
		methodList.clear();

		if (query.exec())
		{
			while (query.next())
			{
				m_idMap.append(query.value(0).toInt());

				QString methodName = query.value(1).toString();
				methodList.append(methodName);
				m_messageList.append(methodName);
			}

			state = true;
			m_messageList.append(QStringLiteral("��ѯ���Է����б�ɹ���"));

		}
		else
		{
			m_messageList.append(QStringLiteral("��ѯ���Է����б����"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Է������ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������index--���Է����б�����
			method--��������Է�������
* ���أ�
* ���ܣ��������ݿ���parameters���id����ѯ���Է�������
*/
bool MethodParam::GetMethodInfo(int index, STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM parameters WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			while (query.next())
			{
				method.id					= query.value(0).toInt();
				method.name			= query.value(1).toString();
				method.plan				= query.value(2).toInt();
				method.standard		= query.value(3).toString();
				method.rate				= query.value(4).toDouble();
				method.timing			= query.value(5).toDouble();
				method.pressure		= query.value(6).toDouble();
				method.cycle				= query.value(7).toInt();
				method.holdingTime	= query.value(8).toDouble();
				method.unit = static_cast<ENUM_PressureUnit>(query.value(9).toInt());
				method.description		= query.value(10).toString();
				method.lastTime		= query.value(11).toString();
			}

			state = true;
			m_messageList.append(QStringLiteral("��ѯ���Է��������ɹ���"));
			
		}
		else
		{
			m_messageList.append(QStringLiteral("��ѯ���Է�����������"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Է������ݿ�ʧ�ܣ�"));
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
bool MethodParam::AddMethod(const STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "INSERT INTO   parameters (name, plan, standard, rate, timing, pressure, cycle, holdingTime, unit, description) \
											VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
		QSqlQuery query(strQuery, db);
		query.bindValue(0, method.name);
		query.bindValue(1, method.plan);
		query.bindValue(2, method.standard);
		query.bindValue(3, method.rate);
		query.bindValue(4, method.timing);
		query.bindValue(5, method.pressure);
		query.bindValue(6, method.cycle);
		query.bindValue(7, method.holdingTime);
		query.bindValue(8, method.unit);
		query.bindValue(9, method.description);


		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("���Ӳ��Է����ɹ���"));
		}
		else
		{
			m_messageList.append(QStringLiteral("���Ӳ��Է�������"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Է������ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������index--���Է����б�����
* ���أ�
* ���ܣ����ݲ����б�����ɾ��һ�����Է���
*/
bool MethodParam::DeleteMethod(int index)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "DELETE  FROM parameters WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("ɾ��һ�����Է����ɹ���"));

		}
		else
		{
			m_messageList.append(QStringLiteral("ɾ�����Է���ʱ����"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Է������ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������index--���Է����б�����
* ���أ�
* ���ܣ����ݲ����б������޸�һ�����Է����Ĳ���
*/
bool MethodParam::UpdateMethodParam(int index, const STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "UPDATE parameters SET name = ?, plan = ?, standard = ?, rate = ?, timing = ?, pressure= ?, \
						   cycle = ?, holdingTime = ?, unit = ?, description = ?  WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.bindValue(0, method.name);
		query.bindValue(1, method.plan);
		query.bindValue(2, method.standard);
		query.bindValue(3, method.rate);
		query.bindValue(4, method.timing);
		query.bindValue(5, method.pressure);
		query.bindValue(6, method.cycle);
		query.bindValue(7, method.holdingTime);
		query.bindValue(8, method.unit);
		query.bindValue(9, method.description);
		query.bindValue(10, id);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("����һ�����Է����ɹ���"));

		}
		else
		{
			m_messageList.append(QStringLiteral("���²��Է���ʱ����"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Է������ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������index--�߼����ò����б�����
*           params--������߼����ò���
* ���أ�
* ���ܣ��������ݿ���advance���id����ѯ�߼����ò���
*/
bool MethodParam::GetAdvanceParams(int index, STRUCT_AdvanceParams &params)
{
	bool state = false;
	m_messageList.clear();

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM advance WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(index);

		if (query.exec())
		{
			while (query.next())
			{
				params.id = query.value(0).toInt();
				params.density = query.value(1).toInt();
				params.waterInTime = query.value(2).toInt();
				params.dropletArea = query.value(3).toDouble();
				params.sensitivity = query.value(4).toInt();
				params.ignorePixes = query.value(5).toInt();
				params.x = query.value(6).toInt();
				params.y = query.value(7).toInt();
				params.r = query.value(8).toInt();
				params.pressureRange = query.value(9).toInt();
			}

			state = true;
			m_messageList.append(QStringLiteral("��ѯ�߼����ò����ɹ���"));

		}
		else
		{
			m_messageList.append(QStringLiteral("��ѯ�߼����ò�������"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Է������ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������index--�߼����ò����б�����
*           params--�߼����ò���
* ���أ�
* ���ܣ�
*/
bool MethodParam::UpdateAdvanceParams(int index, const STRUCT_AdvanceParams &params)
{
	bool state = false;
	m_messageList.clear();

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "UPDATE advance SET density = ?, waterInTime = ?, dropletArea = ?, sensitivity = ?, ignorePixes = ?, x= ?, \
						   						   y = ?, r = ?, pressureRange = ? WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.bindValue(0, params.density);
		query.bindValue(1, params.waterInTime);
		query.bindValue(2, params.dropletArea);
		query.bindValue(3, params.sensitivity);
		query.bindValue(4, params.ignorePixes);
		query.bindValue(5, params.x);
		query.bindValue(6, params.y);
		query.bindValue(7, params.r);
		query.bindValue(8, params.pressureRange);
		query.bindValue(9, index);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("���¸߼����ò����ɹ���"));

		}
		else
		{
			m_messageList.append(QStringLiteral("���¸߼����ò���ʱ����"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Է������ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}
/*
* �������ڣ�2016-09-14
* ����޸ģ�2016-09-14
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
* ���أ�
* ���ܣ�
*/
bool MethodParam::GetMethodList(QStringList &methodList)
{
	bool state = false;
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
				methodList.append(query.value(1).toString());
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
* ������id--������Ŀid
			method--��������Է�������
* ���أ�
* ���ܣ��������ݿ���parameters���id����ѯ���Է�������
*/
bool MethodParam::GetMethodInfo(int id, STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();

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
				method.uint				= query.value(9).toInt();
				method.discription		= query.value(10).toString();
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

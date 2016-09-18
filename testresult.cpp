/*
* �������ڣ�2016-09-14
* ����޸ģ�2016-09-15
* ��      �ߣ�syf
* ��      ����
*/
#include "testresult.h"


/*
* ������
* ���أ�
* ���ܣ���������
*/
TestResult::TestResult(QObject *parent)
	: QObject(parent)
{

}


/*
* ������
* ���أ�
* ���ܣ����캯��
*/
TestResult::~TestResult()
{

}


/*
* ������
* ���أ�
* ���ܣ��������ݿ���������Ϣ
*/
QStringList& TestResult::GetMessageList()
{
	return m_messageList;
}


/*
* ������reportList:���������ı����б�
			startDate:��ѯ��ʼ����
			endDate:��ѯ��������
			methodPlan:���Է�������
			methodName:���Է�������
			userName:�����û�����
* ���أ�
* ���ܣ�
*/
bool TestResult::GetPeportList(QList<STRUCT_Reprot> &reportList, const QDateTime &startDate, const QDateTime &endDate, int methodPlan, const QString &methodName, const QString &userName)
{
	m_messageList.clear();
	reportList.clear();
	bool state = false;

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestResult.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM report WHERE testDate > ? AND testDate < ?";

		if (-1 != methodPlan)
		{
			strQuery += " AND methodPlan = ?";
		}

		if (!methodName.isEmpty())
		{
			strQuery += " AND methodName = ?";
		}

		if (!userName.isEmpty())
		{
			strQuery += " AND userName = ?";
		}

		QSqlQuery query(strQuery, db);
		int index = 0;
		query.bindValue(index++, startDate);
		query.bindValue(index++, endDate);

		if (-1 != methodPlan)
		{
			query.bindValue(index++, methodPlan);
		}

		if (!methodName.isEmpty())
		{
			query.bindValue(index++, methodName);
		}

		if (!userName.isEmpty())
		{
			query.bindValue(index, userName);
		}

		if (query.exec())
		{
			while (query.next())
			{
				STRUCT_Reprot report;
				report.id = query.value(0).toInt();
				report.methodName = query.value(1).toString();
				report.methodPlan = query.value(2).toInt();
				report.testDate = query.value(3).toString();
				report.endMode = query.value(4).toInt();
				report.userName = query.value(5).toString();

				reportList.append(report);
			}

			state = true;
			m_messageList.append(QStringLiteral("�ڲ��Խ�����ݿ��ѯ������Ϣ�ɹ���"));
		}
		else
		{
			m_messageList.append(QStringLiteral("�ڲ��Խ�����ݿ��ѯ������Ϣʧ�ܣ�"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Խ�����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������id--���ݿ����������Ŀ��������
			report--��������
* ���أ�
* ���ܣ����������ţ���ȡ��������
*/
bool TestResult::GetReport(int id, STRUCT_Reprot &report)
{
	m_messageList.clear();
	bool state = false;

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestResult.db");

	if (db.open())
	{
		QString strQuery = "SELECT * FROM report WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			if (query.next())
			{
				report.id = query.value(0).toInt();
				report.methodName = query.value(1).toString();
				report.methodPlan = query.value(2).toInt();
				report.testDate = query.value(3).toString();
				report.endMode = query.value(4).toInt();
				report.userName = query.value(5).toString();
			}

			state = true;
			m_messageList.append(QStringLiteral("�Ӳ��Խ�����ݿ��л�ȡ������Ϣ�ɹ���"));
		}
		else
		{
			m_messageList.append(QStringLiteral("�ڲ��Խ�����ݿ��ȡ������Ϣʧ�ܣ�"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Խ�����ݿ�ʧ�ܣ�"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* ������id--���ݿ����������Ŀ��������
* ���أ�
* ���ܣ����������ţ�ɾ�����е�һ������
*/
bool TestResult::DeleteReport(int id)
{
	m_messageList.clear();
	bool state = false;

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/TestResult.db");

	if (db.open())
	{
		QString strQuery = "DELETE FROM report WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("�ڲ��Խ�����ݿ�ɾ��������Ϣ�ɹ���"));
		}
		else
		{
			m_messageList.append(QStringLiteral("�ڲ��Խ�����ݿ�ɾ��������Ϣʧ�ܣ�"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("���Ӳ��Խ�����ݿ�ʧ�ܣ�"));
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

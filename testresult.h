/*
* �������ڣ�2016-09-14
* ����޸ģ�2016-09-15
* ��      �ߣ�syf
* ��      ����
*/
#ifndef TESTRESULT_H
#define TESTRESULT_H

#include <QObject>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVariant>

#include "common.h"

class TestResult : public QObject
{
	Q_OBJECT

public:
	TestResult(QObject *parent = 0);
	~TestResult();

	QStringList& GetMessageList();
	//bool GetPeportList(QList<STRUCT_Reprot> &reportList, const QString &startDate, const QString &endDate, int methodPlan = -1, const QString &methodName = "", const QString &userName = "");
	bool GetPeportList(QList<STRUCT_Reprot> &reportList, const QDateTime &startDate, const QDateTime &endDate, int methodPlan = -1, const QString &methodName = "", const QString &userName = "");
	bool GetReport(int id,  STRUCT_Reprot &report);
	bool DeleteReport(int id);

private:
	QStringList m_messageList;
};

#endif // TESTRESULT_H

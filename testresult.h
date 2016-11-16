/*
* 创建日期：2016-09-14
* 最后修改：2016-11-11
* 作      者：syf
* 描      述：
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
	bool GetReportList(QList<STRUCT_Report> &reportList, const QDateTime &startDate, const QDateTime &endDate, int methodPlan = -1, const QString &methodName = "", const QString &userName = "");
	bool GetReport(int id,  STRUCT_Report &report);
	bool DeleteReport(int id);
	bool AddReport(const STRUCT_Report &report);

private:
	QStringList m_messageList;
};

#endif // TESTRESULT_H

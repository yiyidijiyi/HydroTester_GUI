/*
* 创建日期：2016-09-14
* 最后修改：2016-11-30
* 作      者：syf
* 描      述：
*/
#include "testresult.h"


/*
* 参数：
* 返回：
* 功能：析构函数
*/
TestResult::TestResult(QObject *parent)
	: QObject(parent)
{

}


/*
* 参数：
* 返回：
* 功能：构造函数
*/
TestResult::~TestResult()
{

}


/*
* 参数：
* 返回：
* 功能：返回数据库操作结果信息
*/
QStringList& TestResult::GetMessageList()
{
	return m_messageList;
}


/*
* 参数：reportList:符合条件的报告列表
			startDate:查询起始日期
			endDate:查询结束日期
			methodPlan:测试方案类型
			methodName:测试方法名称
			userName:测试用户名称
* 返回：
* 功能：
*/
bool TestResult::GetReportList(QList<STRUCT_Report> &reportList, const QDateTime &startDate, const QDateTime &endDate, int methodPlan, const QString &methodName, const QString &userName)
{
	m_messageList.clear();
	reportList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection2"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection2");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection2");
	}
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
				STRUCT_Report report;
				report.id = query.value(0).toInt();
				report.methodName = query.value(1).toString();
				report.methodPlan = query.value(2).toInt();
				report.testDate = query.value(3).toString();
				report.endMode = query.value(4).toInt();
				report.userName = query.value(5).toString();
				report.fileName = query.value(6).toString();
				report.rate = query.value(7).toDouble();
				report.timing = query.value(8).toDouble();
				report.pressure = query.value(9).toDouble();
				report.cycle = query.value(10).toInt();
				report.holdingTime = query.value(11).toDouble();
				report.unit = static_cast<ENUM_PressureUnit>(query.value(12).toInt());
				report.standard = query.value(13).toString();
				report.description = query.value(14).toString();
				report.decTime = query.value(15).toString();
				report.decPressure = query.value(16).toString();
				reportList.append(report);
			}

			state = true;
			m_messageList.append(QStringLiteral("在测试结果数据库查询报告信息成功！"));
		}
		else
		{
			m_messageList.append(QStringLiteral("在测试结果数据库查询报告信息失败！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("连接测试结果数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：id--数据库表中数据条目的索引号
			report--报告内容
* 返回：
* 功能：根据索引号，获取报告内容
*/
bool TestResult::GetReport(int id, STRUCT_Report &report)
{
	m_messageList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection2"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection2");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection2");
	}
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
				report.fileName = query.value(6).toString();
				report.rate = query.value(7).toDouble();
				report.timing = query.value(8).toDouble();
				report.pressure = query.value(9).toDouble();
				report.cycle = query.value(10).toInt();
				report.holdingTime = query.value(11).toDouble();
				report.unit = static_cast<ENUM_PressureUnit>(query.value(12).toInt());
				report.standard = query.value(13).toString();
				report.description = query.value(14).toString();
				report.decTime = query.value(15).toString();
				report.decPressure = query.value(16).toString();
			}

			state = true;
			m_messageList.append(QStringLiteral("从测试结果数据库中获取报告信息成功！"));
		}
		else
		{
			m_messageList.append(QStringLiteral("在测试结果数据库获取报告信息失败！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("连接测试结果数据库失败！"));
		m_messageList.append(db.lastError().text());
	}

	db.close();
	return state;
}


/*
* 参数：id--数据库表中数据条目的索引号
* 返回：
* 功能：根据索引号，删除表中的一条数据
*/
bool TestResult::DeleteReport(int id)
{
	m_messageList.clear();
	bool state = false;

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection2"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection2");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection2");
	}
	db.setDatabaseName("./data/TestResult.db");

	if (db.open())
	{
		QString strQuery = "DELETE FROM report WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("在测试结果数据库删除报告信息成功！"));
		}
		else
		{
			m_messageList.append(QStringLiteral("在测试结果数据库删除报告信息失败！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("连接测试结果数据库失败！"));
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
bool TestResult::AddReport(const STRUCT_Report &report)
{
	bool state = false;
	m_messageList.clear();

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection2"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection2");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection2");
	}
	db.setDatabaseName("./data/TestResult.db");

	if (db.open())
	{
		QString strQuery = "INSERT INTO   report (methodName, methodPlan, testDate,endMode, userName, fileName, rate, timing, pressure, cycle, holdingTime, unit,  standard, description, decTime, decPressure) \
						   											VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
		QSqlQuery query(strQuery, db);
		query.bindValue(0, report.methodName);
		query.bindValue(1, report.methodPlan);
		query.bindValue(2, report.testDate);
		query.bindValue(3, report.endMode);
		query.bindValue(4, report.userName);
		query.bindValue(5, report.fileName);
		query.bindValue(6, report.rate);
		query.bindValue(7, report.timing);
		query.bindValue(8, report.pressure);
		query.bindValue(9, report.cycle);
		query.bindValue(10, report.holdingTime);
		query.bindValue(11, report.unit);
		query.bindValue(12, report.standard);
		query.bindValue(13, report.description);
		query.bindValue(14, report.decTime);
		query.bindValue(15, report.decPressure);


		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("增加测试方法成功！"));
		}
		else
		{
			m_messageList.append(QStringLiteral("增加测试方法出错！"));
			m_messageList.append(query.lastError().text());
		}
	}
	else
	{
		m_messageList.append(QStringLiteral("连接测试方法数据库失败！"));
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

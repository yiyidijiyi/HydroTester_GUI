/*
* 创建日期：2016-09-14
* 最后修改：2016-11-30
* 作      者：syf
* 描      述：
*/
#include "methodparam.h"



/*
* 参数：
* 返回：
* 功能：构造函数
*/
MethodParam::MethodParam(QObject *parent)
	: QObject(parent)
{

}


/*
* 参数：
* 返回：
* 功能：析构函数
*/
MethodParam::~MethodParam()
{

}


/*
* 参数：
* 返回：函数调用结果信息列表
* 功能：返回成员函数调用后的结果信息列表
*/
QStringList& MethodParam::GetMessageList()
{
	return m_messageList;
}


/*
* 参数：
* 返回：
* 功能：返回用户名列表
*/
QStringList& MethodParam::GetMethodNameList()
{
	return m_methodNameList;
}


/*
* 参数：methodList--返回，测试方法列表
* 返回：
* 功能：查询测试方法参数数据库中所有的测试方法
*/
bool MethodParam::GetMethodList(QStringList &methodList)
{
	bool state = false;
	m_methodNameList.clear();
	m_messageList.clear();

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection1"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection1");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection1");
	}
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
			m_messageList.append(QStringLiteral("查询测试方法列表成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("查询测试方法列表出错！"));
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
* 参数：index--测试方法列表索引
			method--输出，测试方法参数
* 返回：
* 功能：根据数据库中parameters表的id，查询测试方法参数
*/
bool MethodParam::GetMethodInfo(int index, STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection1"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection1");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection1");
	}
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
			m_messageList.append(QStringLiteral("查询测试方法参数成功！"));
			
		}
		else
		{
			m_messageList.append(QStringLiteral("查询测试方法参数出错！"));
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
bool MethodParam::AddMethod(const STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection1"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection1");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection1");
	}
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
* 参数：index--测试方法列表索引
* 返回：
* 功能：根据测试列表索引删除一条测试方法
*/
bool MethodParam::DeleteMethod(int index)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection1"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection1");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection1");
	}
	db.setDatabaseName("./data/TestMethod.db");

	if (db.open())
	{
		QString strQuery = "DELETE  FROM parameters WHERE id = ?";
		QSqlQuery query(strQuery, db);
		query.addBindValue(id);

		if (query.exec())
		{
			state = true;
			m_messageList.append(QStringLiteral("删除一条测试方法成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("删除测试方法时出错！"));
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
* 参数：index--测试方法列表索引
* 返回：
* 功能：根据测试列表索引修改一条测试方法的参数
*/
bool MethodParam::UpdateMethodParam(int index, const STRUCT_MethodParam &method)
{
	bool state = false;
	m_messageList.clear();
	int id = m_idMap[index];

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection1"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection1");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection1");
	}
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
			m_messageList.append(QStringLiteral("更新一条测试方法成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("更新测试方法时出错！"));
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
* 参数：index--高级设置参数列表索引
*           params--输出，高级设置参数
* 返回：
* 功能：根据数据库中advance表的id，查询高级设置参数
*/
bool MethodParam::GetAdvanceParams(int index, STRUCT_AdvanceParams &params)
{
	bool state = false;
	m_messageList.clear();

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection1"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection1");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection1");
	}
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
			m_messageList.append(QStringLiteral("查询高级设置参数成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("查询高级设置参数出错！"));
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
* 参数：index--高级设置参数列表索引
*           params--高级设置参数
* 返回：
* 功能：
*/
bool MethodParam::UpdateAdvanceParams(int index, const STRUCT_AdvanceParams &params)
{
	bool state = false;
	m_messageList.clear();

	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QSqlDatabase db;
	if (QSqlDatabase::contains("qt_sql_default_connection1"))
	{
		db = QSqlDatabase::database("qt_sql_default_connection1");
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection1");
	}
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
			m_messageList.append(QStringLiteral("更新高级设置参数成功！"));

		}
		else
		{
			m_messageList.append(QStringLiteral("更新高级设置参数时出错！"));
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